/**

GDevelop - Box 3D Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#include "GDCpp/ExtensionBase.h"
#include "GDCore/Tools/Version.h"
#include "Box3DObject.h"

#include <iostream>

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
        SetExtensionInformation("Box3DObject",
                              _("3D Box Object"),
                              _("Extension allowing to use 3D Box objects."),
                              "Florian Rival",
                              "Open source (MIT License)");

        {
            gd::ObjectMetadata & obj = AddObject("Box3D",
                       _("3D Box"),
                       _("Displays a 3D Box"),
                       "CppPlatform/Extensions/Box3Dicon.png",
                       &CreateBox3DObject);

            AddRuntimeObject(obj, "RuntimeBox3DObject", &CreateRuntimeBox3DObject);

            #if defined(GD_IDE_ONLY)

            obj.SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddAction("Width",
                           _("Width"),
                           _("Modify the width of a 3D Box."),
                           _("Do _PARAM1__PARAM2_ to the width of _PARAM0_"),
                           _("Size"),
                           "res/actions/scaleWidth24.png",
                           "res/actions/scaleWidth.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetWidth").SetManipulatedType("number").SetGetter("GetWidth").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddCondition("Width",
                           _("Width"),
                           _("Compare the width of a 3D Box."),
                           _("width of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Size"),
                           "res/conditions/scaleWidth24.png",
                           "res/conditions/scaleWidth.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("GetWidth").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("Height",
                           _("Height"),
                           _("Modify the height of a 3D Box."),
                           _("Do _PARAM1__PARAM2_ to the height of _PARAM0_"),
                           _("Size"),
                           "res/actions/scaleHeight24.png",
                           "res/actions/scaleHeight.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetHeight").SetManipulatedType("number").SetGetter("GetHeight").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("Height",
                           _("Height"),
                           _("Compare the height of a 3D Box."),
                           _("height of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Size"),
                           "res/conditions/scaleHeight24.png",
                           "res/conditions/scaleHeight.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("SetHeight").SetManipulatedType("number").SetGetter("GetHeight").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("Depth",
                           _("Depth"),
                           _("Modify the depth of a 3D Box."),
                           _("Do _PARAM1__PARAM2_ to the depth of _PARAM0_"),
                           _("Size"),
                           "res/actions/depth24.png",
                           "res/actions/depth.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetDepth").SetManipulatedType("number").SetGetter("GetDepth").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("Depth",
                           _("Depth"),
                           _("Compare the depth of a 3D Box."),
                           _("depth of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Size"),
                           "res/conditions/depth24.png",
                           "res/conditions/depth.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("GetDepth").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("ZPosition",
                           _("Z Position"),
                           _("Modify the Z Position of a 3D Box."),
                           _("Do _PARAM1__PARAM2_ to the Z position of _PARAM0_"),
                           _(""),
                           "res/actions/position24.png",
                           "res/actions/position.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetZPosition").SetManipulatedType("number").SetGetter("GetZPosition").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("ZPosition",
                           _("Z Position"),
                           _("Compare Z position of a 3D Box."),
                           _("Z position of _PARAM0_ is _PARAM1__PARAM2_"),
                           _(""),
                           "res/conditions/position24.png",
                           "res/conditions/position.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("GetZPosition").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("Yaw",
                           _("Yaw"),
                           _("Modify Yaw of a 3D Box object."),
                           _("Do _PARAM1__PARAM2_ to yaw of _PARAM0_"),
                           _("Angle"),
                           "res/actions/rotate24.png",
                           "res/actions/rotate.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetAngle").SetManipulatedType("number").SetGetter("GetAngle").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("Yaw",
                           _("Yaw"),
                           _("Compare Yaw of a 3D Box object."),
                           _("Yaw of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Angle"),
                           "res/conditions/rotate24.png",
                           "res/conditions/rotate.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("GetAngle").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("Pitch",
                           _("Pitch"),
                           _("Modify Pitch of a 3D Box object."),
                           _("Do _PARAM1__PARAM2_ to pitch of _PARAM0_"),
                           _("Angle"),
                           "res/actions/rotate24.png",
                           "res/actions/rotate.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetPitch").SetManipulatedType("number").SetGetter("GetPitch").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("Pitch",
                           _("Pitch"),
                           _("Compare Pitch of a 3D Box object."),
                           _("Pitch of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Angle"),
                           "res/conditions/rotate24.png",
                           "res/conditions/rotate.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))

                .SetFunctionName("GetPitch").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddAction("Roll",
                           _("Roll"),
                           _("Modify Roll of a 3D Box object."),
                           _("Do _PARAM1__PARAM2_ to roll of _PARAM0_"),
                           _("Angle"),
                           "res/actions/rotate24.png",
                           "res/actions/rotate.png")
                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("operator", _("Modification's sign"))
                .AddParameter("expression", _("Value"))

                .SetFunctionName("SetRoll").SetManipulatedType("number").SetGetter("GetRoll").SetIncludeFile("Box3DObject/Box3DObject.h");


            obj.AddCondition("Roll",
                           _("Roll"),
                           _("Compare Roll of a 3D Box object."),
                           _("Roll of _PARAM0_ is _PARAM1__PARAM2_"),
                           _("Angle"),
                           "res/conditions/rotate24.png",
                           "res/conditions/rotate.png")

                .AddParameter("object", _("Object"), "Box3D")
                .AddParameter("relationalOperator", _("Sign of the test"))
                .AddParameter("expression", _("Value to test"))
                .SetFunctionName("GetRoll").SetManipulatedType("number").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddExpression("Depth", _("Depth of the 3D Box"), _("Depth of the 3D Box"), _("Size"), "res/actions/scaleHeight.png")
                .AddParameter("object", _("Object"), "Box3D")
                .codeExtraInformation.SetFunctionName("GetDepth").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddExpression("Z", _("Z Position"), _("Z Position"), _("Position"), "res/actions/scaleHeight.png")
                .AddParameter("object", _("Object"), "Box3D")
                .SetFunctionName("GetZPosition").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddExpression("Yaw", _("Yaw"), _("Yaw"), _("Angle"), "res/actions/scaleHeight.png")
                .AddParameter("object", _("Object"), "Box3D")
                .SetFunctionName("GetAngle").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddExpression("Pitch", _("Pitch"), _("Pitch"), _("Angle"), "res/actions/scaleHeight.png")
                .AddParameter("object", _("Object"), "Box3D")
                .SetFunctionName("GetPitch").SetIncludeFile("Box3DObject/Box3DObject.h");

            obj.AddExpression("Roll", _("Roll"), _("Roll"), _("Angle"), "res/actions/scaleHeight.png")
                .AddParameter("object", _("Object"), "Box3D")
                .SetFunctionName("GetRoll").SetIncludeFile("Box3DObject/Box3DObject.h");

            #endif

        }

        GD_COMPLETE_EXTENSION_COMPILATION_INFORMATION();
    };
};

/**
 * Used by GDevelop to create the extension class
 * -- Do not need to be modified. --
 */
extern "C" ExtensionBase * GD_EXTENSION_API CreateGDExtension() {
    return new Extension;
}
