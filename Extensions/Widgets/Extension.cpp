/**

GDevelop - Text Object Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/
/**
 * Contributors to the extension:
 * Victor Levasseur ( Bold/Italic/Underlined styles )
 */

#include "GDCpp/Extensions/ExtensionBase.h"
#include "GDCore/Extensions/PlatformExtension.h"
#include "GDCore/Tools/Version.h"
#include "TextBoxObject.h"


void DeclareTextBoxObjectExtension(gd::PlatformExtension & extension)
{
    extension.SetExtensionInformation("Widgets",
                          _("Widgets"),
                          _("Extension allowing to use a textbox."),
                          "Victor Levasseur",
                          "Open source (MIT License)");

    gd::ObjectMetadata & obj = extension.AddObject<TextBoxObject>(
               "TextBox",
               _("Textbox"),
               _("Displays a textbox widget"),
               "CppPlatform/Extensions/texticon.png");

    #if defined(GD_IDE_ONLY)
    obj.SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("String",
                   _("Modify the text"),
                   _("Modify the text of a Text object."),
                   _("Do _PARAM1__PARAM2_ to the text of _PARAM0_"),
                   _(""),
                   "res/actions/text24.png",
                   "res/actions/text.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("operator", _("Modification's sign"))
        .AddParameter("string", _("Text"))
        .SetFunctionName("SetString").SetManipulatedType("string").SetGetter("GetString").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("String",
                   _("Compare the text"),
                   _("Compare the text of a Text object."),
                   _("Text of _PARAM0_ is _PARAM1__PARAM2_"),
                   _(""),
                   "res/conditions/text24.png",
                   "res/conditions/text.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("relationalOperator", _("Sign of the test"))
        .AddParameter("string", _("Text to test"))
        .SetFunctionName("GetString").SetManipulatedType("string").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("Font",
                   _("Font"),
                   _("Modify the font of the text."),
                   _("Change font of _PARAM0_ to _PARAM1_"),
                   _("Font"),
                   "res/actions/font24.png",
                   "res/actions/font.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("police", _("Font"))
        .SetFunctionName("ChangeFont").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("Size",
                   _("Size"),
                   _("Modify the size of the text."),
                   _("Do _PARAM1__PARAM2_ to the size of the text of _PARAM0_"),
                   _(""),
                   "res/actions/characterSize24.png",
                   "res/actions/characterSize.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("operator", _("Modification's sign"))
        .AddParameter("expression", _("Value"))
        .SetFunctionName("SetCharacterSize").SetManipulatedType("number").SetGetter("GetCharacterSize").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("Size",
                   _("Size"),
                   _("Test the size of the text"),
                   _("The size of the text of _PARAM0_ is _PARAM1__PARAM2_"),
                   _(""),
                   "res/conditions/characterSize24.png",
                   "res/conditions/characterSize.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("relationalOperator", _("Sign of the test"))
        .AddParameter("expression", _("Size to test"))
        .SetFunctionName("GetCharacterSize").SetManipulatedType("number").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("ChangeColor",
                   _("Color"),
                   _("Change the color of the text. The color is white by default."),
                   _("Change color of _PARAM0_ to _PARAM1_"),
                   _("Effects"),
                   "res/actions/color24.png",
                   "res/actions/color.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("color", _("Color"))
        .SetFunctionName("SetColor").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("Opacity",
                   _("Opacity"),
                   _("Modify the opacity of a Text object."),
                   _("Do _PARAM1__PARAM2_ to the opacity of _PARAM0_"),
                   _(""),
                   "res/actions/opacity24.png",
                   "res/actions/opacity.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("operator", _("Modification's sign"))
        .AddParameter("expression", _("Value"))
        .SetFunctionName("SetOpacity").SetManipulatedType("number").SetGetter("GetOpacity").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("Opacity",
                   _("Opacity"),
                   _("Test the value of the opacity of a text object."),
                   _("The opacity of _PARAM0_ is _PARAM1__PARAM2_"),
                   _(""),
                   "res/conditions/opacity24.png",
                   "res/conditions/opacity.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("relationalOperator", _("Sign of the test"))
        .AddParameter("expression", _("Value to test"))
        .SetFunctionName("GetOpacity").SetManipulatedType("number").SetIncludeFile("TextBoxObject/TextBoxObject.h");



    obj.AddAction("SetSmooth",
                   _("Smoothing"),
                   _("Activate or desactivate text smoothing."),
                   _("Smooth _PARAM0_: _PARAM1_"),
                   _("Style"),
                   "res/actions/opacity24.png",
                   "res/actions/opacity.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("yesorno", _("Smooth the text"))
        .SetFunctionName("SetSmooth").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("Smoothed",
                   _("Smoothing"),
                   _("Test if an object is smoothed"),
                   _("_PARAM0_ is smoothed"),
                   _("Style"),
                   "res/conditions/opacity24.png",
                   "res/conditions/opacity.png")

        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("IsSmoothed").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("SetBold",
                   _("Bold"),
                   _("De/activate bold"),
                   _("Set bold style of _PARAM0_ : _PARAM1_"),
                   _("Style"),
                   "res/actions/bold.png",
                   "res/actions/bold16.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("yesorno", _("Set bold style"))
        .SetFunctionName("SetBold").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("IsBold",
                   _("Bold"),
                   _("Test if bold style is activated"),
                   _("_PARAM0_ bold style is set"),
                   _("Style"),
                   "res/conditions/bold.png",
                   "res/conditions/bold16.png")

        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("IsBold").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("SetItalic",
                   _("Italic"),
                   _("De/activate italic."),
                   _("Set italic style for _PARAM0_ : _PARAM1_"),
                   _("Style"),
                   "res/actions/italic.png",
                   "res/actions/italic16.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("yesorno", _("Set italic"))
        .SetFunctionName("SetItalic").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("IsItalic",
                   _("Italic"),
                   _("Test if the italic style is activated"),
                   _("_PARAM0_ italic style is set"),
                   _("Style"),
                   "res/conditions/italic.png",
                   "res/conditions/italic16.png")

        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("IsItalic").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("SetUnderlined",
                   _("Underlined"),
                   _("De/activate underlined style."),
                   _("Set underlined style of _PARAM0_: _PARAM1_"),
                   _("Style"),
                   "res/actions/underline.png",
                   "res/actions/underline16.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("yesorno", _("Underline"))
        .SetFunctionName("SetUnderlined").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("IsUnderlined",
                   _("Underlined"),
                   _("Test if the underlined style of an object is set."),
                   _("_PARAM0_ underlined style is activated"),
                   _("Style"),
                   "res/conditions/underline.png",
                   "res/conditions/underline16.png")

        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("IsUnderlined").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddAction("Angle",
                   _("Angle"),
                   _("Modify the angle of a Text object."),
                   _("Do _PARAM1__PARAM2_ to the angle of _PARAM0_"),
                   _("Rotation"),
                   "res/actions/rotate24.png",
                   "res/actions/rotate.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("operator", _("Modification's sign"))
        .AddParameter("expression", _("Value"))
        .SetFunctionName("SetAngle").SetManipulatedType("number").SetGetter("GetAngle").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddCondition("Angle",
                   _("Angle"),
                   _("Test the value of the angle of a text object."),
                   _("The angle of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Rotation"),
                   "res/conditions/rotate24.png",
                   "res/conditions/rotate.png")

        .AddParameter("object", _("Object"), "Text")
        .AddParameter("relationalOperator", _("Sign of the test"))
        .AddParameter("expression", _("Value to test"))
        .SetFunctionName("GetAngle").SetManipulatedType("number").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddExpression("Opacity", _("Opacity"), _("Opacity"), _("Opacity"), "res/actions/opacity.png")
        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("GetOpacity").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddExpression("Angle", _("Angle"), _("Angle"), _("Rotation"), "res/actions/rotate.png")
        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("GetAngle").SetIncludeFile("TextBoxObject/TextBoxObject.h");

    obj.AddStrExpression("String", _("Text"), _("Text"), _("Text"), "res/texteicon.png")
        .AddParameter("object", _("Object"), "Text")
        .SetFunctionName("GetString").SetIncludeFile("TextBoxObject/TextBoxObject.h");
    #endif
}

/**
 * \brief This class declares information about the C++ extension.
 */
class Extension : public ExtensionBase
{
public:

    /**
     * Constructor of an extension declares everything the extension contains: objects, actions, conditions and expressions.
     */
    Extension()
    {
        DeclareTextBoxObjectExtension(*this);
        AddRuntimeObject<TextBoxObject, RuntimeTextBoxObject>(
            GetObjectMetadata("Widgets::TextBox"),
            "RuntimeTextBoxObject");

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
