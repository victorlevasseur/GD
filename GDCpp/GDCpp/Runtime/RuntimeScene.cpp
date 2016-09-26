/*
 * GDevelop C++ Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/wx.h> //Must be include first otherwise we get nice errors relative to "cannot convert 'const TCHAR*'..." in wx/msw/winundef.h
#endif
#include <sstream>
#include <fstream>
#include <iomanip>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "GDCpp/Runtime/RuntimeScene.h"
#include "GDCpp/Runtime/RuntimeGame.h"
#include "GDCpp/Runtime/RuntimeLayer.h"
#include "GDCpp/Runtime/Project/Layout.h"
#include "GDCpp/Runtime/Project/Project.h"
#include "GDCpp/Runtime/Project/Object.h"
#include "GDCpp/Runtime/RuntimeObjectHelpers.h"
#include "GDCpp/Runtime/ImageManager.h"
#include "GDCpp/Runtime/SoundManager.h"
#include "GDCpp/Runtime/Project/Layer.h"
#include "GDCpp/Runtime/profile.h"
#include "GDCpp/Runtime/Project/InitialInstance.h"
#include "GDCpp/Runtime/FontManager.h"
#include "GDCpp/Runtime/Project/BehaviorsSharedData.h"
#include "GDCpp/Runtime/BehaviorsRuntimeSharedData.h"
#include "GDCpp/Runtime/RuntimeContext.h"
#include "GDCpp/Runtime/Project/Project.h"
#include "GDCpp/Runtime/ManualTimer.h"
#include "GDCpp/Runtime/Tools/OpenGLTools.h"
#include "GDCpp/Extensions/CppPlatform.h"
#include "GDCore/Tools/Localization.h"
#include "GDCore/Tools/Log.h"
#if !defined(ANDROID) && !defined(MACOS)
#include <GL/glu.h>
#endif

#include "GDCpp/Runtime/CodeExecutionEngine.h"
#if defined(GD_IDE_ONLY)
#include "GDCpp/Events/Builtin/ProfileEvent.h"
#include "GDCpp/IDE/BaseProfiler.h"
#include "GDCpp/IDE/BaseDebugger.h"
#include "GDCpp/Extensions/Builtin/ProfileTools.h"
#endif
#include "GDCpp/Extensions/ExtensionBase.h"
#undef GetObject //Disable an annoying macro

RuntimeLayer RuntimeScene::badRuntimeLayer;

RuntimeScene::RuntimeScene(sf::RenderWindow * renderWindow_, RuntimeGame * game_) :
    renderWindow(renderWindow_),
    game(game_),
    #if defined(GD_IDE_ONLY)
    debugger(NULL),
    #endif
    isFullScreen(false),
    inputManager(renderWindow_),
    codeExecutionEngine(new CodeExecutionEngine)
{
    ChangeRenderWindow(renderWindow);
}

RuntimeScene::~RuntimeScene()
{
	for (std::size_t i = 0;i<game->GetUsedExtensions().size();++i)
    {
        std::shared_ptr<gd::PlatformExtension> gdExtension = CppPlatform::Get().GetExtension(game->GetUsedExtensions()[i]);
        std::shared_ptr<ExtensionBase> extension = std::dynamic_pointer_cast<ExtensionBase>(gdExtension);
        if ( extension != std::shared_ptr<ExtensionBase>() )
            extension->SceneUnloaded(*this);
    }

    objectsInstances.Clear(); //Force destroy objects NOW as they can have pointers to some
                              //RuntimeScene members which so need to be destroyed AFTER objects.
}

std::shared_ptr<gd::ImageManager> RuntimeScene::GetImageManager() const
{
    return game->GetImageManager();
}

void RuntimeScene::ChangeRenderWindow(sf::RenderWindow * newWindow)
{
    renderWindow = newWindow;
    inputManager.SetWindow(newWindow);

    if (!renderWindow) return;

    renderWindow->setTitle(GetWindowDefaultTitle());

    if(game)
    {
        renderWindow->setFramerateLimit(game->GetMaximumFPS());
        renderWindow->setVerticalSyncEnabled(game->IsVerticalSynchronizationEnabledByDefault());
    }
    SetupOpenGLProjection();
}

void RuntimeScene::SetupOpenGLProjection()
{
    #if !defined(ANDROID) //TODO: OpenGL
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double windowRatio = static_cast<double>(renderWindow->getSize().x)/static_cast<double>(renderWindow->getSize().y);
    OpenGLTools::PerspectiveGL(GetOpenGLFOV(), windowRatio, GetOpenGLZNear(), GetOpenGLZFar());
    #endif
}

void RuntimeScene::RequestChange(SceneChange::Change change, gd::String sceneName) {
    requestedChange.change = change;
    requestedChange.requestedScene = sceneName;
}

bool RuntimeScene::RenderAndStep()
{
    requestedChange.change = SceneChange::CONTINUE;
    ManageRenderTargetEvents();
    timeManager.Update(clock.restart().asMicroseconds(), game->GetMinimumFPS());
    ManageObjectsBeforeEvents();
    if (game) game->GetSoundManager().ManageGarbage();

    #if defined(GD_IDE_ONLY)
    if( GetProfiler() )
    {
        if ( timeManager.IsFirstLoop() ) GetProfiler()->Reset();
        GetProfiler()->eventsClock.reset();
    }
    #endif

    GetCodeExecutionEngine()->Execute();

    #if defined(GD_IDE_ONLY)
    if( GetProfiler() && GetProfiler()->profilingActivated )
    {
        GetProfiler()->lastEventsTime = GetProfiler()->eventsClock.getTimeMicroseconds();
        GetProfiler()->renderingClock.reset();
    }
    #endif

    ManageObjectsAfterEvents();

    #if defined(GD_IDE_ONLY)
    if( debugger ) debugger->Update();
    #endif

    //Rendering
    Render();

    #if defined(GD_IDE_ONLY)
    if( GetProfiler() && GetProfiler()->profilingActivated )
    {
        GetProfiler()->lastRenderingTime = GetProfiler()->renderingClock.getTimeMicroseconds();
        GetProfiler()->totalSceneTime += GetProfiler()->lastRenderingTime + GetProfiler()->lastEventsTime;
        GetProfiler()->totalEventsTime += GetProfiler()->lastEventsTime;
        GetProfiler()->Update();
    }
    #endif

    return requestedChange.change != SceneChange::CONTINUE;
}

void RuntimeScene::ManageRenderTargetEvents()
{
    if (!renderWindow) return;
    inputManager.NextFrame();

    sf::Event event;
    while (renderWindow->pollEvent(event))
    {
        if ( event.type == sf::Event::Closed )
        {
            //Handle window closing
            RequestChange(SceneChange::STOP_GAME);
            renderWindow->close();
        }
        else if (event.type == sf::Event::Resized)
        {
            //Resetup OpenGL when window is resized
            SetupOpenGLProjection();
        }
        else
        {
            //Most events will be input related and should be forwarded
            //to the InputManager:
            inputManager.HandleEvent(event);
        }
    }
}


void RuntimeScene::RenderWithoutStep()
{
    ManageRenderTargetEvents();
    Render();

    #if defined(GD_IDE_ONLY)
    if( debugger )
        debugger->Update();
    #endif
}

void RuntimeScene::Render()
{
    if (!renderWindow) return;

    renderWindow->clear( sf::Color( GetBackgroundColorRed(), GetBackgroundColorGreen(), GetBackgroundColorBlue() ) );

    //Sort object by order to render them
    RuntimeObjNonOwningPtrList allObjects = objectsInstances.GetAllObjects();
    OrderObjectsByZOrder(allObjects);

    #if !defined(ANDROID) //TODO: OpenGL
    //To allow using OpenGL to draw:
    glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer
    renderWindow->pushGLStates();
    #endif
    renderWindow->setActive();

    //Draw layer by layer
    for (std::size_t layerIndex =0;layerIndex<layers.size();++layerIndex)
    {
        if ( layers[layerIndex].GetVisibility() )
        {
            for (std::size_t cameraIndex = 0;cameraIndex < layers[layerIndex].GetCameraCount();++cameraIndex)
            {
                RuntimeCamera & camera = layers[layerIndex].GetCamera(cameraIndex);

                //Prepare OpenGL rendering
                #if !defined(ANDROID) //TODO: OpenGL
                renderWindow->popGLStates();

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                OpenGLTools::PerspectiveGL(GetOpenGLFOV(), camera.GetWidth()/camera.GetHeight(), GetOpenGLZNear(), GetOpenGLZFar());
                #endif

                const sf::FloatRect & viewport = camera.GetSFMLView().getViewport();

                #if !defined(ANDROID) //TODO: OpenGL
                glViewport(viewport.left*renderWindow->getSize().x,
                           renderWindow->getSize().y-(viewport.top+viewport.height)*renderWindow->getSize().y, //Y start from bottom
                           viewport.width*renderWindow->getSize().x,
                           viewport.height*renderWindow->getSize().y);

                renderWindow->pushGLStates();
                #endif

                //Prepare SFML rendering
                renderWindow->setView(camera.GetSFMLView());

                //Rendering all objects
                for (std::size_t id = 0;id < allObjects.size();++id)
                {
                    if (allObjects[id]->GetLayer() == layers[layerIndex].GetName())
                        allObjects[id]->Draw(*renderWindow);
                }
            }
        }
    }

    // Display window contents on screen
    //TODO: If nothing is displayed, double check popGLStates.
    #if !defined(ANDROID) //TODO: OpenGL
    renderWindow->popGLStates();
    #endif
    renderWindow->display();
}

bool RuntimeScene::OrderObjectsByZOrder(RuntimeObjNonOwningPtrList & objList)
{
    if ( StandardSortMethod() )
        std::sort( objList.begin(), objList.end(), [](const RuntimeObject * o1, const RuntimeObject * o2) {
            return o1->GetZOrder() < o2->GetZOrder();
        });
    else
        std::stable_sort( objList.begin(), objList.end(), [](const RuntimeObject * o1, const RuntimeObject * o2) {
            return o1->GetZOrder() < o2->GetZOrder();
        });

    return true;
}

RuntimeLayer & RuntimeScene::GetRuntimeLayer(const gd::String & name)
{
    for (std::size_t i = 0;i<layers.size();++i)
    {
        if ( layers[i].GetName() == name )
            return layers[i];
    }

    return badRuntimeLayer;
}

void RuntimeScene::ManageObjectsAfterEvents()
{
    //Delete objects that were removed.
    RuntimeObjNonOwningPtrList allObjects = objectsInstances.GetAllObjects();
    for (std::size_t id = 0;id<allObjects.size();++id)
    {
    	if ( allObjects[id]->GetName().empty() )
        {
            for (std::size_t i = 0;i<extensionsToBeNotifiedOnObjectDeletion.size();++i)
                extensionsToBeNotifiedOnObjectDeletion[i]->ObjectDeletedFromScene(*this, allObjects[id]);

            objectsInstances.RemoveObject(allObjects[id]); //Remove from objects instances, not from the temporary list!
        }
    }

    //Update objects positions, forces and behaviors
    allObjects = objectsInstances.GetAllObjects();
    double elapsedTime = static_cast<double>(timeManager.GetElapsedTime())/1000000.0;
    for (std::size_t id = 0;id<allObjects.size();++id)
    {
        allObjects[id]->SetX( allObjects[id]->GetX() + (allObjects[id]->TotalForceX() * elapsedTime));
        allObjects[id]->SetY( allObjects[id]->GetY() + (allObjects[id]->TotalForceY() * elapsedTime));
        allObjects[id]->UpdateTime(elapsedTime);
        allObjects[id]->UpdateForce(elapsedTime);
        allObjects[id]->DoBehaviorsPostEvents(*this);
    }
}

void RuntimeScene::ManageObjectsBeforeEvents()
{
    RuntimeObjNonOwningPtrList allObjects = objectsInstances.GetAllObjects();
    for (std::size_t id = 0;id<allObjects.size();++id)
        allObjects[id]->DoBehaviorsPreEvents(*this);
}

/**
 * \brief Internal Tool class used by RuntimeScene::CreateObjectsFrom
 */
class ObjectsFromInitialInstanceCreator : public gd::InitialInstanceFunctor
{
public:
    ObjectsFromInitialInstanceCreator(gd::Project & game_, RuntimeScene & scene_, float xOffset_, float yOffset_) :
        game(game_),
        scene(scene_),
        xOffset(xOffset_),
        yOffset(yOffset_)
    {};
    virtual ~ObjectsFromInitialInstanceCreator() {};

    virtual void operator()(gd::InitialInstance & instance)
    {
        std::vector<ObjSPtr>::const_iterator sceneObject = std::find_if(scene.GetObjects().begin(), scene.GetObjects().end(), std::bind2nd(ObjectHasName(), instance.GetObjectName()));
        std::vector<ObjSPtr>::const_iterator globalObject = std::find_if(game.GetObjects().begin(), game.GetObjects().end(), std::bind2nd(ObjectHasName(), instance.GetObjectName()));

        RuntimeObjSPtr newObject;

        if ( sceneObject != scene.GetObjects().end() ) //We check first scene's objects' list.
            newObject = CppPlatform::Get().CreateRuntimeObject(scene, **sceneObject);
        else if ( globalObject != scene.game->GetObjects().end() ) //Then the global object list
            newObject = CppPlatform::Get().CreateRuntimeObject(scene, **globalObject);

        if ( newObject != std::unique_ptr<RuntimeObject> () )
        {
            newObject->SetX( instance.GetX() + xOffset );
            newObject->SetY( instance.GetY() + yOffset );
            newObject->SetZOrder( instance.GetZOrder() );
            newObject->SetLayer( instance.GetLayer() );
            newObject->ExtraInitializationFromInitialInstance(instance);
            newObject->SetAngle( instance.GetAngle() );

            if ( instance.HasCustomSize() )
            {
                newObject->SetWidth(instance.GetCustomWidth());
                newObject->SetHeight(instance.GetCustomHeight());
            }

            //Substitute initial variables specific to that object instance.
            newObject->GetVariables().Merge(instance.GetVariables());

            scene.objectsInstances.AddObject(std::move(newObject));
        }
        else
            std::cout << "Could not find and put object " << instance.GetObjectName() << std::endl;
    }

private:
    gd::Project & game;
    RuntimeScene & scene;
    float xOffset;
    float yOffset;
};

void RuntimeScene::CreateObjectsFrom(const gd::InitialInstancesContainer & container, float xOffset, float yOffset)
{
    ObjectsFromInitialInstanceCreator func(*game, *this, xOffset, yOffset);
    const_cast<gd::InitialInstancesContainer&>(container).IterateOverInstances(func);
}

bool RuntimeScene::LoadFromScene( const gd::Layout & scene )
{
    return LoadFromSceneAndCustomInstances(scene, scene.GetInitialInstances());
}

bool RuntimeScene::LoadFromSceneAndCustomInstances( const gd::Layout & scene, const gd::InitialInstancesContainer & instances )
{
    std::cout << "Loading RuntimeScene from a scene.";
    if (!game)
    {
        std::cout << "..No valid gd::Project associated to the RuntimeScene. Aborting loading." << std::endl;
        return false;
    }

    //Copy inherited scene
    Scene::operator=(scene);

    //Clear RuntimeScene datas
    objectsInstances.Clear();
    timeManager.Reset();

    std::cout << ".";
    codeExecutionEngine->runtimeContext.scene = this;
    inputManager.DisableInputWhenFocusIsLost(IsInputDisabledWhenFocusIsLost());

    //Initialize variables
    variables = scene.GetVariables();

    //Initialize layers
    std::cout << ".";
    layers.clear();
    sf::View defaultView( sf::FloatRect( 0.0f, 0.0f, game->GetMainWindowDefaultWidth(), game->GetMainWindowDefaultHeight() ) );
    for (std::size_t i = 0;i<GetLayersCount();++i) {
        layers.push_back(RuntimeLayer(GetLayer(i), defaultView));
    }

    //Create object instances which are originally positioned on scene
    std::cout << ".";
    CreateObjectsFrom(instances);

    //Behaviors shared data
    std::cout << ".";
    behaviorsSharedDatas.LoadFrom(scene.behaviorsInitialSharedDatas);

    std::cout << ".";
    //Extensions specific initialization
	for (std::size_t i = 0;i<game->GetUsedExtensions().size();++i)
    {
        std::shared_ptr<gd::PlatformExtension> gdExtension = CppPlatform::Get().GetExtension(game->GetUsedExtensions()[i]);
        std::shared_ptr<ExtensionBase> extension = std::dynamic_pointer_cast<ExtensionBase>(gdExtension);
        if ( extension != std::shared_ptr<ExtensionBase>() )
        {
            extension->SceneLoaded(*this);
            if ( extension->ToBeNotifiedOnObjectDeletion() ) extensionsToBeNotifiedOnObjectDeletion.push_back(extension.get());
        }
    }

    std::cout << ".";
    if ( StopSoundsOnStartup() ) {game->GetSoundManager().ClearAllSoundsAndMusics(); }
    if ( renderWindow ) renderWindow->setTitle(GetWindowDefaultTitle());

    std::cout << " Done." << std::endl;

    return true;
}
