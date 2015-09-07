/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef GDCORE_LAYOUT_H
#define GDCORE_LAYOUT_H
#include "GDCore/String.h"
#include <vector>
#include <map>
#include <memory>
#include "GDCore/Events/EventsList.h"
#include "GDCore/PlatformDefinition/ObjectGroup.h"
#include "GDCore/PlatformDefinition/ClassWithObjects.h"
#include "GDCore/PlatformDefinition/BehaviorsSharedData.h"
#include "GDCore/PlatformDefinition/VariablesContainer.h"
#include "GDCore/PlatformDefinition/InitialInstancesContainer.h"
#include "GDCore/PlatformDefinition/Layer.h"
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include "GDCore/IDE/Dialogs/LayoutEditorCanvas/LayoutEditorCanvasOptions.h"
#endif
namespace gd { class BaseEvent; }
namespace gd { class Object; }
namespace gd { class Project; }
namespace gd { class InitialInstancesContainer; }
class TiXmlElement;
class BaseProfiler;
#undef GetObject //Disable an annoying macro

namespace gd
{

/**
 * \brief Represent a layout ( also called a scene ) of a project.
 *
 * \ingroup PlatformDefinition
 */
class GD_CORE_API Layout : public ClassWithObjects
{
public:
    Layout();
    Layout(const Layout&);
    virtual ~Layout();
    Layout& operator=(const Layout & rhs);

    /**
     * \brief Return a pointer to a copy of the layout.
     */
    Layout * Clone() const { return new Layout(*this); };

    /** \name Common properties
     * Members functions related to common properties of layouts
     */
    ///@{

    /**
     * Change the name of the layout with the name passed as parameter.
     */
    void SetName(const gd::String & name_);

    /**
     * Return the name of the layout.
     */
    const gd::String & GetName() const {return name;};

    /**
     * Return the name of the layout mangled by SceneNameMangler.
     */
    const gd::String & GetMangledName() const {return mangledName;};

    /**
     * Set the background color
     */
    void SetBackgroundColor(unsigned int r, unsigned int g, unsigned int b) { backgroundColorR = r; backgroundColorG = g; backgroundColorB = b; }

    /**
     * Get the background color red component
     */
    unsigned int GetBackgroundColorRed() const { return backgroundColorR; }

    /**
     * Get the background color green component
     */
    unsigned int GetBackgroundColorGreen() const { return backgroundColorG; }

    /**
     * Get the background color blue component
     */
    unsigned int GetBackgroundColorBlue() const { return backgroundColorB; }

    /**
     * Get scene window default title
     */
    const gd::String & GetWindowDefaultTitle() const {return title;};

    /**
     * Set scene window default title
     */
    void SetWindowDefaultTitle(const gd::String & title_) {title = title_;};

    ///@}

    /** \name Layout's initial instances
     * Members functions related to initial instances of objects created at the layout start up
     */
    ///@{
    /**
     * Return the container storing initial instances.
     */
    const gd::InitialInstancesContainer & GetInitialInstances() const { return initialInstances; }

    /**
     * Return the container storing initial instances.
     */
    gd::InitialInstancesContainer & GetInitialInstances() { return initialInstances; }
    ///@}

    /** \name Layout's events
     * Members functions related to events management.
     */
    ///@{

#if defined(GD_IDE_ONLY)
    /**
     * Get the events of the layout
     */
    const gd::EventsList & GetEvents() const { return events; }

    /**
     * Get the events of the layout
     */
    gd::EventsList & GetEvents() { return events; }
#endif
    ///@}

    /** \name Layout objects groups management
     * Members functions related to layout objects groups management.
     */
    ///@{

    /**
     * Return a reference to the vector containing the layout's objects groups.
     */
    std::vector <ObjectGroup> & GetObjectGroups() { return objectGroups; }

    /**
     * Return a const reference to the vector containing the layout's objects groups.
     */
    const std::vector <ObjectGroup> & GetObjectGroups() const { return objectGroups; }

    ///@}

    /** \name Variable management
     * Members functions related to layout variables management.
     */
    ///@{

    /**
     * Provide access to the gd::VariablesContainer member containing the layout variables
     * \see gd::VariablesContainer
     */
    inline const gd::VariablesContainer & GetVariables() const { return variables; }

    /**
     * Provide access to the gd::VariablesContainer member containing the layout variables
     * \see gd::VariablesContainer
     */
    inline gd::VariablesContainer & GetVariables() { return variables; }

    ///@}

    /** \name Layout layers management
     * Members functions related to layout layers management.
     */
    ///@{

    /**
     * Must return true if the layer called "name" exists.
     */
    bool HasLayerNamed(const gd::String & name) const;

    /**
     * Must return a reference to the layer called "name".
     */
    Layer & GetLayer(const gd::String & name);

    /**
     * Must return a reference to the layer called "name".
     */
    const Layer & GetLayer(const gd::String & name) const;

    /**
     * Must return a reference to the layer at position "index" in the layers list
     */
    Layer & GetLayer(std::size_t index);

    /**
     * Must return a reference to the layer at position "index" in the layers list
     */
    const Layer & GetLayer (std::size_t index) const;

    /**
     * Must return the position of the layer called "name" in the layers list
     */
    std::size_t GetLayerPosition(const gd::String & name) const;

    /**
     * Must return the number of layers.
     */
    std::size_t GetLayersCount() const;

    /**
     * Must add a new empty the layer sheet called "name" at the specified position in the layout list.
     */
    void InsertNewLayer(const gd::String & name, std::size_t position);

    /**
     * Must add a new the layer constructed from the layout passed as parameter.
     * \note No pointer or reference must be kept on the layer passed as parameter.
     * \param theLayer The the layer that must be copied and inserted into the project
     * \param position Insertion position. Even if the position is invalid, the layer must be inserted at the end of the layers list.
     */
    void InsertLayer(const Layer & theLayer, std::size_t position);

    /**
     * Must delete the layer named "name".
     */
    void RemoveLayer(const gd::String & name);

    /**
     * Must swap the position of the specified layers.
     */
    void SwapLayers(std::size_t firstLayerIndex, std::size_t secondLayerIndex);
    ///@}

    /**
     * Make sure that the scene had an instance of shared data for
     * every behavior of every object that can be used on the scene
     * ( i.e. the objects of the scene and the global objects )
     *
     * Must be called when a behavior have been added/deleted
     * or when a scene have been added to a project.
     */
    void UpdateBehaviorsSharedData(gd::Project & project);

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
    /**
     * Return the settings associated to the layout.
     * \see gd::LayoutEditorCanvasOptions
     */
    const gd::LayoutEditorCanvasOptions & GetAssociatedLayoutEditorCanvasOptions() const { return associatedSettings; }

    /**
     * Return the settings associated to the layout.
     * \see gd::LayoutEditorCanvasOptions
     */
    gd::LayoutEditorCanvasOptions & GetAssociatedLayoutEditorCanvasOptions() { return associatedSettings; }
#endif

    /** \name Other properties
     */
    ///@{
    /**
     * Set if the input must be disabled when window lose focus.
     */
    void DisableInputWhenFocusIsLost(bool disable = true) { disableInputWhenNotFocused = disable; }

    /**
     * Return true if the input must be disabled when window lost focus.
     */
    bool IsInputDisabledWhenFocusIsLost() { return disableInputWhenNotFocused; }

    /**
     * Set if the objects z-order are sorted using the standard method
     */
    void SetStandardSortMethod(bool enable = true) { standardSortMethod = enable; }

    /**
     * Return true if the objects z-order are sorted using the standard method
     */
    bool StandardSortMethod() const { return standardSortMethod; }

    /**
     * Set if the scene must stop all the sounds being played when it is launched.
     */
    void SetStopSoundsOnStartup(bool enable = true) { stopSoundsOnStartup = enable; }

    /**
     * Return true if the scene must stop all the sounds being played when it is launched
     */
    bool StopSoundsOnStartup() const { return stopSoundsOnStartup; }

    /**
     * Set OpenGL default field of view
     */
    void SetOpenGLFOV(float oglFOV_) { oglFOV = oglFOV_; }

    /**
     * Get OpenGL default field of view
     */
    float GetOpenGLFOV() const { return oglFOV; }

    /**
     * Set OpenGL near clipping plan
     */
    void SetOpenGLZNear(float oglZNear_) { oglZNear = oglZNear_; }

    /**
     * Get OpenGL near clipping plan
     */
    float GetOpenGLZNear() const { return oglZNear; }

    /**
     * Set OpenGL far clipping plan
     */
    void SetOpenGLZFar(float oglZFar_) { oglZFar = oglZFar_; }

    /**
     * Get OpenGL far clipping plan
     */
    float GetOpenGLZFar() const { return oglZFar; }
    ///@}

    /** \name Saving and loading
     * Members functions related to saving and loading the object.
     */
    ///@{
    #if defined(GD_IDE_ONLY)
    /**
     * \brief Serialize the layout.
     */
    void SerializeTo(SerializerElement & element) const;
    #endif

    /**
     * \brief Unserialize the layout.
     */
    void UnserializeFrom(gd::Project & project, const SerializerElement & element);
    ///@}

    //TODO: Send this to private part.
    std::map < gd::String, std::shared_ptr<gd::BehaviorsSharedData> > behaviorsInitialSharedDatas; ///< Initial shared datas of behaviors

    //TODO: GD C++ Platform specific code below
    #if defined(GD_IDE_ONLY)
    /** \name Events compilation and bitcode management
     * Members functions related to managing the compilation of events and the resulting bitcodes.
     *
     * \see CodeCompilationHelpers
     */
    ///@{

    /**
     * Set that the events need to be compiled.
     * \note The compilation is not launched at this time. It will for example occur when triggered by SceneEditorCanvas
     * or if you manually add a task to the code compiler ( see CodeCompilationHelpers ).
     *
     * \see ChangesNotifier
     * \see CodeCompilationHelpers
     */
    void SetCompilationNeeded() { compilationNeeded = true; };

    /**
     * Must be called when compilation of events is over and so events are not considered "modified" anymore.
     */
    void SetCompilationNotNeeded() { compilationNeeded = false; };

    /**
     * Return true if a compilation is needed.
     * This method is usually called by SceneEditorCanvas when (re)loading a scene
     * so as to know if it should launch compilation.
     */
    bool CompilationNeeded() { return compilationNeeded; };

    const gd::String & GetCompiledEventsFile() const { return compiledEventsFile; }
    void SetCompiledEventsFile(const gd::String & file) { compiledEventsFile = file; }

    ///@}

    /** \name Changes notification
     * Members functions used to notify the editor ( mainly SceneEditorCanvas ) that changes have been made
     * and that refreshing should be made.
     */
    ///@{

    /**
     * Return true if important changes have been made and so the editors must reload the scene.
     * ( Important changes may refers to objects modified, properties updated, objects groups modified, variables modified )
     */
    bool RefreshNeeded() const { return refreshNeeded; }

    /**
     * Must be called when some important changes have been made and so the editors must reload the scene
     * \see Scene::RefreshNeeded
     */
    void SetRefreshNeeded() { refreshNeeded = true; }

    /**
     * Must be called when the editor ( i.e: SceneEditorCanvas ) managing the scene has reloaded it.
     */
    void SetRefreshNotNeeded() { refreshNeeded = false; }
    ///@}

    /**
     * Get the profiler associated with the scene. Can be NULL.
     */
    BaseProfiler * GetProfiler() const { return profiler; };

    /**
     * Set the profiler associated with the scene. Can be NULL.
     */
    void SetProfiler(BaseProfiler * profiler_) { profiler = profiler_; };
    #endif

private:
    gd::String                                 name; ///< Scene name
    gd::String                                 mangledName; ///< The scene name mangled by SceneNameMangler
    unsigned int                                backgroundColorR; ///< Background color Red component
    unsigned int                                backgroundColorG; ///< Background color Green component
    unsigned int                                backgroundColorB; ///< Background color Blue component
    gd::String                                 title; ///< Title displayed in the window
    gd::VariablesContainer                      variables; ///< Variables list
    gd::InitialInstancesContainer               initialInstances; ///< Initial instances
    std::vector < gd::Layer >                   initialLayers; ///< Initial layers
    std::vector<ObjectGroup>                    objectGroups; ///< Objects groups
    bool                                        stopSoundsOnStartup; ///< True to make the scene stop all sounds at startup.
    bool                                        standardSortMethod; ///< True to sort objects using standard sort.
    float                                       oglFOV; ///< OpenGL Field Of View value
    float                                       oglZNear; ///< OpenGL Near Z position
    float                                       oglZFar; ///< OpenGL Far Z position
    bool                                        disableInputWhenNotFocused; /// If set to true, the input must be disabled when the window do not have the focus.
    static gd::Layer                            badLayer; ///< Null object, returned when GetLayer can not find an appropriate layer.
    #if defined(GD_IDE_ONLY)
    EventsList                                  events; ///< Scene events
    #endif
    #if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
    gd::LayoutEditorCanvasOptions               associatedSettings;
    #endif

    //TODO: GD C++ Platform specific code below
    #if defined(GD_IDE_ONLY)
    BaseProfiler * profiler; ///< Pointer to the profiler. Can be NULL.
    bool refreshNeeded; ///< If set to true, the IDE will reload the scene( thanks to SceneEditorCanvas notably which check this flag when the scene is being edited )
    bool compilationNeeded; ///< If set to true, the IDE will recompile the events ( thanks to SceneEditorCanvas notably which check this flag when the scene is being edited )
    gd::String compiledEventsFile;
    #endif

    /**
     * Initialize from another layout. Used by copy-ctor and assign-op.
     * Don't forget to update me if members were changed !
     */
    void Init(const gd::Layout & other);
};

/**
 * \brief Functor testing layout name.
 * \see gd::Layout
 */
struct LayoutHasName : public std::binary_function<std::shared_ptr<Layout>, gd::String, bool> {
    bool operator()(const std::shared_ptr<Layout> & layout, gd::String name) const { return layout->GetName() == name; }
};

/**
 * \brief Get the names of all layers from the given layout
 * that are invisible.
 * \see gd::Layout
 */
std::vector<gd::String> GetHiddenLayers(const Layout & layout);

/**
 * \brief Get a type from an object/group name.
 * \note If a group contains only objects of a same type, then the group has this type. Otherwise, it is considered as an object without any specific type.
 *
 * @return Type of the object/group.
 */
gd::String GD_CORE_API GetTypeOfObject(const Project & game, const Layout & layout, gd::String objectName, bool searchInGroups = true);

/**
 * \brief Get a type from a behavior name
 * @return Type of the behavior.
 */
gd::String GD_CORE_API GetTypeOfBehavior(const Project & game, const Layout & layout, gd::String behaviorName, bool searchInGroups = true);

/**
 * \brief Get behaviors of an object/group
 * \note The behaviors of a group are the behaviors which are found in common when looking all the objects of the group.
 *
 * @return Vector containing names of behaviors
 */
std::vector < gd::String > GD_CORE_API GetBehaviorsOfObject(const Project & game, const Layout & layout, gd::String objectName, bool searchInGroups = true);

}

typedef gd::Layout Scene;

#endif // GDCORE_LAYOUT_H
