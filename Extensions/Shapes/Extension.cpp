/**

GDevelop - AdMob Object Extension
Copyright (c) 2015 - Victor Levasseur (victorlevasseur52@gmail.com)
This project is released under the MIT License.
*/

#include "GDCore/Tools/Localization.h"
#include "GDCore/Tools/Version.h"
#include "GDCpp/Extensions/ExtensionBase.h"

#include "EllipseObject.h"
#include "RuntimeEllipseObject.h"
#include "EllipseInstructions.h"

void DeclareShapesExtension(gd::PlatformExtension & extension)
{
    extension.SetExtensionInformation("Shapes",
                          _("Shapes objects"),
                          _("Provides objects to display simple geometric shapes in games."),
                          "Victor Levasseur",
                          "Open source (MIT License)");

    gd::ObjectMetadata & obj = extension.AddObject("Ellipse",
               _("Ellipse"),
               _("Display a ellipse shape."),
               "CppPlatform/Extensions/EllipseObjecticon32.png",
               &CreateEllipseObject);

    obj.SetHelpUrl("/gdevelop/documentation/manual/built_shapes");
    #if defined(GD_IDE_ONLY)
    obj.SetIncludeFile("Shapes/RuntimeEllipseObject.h");
    DeclareEllipseObjectInstruction(obj);
    #endif
}

/**
 * \brief This class declares information about the extension.
 */
class Extension : public ExtensionBase
{
public:

    /**
     * Constructor of an extension declares everything the extension contains: objects, actions, conditions and expressions.
     */
    Extension()
    {
        DeclareShapesExtension(*this);
        AddRuntimeObject(GetObjectMetadata("Shapes::Ellipse"),
            "RuntimeEllipseObject", CreateRuntimeEllipseObject);

        GD_COMPLETE_EXTENSION_COMPILATION_INFORMATION();
    };
};

#if !defined(EMSCRIPTEN)
/**
 * Used by GDevelop to create the extension class
 * -- Do not need to be modified. --
 */
extern "C" ExtensionBase * GD_EXTENSION_API CreateGDExtension() {
    return new Extension;
}
#endif
