/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/msgdlg.h> //Must be placed first
#endif
#include <vector>
#include "GDCore/Tools/Log.h"
#include "GDCpp/BuiltinExtensions/RuntimeSceneTools.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/BuiltinExtensions/CommonInstructionsTools.h"
#include "GDCpp/RuntimeObject.h"
#include "GDCpp/RuntimeLayer.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/CppPlatform.h"
#include "GDCpp/ObjectHelpers.h"
#include "GDCpp/ObjectsListsTools.h"
#include "GDCpp/RuntimeGame.h"
#include "GDCpp/profile.h"
#include "GDCpp/CommonTools.h"
#include "GDCpp/Variable.h"
#include "GDCpp/Text.h"
#include "GDCpp/CppPlatform.h"

gd::String GD_API GetSceneName(RuntimeScene & scene)
{
    return scene.GetName();
}

bool GD_API LayerVisible( RuntimeScene & scene, const gd::String & layer )
{
    return scene.GetRuntimeLayer(layer).GetVisibility();
}

void GD_API ShowLayer( RuntimeScene & scene, const gd::String & layer )
{
    scene.GetRuntimeLayer(layer).SetVisibility(true);
}

void GD_API HideLayer( RuntimeScene & scene, const gd::String & layer )
{
    scene.GetRuntimeLayer(layer).SetVisibility(false);
}

void GD_API ChangeSceneBackground( RuntimeScene & scene, gd::String newColor )
{
    std::vector < gd::String > colors = newColor.Split(U';');
    if ( colors.size() > 2 ) scene.SetBackgroundColor( colors[0].To<int>(), colors[1].To<int>(), colors[2].To<int>() );

    return;
}

void GD_API StopGame( RuntimeScene & scene )
{
    scene.RequestChange(RuntimeScene::SceneChange::STOP_GAME);
}

void GD_API ReplaceScene(RuntimeScene & scene, gd::String newSceneName, bool clearOthers)
{
    if (!scene.game->HasLayoutNamed(newSceneName)) return;
    scene.RequestChange(clearOthers ?
        RuntimeScene::SceneChange::CLEAR_SCENES :
        RuntimeScene::SceneChange::REPLACE_SCENE, newSceneName);
}

void GD_API PushScene(RuntimeScene & scene, gd::String newSceneName)
{
    if (!scene.game->HasLayoutNamed(newSceneName)) return;
    scene.RequestChange(RuntimeScene::SceneChange::PUSH_SCENE, newSceneName);
}

void GD_API PopScene(RuntimeScene & scene)
{
    scene.RequestChange(RuntimeScene::SceneChange::POP_SCENE);
}

bool GD_API SceneJustBegins(RuntimeScene & scene )
{
    return scene.IsFirstLoop();
}

void GD_API MoveObjects( RuntimeScene & scene )
{
    RuntimeObjList allObjects = scene.objectsInstances.GetAllObjects();

    for (unsigned int id = 0;id < allObjects.size();++id)
    {
        allObjects[id]->SetX( allObjects[id]->GetX() + allObjects[id]->TotalForceX() * static_cast<double>(scene.GetElapsedTime())/1000000.0 );
        allObjects[id]->SetY( allObjects[id]->GetY() + allObjects[id]->TotalForceY() * static_cast<double>(scene.GetElapsedTime())/1000000.0 );

        allObjects[id]->UpdateForce( static_cast<double>(scene.GetElapsedTime())/1000000.0 );
    }

    return;
}

namespace {

void DoCreateObjectOnScene(RuntimeScene & scene, gd::String objectName, std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists, float positionX, float positionY, const gd::String & layer)
{
    if ( pickedObjectLists.empty() ) return;

    //Find the object to be created
    std::vector<ObjSPtr>::const_iterator sceneObject = std::find_if(scene.GetObjects().begin(), scene.GetObjects().end(), std::bind2nd(ObjectHasName(), objectName));
    std::vector<ObjSPtr>::const_iterator globalObject = std::find_if(scene.game->GetObjects().begin(), scene.game->GetObjects().end(), std::bind2nd(ObjectHasName(), objectName));

    RuntimeObjSPtr newObject = std::shared_ptr<RuntimeObject> ();

    if ( sceneObject != scene.GetObjects().end() ) //We check first scene's objects' list.
        newObject = CppPlatform::Get().CreateRuntimeObject(scene, **sceneObject);
    else if ( globalObject != scene.game->GetObjects().end() ) //Then the global object list
        newObject = CppPlatform::Get().CreateRuntimeObject(scene, **globalObject);

    if ( newObject == std::shared_ptr<RuntimeObject> () )
        return; //Unable to create the object

    //Set up the object
    newObject->SetX( positionX );
    newObject->SetY( positionY );
    newObject->SetLayer( layer );

    //Add object to scene and let it be concerned by futures actions
    scene.objectsInstances.AddObject(newObject);
    pickedObjectLists[objectName]->push_back( newObject.get() );
}


}

void GD_API CreateObjectOnScene(RuntimeScene & scene, std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists, float positionX, float positionY, const gd::String & layer)
{
    if ( pickedObjectLists.empty() ) return;

    ::DoCreateObjectOnScene(scene, pickedObjectLists.begin()->first, pickedObjectLists, positionX, positionY, layer);
}

void GD_API CreateObjectFromGroupOnScene(RuntimeScene & scene, std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists, const gd::String & objectWanted, float positionX, float positionY, const gd::String & layer)
{
    if ( pickedObjectLists[objectWanted] == NULL ) return; //Bail out if the object is not present in the specified group

    ::DoCreateObjectOnScene(scene, objectWanted, pickedObjectLists, positionX, positionY, layer);
}

bool GD_API PickAllObjects(RuntimeScene & scene, std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists)
{
    for (auto it = pickedObjectLists.begin();it!=pickedObjectLists.end();++it)
    {
        if ( it->second != NULL )
        {
            std::vector<RuntimeObject*> objectsOnScene = scene.objectsInstances.GetObjectsRawPointers(it->first);

            for (unsigned int j = 0;j<objectsOnScene.size();++j)
            {
                if ( find(it->second->begin(), it->second->end(), objectsOnScene[j]) == it->second->end() )
                    it->second->push_back(objectsOnScene[j]);
            }
        }
    }

    return true;
}

bool GD_API PickRandomObject(RuntimeScene &, std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists)
{
    //Create a list with all objects
    std::vector<RuntimeObject*> allObjects;
    for (auto it = pickedObjectLists.begin();it!=pickedObjectLists.end();++it)
    {
        if ( it->second != NULL )
            std::copy(it->second->begin(), it->second->end(), std::back_inserter(allObjects));
    }

    if ( allObjects.empty() )
        return false;

    unsigned int id = GDpriv::CommonInstructions::Random(allObjects.size()-1);
    PickOnly(pickedObjectLists, allObjects[id]);
    return true;
}

bool GD_API PickNearestObject(std::map <gd::String, std::vector<RuntimeObject*> *> pickedObjectLists, double x, double y, bool inverted)
{
    double best = 0;
    bool first = true;
    RuntimeObject * bestObject = NULL;
    for (auto it = pickedObjectLists.begin();it!=pickedObjectLists.end();++it)
    {
        if ( it->second == NULL ) continue;
        auto list = *it->second;

        for (unsigned int i = 0;i<list.size();++i)
        {
            double value = list[i]->GetSqDistanceTo(x, y);
            if (first || ((value < best) ^ inverted)) {
                bestObject = list[i];
                best = value;
            }

            first = false;
        }
    }

    if (!bestObject)
        return false;

    PickOnly(pickedObjectLists, bestObject);
    return true;
}

bool GD_API SceneVariableExists(RuntimeScene & scene, const gd::String & variable)
{
    return scene.GetVariables().Has(variable);
}

bool GD_API GlobalVariableExists(RuntimeScene & scene, const gd::String & variable)
{
    return scene.game->GetVariables().Has(variable);
}

gd::Variable & GD_API ReturnVariable(gd::Variable & variable)
{
    return variable;
};

bool GD_API VariableChildExists(const gd::Variable & variable, const gd::String & childName)
{
    return variable.HasChild(childName);
}

void GD_API VariableRemoveChild(gd::Variable & variable, const gd::String & childName)
{
    variable.RemoveChild(childName);
}

double GD_API GetVariableValue(const gd::Variable & variable)
{
    return variable.GetValue();
};

const gd::String& GD_API GetVariableString(const gd::Variable & variable)
{
    return variable.GetString();
};

void GD_API SetWindowIcon(RuntimeScene & scene, const gd::String & imageName)
{
    //Retrieve the image
    std::shared_ptr<SFMLTextureWrapper> image = scene.GetImageManager()->GetSFMLTexture(imageName);
    if ( image == std::shared_ptr<SFMLTextureWrapper>() )
        return;

    scene.renderWindow->setIcon(image->image.getSize().x, image->image.getSize().y, image->image.getPixelsPtr());
}

void GD_API SetWindowTitle(RuntimeScene & scene, const gd::String & newName)
{
    scene.SetWindowDefaultTitle( newName );
    if (scene.renderWindow != NULL) scene.renderWindow->setTitle(scene.GetWindowDefaultTitle());
}

const gd::String & GD_API GetWindowTitle(RuntimeScene & scene)
{
    return scene.GetWindowDefaultTitle();
}

void GD_API SetWindowSize( RuntimeScene & scene, int windowWidth, int windowHeight, bool useTheNewSizeForCameraDefaultSize)
{
    #if !defined(GD_IDE_ONLY)
    if ( useTheNewSizeForCameraDefaultSize ) //Change future cameras default size if wanted.
    {
        scene.game->SetDefaultWidth( windowWidth );
        scene.game->SetDefaultHeight( windowHeight );
    }

    //Avoid recreating every tick a new window if the size has not changed!
    if ( windowWidth == scene.renderWindow->getSize().x && windowHeight == scene.renderWindow->getSize().y )
        return;

    if ( scene.RenderWindowIsFullScreen() )
    {
        scene.renderWindow->create( sf::VideoMode( windowWidth, windowHeight, 32 ), scene.GetWindowDefaultTitle(), sf::Style::Close | sf::Style::Fullscreen );
        scene.ChangeRenderWindow(scene.renderWindow);
    }
    else
    {
        scene.renderWindow->create( sf::VideoMode( windowWidth, windowHeight, 32 ), scene.GetWindowDefaultTitle(), sf::Style::Close );
        scene.ChangeRenderWindow(scene.renderWindow);
    }
    #endif
}

void GD_API SetFullScreen(RuntimeScene & scene, bool fullscreen, bool)
{
    #if !defined(GD_IDE_ONLY)
    if ( fullscreen && !scene.RenderWindowIsFullScreen() )
    {
        scene.SetRenderWindowIsFullScreen();
        scene.renderWindow->create( sf::VideoMode( scene.game->GetMainWindowDefaultWidth(), scene.game->GetMainWindowDefaultHeight(), 32 ), scene.GetWindowDefaultTitle(), sf::Style::Close | sf::Style::Fullscreen );
        scene.ChangeRenderWindow(scene.renderWindow);
    }
    else if ( !fullscreen && scene.RenderWindowIsFullScreen() )
    {
        scene.SetRenderWindowIsFullScreen(false);
        scene.renderWindow->create( sf::VideoMode( scene.game->GetMainWindowDefaultWidth(), scene.game->GetMainWindowDefaultHeight(), 32 ), scene.GetWindowDefaultTitle(), sf::Style::Close );
        scene.ChangeRenderWindow(scene.renderWindow);
    }
    #endif
}
unsigned int GD_API GetSceneWindowWidth(RuntimeScene & scene)
{
    if ( scene.renderWindow != NULL )
        return scene.renderWindow->getSize().x;

    return 0;
}

unsigned int GD_API GetSceneWindowHeight(RuntimeScene & scene)
{
    if ( scene.renderWindow != NULL )
        return scene.renderWindow->getSize().y;

    return 0;
}

unsigned int GD_API GetScreenWidth()
{
    sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();

    return videoMode.width;
}

unsigned int GD_API GetScreenHeight()
{
    sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();

    return videoMode.height;
}

unsigned int GD_API GetScreenColorDepth()
{
    sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();

    return videoMode.bitsPerPixel;
}

void GD_API DisplayLegacyTextOnScene( RuntimeScene & scene, const gd::String & str, float x, float y, const gd::String & color, float characterSize, const gd::String & fontName, const gd::String & layer)
{
    Text texte;
    texte.text.setString(str);
    texte.text.setPosition(x, y);

    std::vector < gd::String > colors = color.Split(U';');
    if ( colors.size() > 2 ) texte.text.setColor(sf::Color(colors[0].To<int>(), colors[1].To<int>(), colors[2].To<int>() ));

    texte.text.setCharacterSize(characterSize);
    texte.fontName = fontName;
    texte.layer = layer;

    scene.DisplayText(texte);

    return;
}

void GD_API DisableInputWhenFocusIsLost( RuntimeScene & scene, bool disable )
{
    scene.DisableInputWhenFocusIsLost(disable);
}

#if defined(GD_IDE_ONLY)
bool GD_API WarnAboutInfiniteLoop( RuntimeScene & scene )
{
    #if !defined(GD_NO_WX_GUI)
    if (wxMessageBox(_("A \"While\" event was repeated 100000 times: You may have created an infinite loop, which is repeating itself indefinitely and which is going to freeze the software.\n"
                       "\n"
                       "If you want to stop the preview to correct the issue, click on Yes.\n"
                       "If you want to continue the preview, click on No.\n"
                       "You can deactivate this warning by double clicking on While events.\n"
                       "\n"
                       "Stop the preview?"), _("Infinite loop"), wxYES_NO|wxICON_EXCLAMATION ) == wxYES)
    {
        scene.RequestChange(RuntimeScene::SceneChange::STOP_GAME);
        return true;
    }
    #else
    gd::LogWarning("While event repeated 100000 times!");
    #endif

    return false;
}
#endif
