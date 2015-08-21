/**

GDevelop - Platform Behavior Extension
Copyright (c) 2013-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#ifndef PLATFORMBEHAVIOR_H
#define PLATFORMBEHAVIOR_H

#include "GDCpp/Behavior.h"
#include "GDCpp/Object.h"
#include <SFML/System/Vector2.hpp>
class ScenePlatformObjectsManager;
class RuntimeScene;
namespace gd { class SerializerElement; }
#if defined(GD_IDE_ONLY)
#include <map>
namespace gd { class PropertyDescriptor; }
namespace gd { class Project; }
namespace gd { class Layout; }
#endif

/**
 * \brief Behavior that mark object as being a platform for objects using
 * PlatformerObject behavior.
 */
class GD_EXTENSION_API PlatformBehavior : public Behavior
{
public:
    PlatformBehavior();
    virtual ~PlatformBehavior();
    virtual Behavior* Clone() const { return new PlatformBehavior(*this); }

    /**
     * \brief Return the object owning this behavior.
     */
    RuntimeObject * GetObject() const { return object; }

    /**
     * \brief The different types of platforms.
     */
    enum PlatformType
    {
        NormalPlatform,
        Jumpthru,
        Ladder
    };

    /**
     * \brief Return the object owning this behavior.
     */
    PlatformType GetPlatformType() const { return platformType; }

    /**
     * \brief Change the platform type
     * \param platformType The new platform type ("Platform", "Jumpthru" or "Ladder").
     */
    void ChangePlatformType(const gd::String & platformType_);

    virtual void UnserializeFrom(const gd::SerializerElement & element);
    #if defined(GD_IDE_ONLY)
    virtual std::map<gd::String, gd::PropertyDescriptor> GetProperties(gd::Project & project) const;
    virtual bool UpdateProperty(const gd::String & name, const gd::String & value, gd::Project & project);
    virtual void SerializeTo(gd::SerializerElement & element) const;
    #endif

private:
    virtual void OnActivate();
    virtual void OnDeActivate();

    virtual void DoStepPreEvents(RuntimeScene & scene);
    virtual void DoStepPostEvents(RuntimeScene & scene);

    RuntimeScene * parentScene; ///< The scene the object belongs to.
    ScenePlatformObjectsManager * sceneManager; ///< The platform objects manager associated to the scene.
    bool registeredInManager; ///< True if the behavior is registered in the list of platforms of the scene.
    PlatformType platformType;
};

#endif // PLATFORMBEHAVIOR_H

