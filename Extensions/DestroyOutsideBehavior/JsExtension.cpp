/**

GDevelop - DestroyOutside Extension
Copyright (c) 2014-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/
#if defined(GD_IDE_ONLY)
#include "GDCore/PlatformDefinition/PlatformExtension.h"
#include "GDCore/Tools/Localization.h"
#include "GDCore/Tools/Version.h"

#include <iostream>

void DeclareDestroyOutsideBehaviorExtension(gd::PlatformExtension & extension);

/**
 * \brief This class declares information about the JS extension.
 */
class DestroyOutsideBehaviorJsExtension : public gd::PlatformExtension
{
public:

    /**
     * \brief Constructor of an extension declares everything the extension contains: objects, actions, conditions and expressions.
     */
    DestroyOutsideBehaviorJsExtension()
    {
        SetExtensionInformation("DestroyOutsideBehavior",
                              _("Destroy Outside Screen Behavior"),
                              _("Behavior destroying object when they go outside the screen"),
                              "Florian Rival",
                              "Open source (MIT License)");
        DeclareDestroyOutsideBehaviorExtension(*this);

        GetBehaviorMetadata("DestroyOutsideBehavior::DestroyOutside").SetIncludeFile("DestroyOutsideBehavior/destroyoutsideruntimebehavior.js");

        GetAllConditionsForBehavior("DestroyOutsideBehavior::DestroyOutside")["DestroyOutsideBehavior::ExtraBorder"].SetFunctionName("getExtraBorder").SetIncludeFile("DestroyOutsideBehavior/destroyoutsideruntimebehavior.js");
        GetAllActionsForBehavior("DestroyOutsideBehavior::DestroyOutside")["DestroyOutsideBehavior::ExtraBorder"].SetFunctionName("setExtraBorder").SetGetter("getExtraBorder").SetIncludeFile("DestroyOutsideBehavior/destroyoutsideruntimebehavior.js");

        GD_COMPLETE_EXTENSION_COMPILATION_INFORMATION();
    };
};

//We need a specific function to create the extension with emscripten.
#if defined(EMSCRIPTEN)
extern "C" gd::PlatformExtension * CreateGDJSDestroyOutsideBehaviorExtension() {
    return new DestroyOutsideBehaviorJsExtension;
}
#else
/**
 * Used by GDevelop to create the extension class
 * -- Do not need to be modified. --
 */
extern "C" gd::PlatformExtension * GD_EXTENSION_API CreateGDJSExtension() {
    return new DestroyOutsideBehaviorJsExtension;
}
#endif
#endif
