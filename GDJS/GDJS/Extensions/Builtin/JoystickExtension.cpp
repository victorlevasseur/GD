/*
 * GDevelop JS Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "JoystickExtension.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerationContext.h"
#include "GDCore/Events/CodeGeneration/ExpressionsCodeGeneration.h"
#include "GDCore/Events/Tools/EventsCodeNameMangler.h"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/Localization.h"

namespace gdjs
{

JoystickExtension::JoystickExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsJoystickExtension(*this);

    SetExtensionInformation("BuiltinJoystick",
                          _("Joysticks features"),
                          _("Built-in extension allowing to use joysticks"),
                          "Florian Rival",
                          "Open source (MIT License)");

    //Nothing is available for now.
    StripUnimplementedInstructionsAndExpressions(); //Unimplemented things are listed here:
    /*
    AddCondition("JoystickButtonDown",
                   _("A button of a joystick is pressed"),
                   _("Test if a button of a joystick is pressed."),
                   _("The button _PARAM2_ of joystick _PARAM1_ is pressed"),
                   _("Joystick"),
                   "res/conditions/joystick24.png",
                   "res/conditions/joystick.png")
        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("expression", _("Joystick number ( First joystick: 0 )"))
        .AddParameter("expression", _("Button"))
        .SetFunctionName("JoystickButtonDown").SetIncludeFile("GDCpp/Extensions/Builtin/JoystickTools.h");



    AddCondition("JoystickAxis",
                   _("Value of an axis of a joystick"),
                   _("Test the value of an axis of a joystick."),
                   _("The value of the axis _PARAM2_ of joystick _PARAM1_ is _PARAM3__PARAM4_"),
                   _("Joystick"),
                   "res/conditions/joystick24.png",
                   "res/conditions/joystick.png")
        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("expression", _("Joystick number ( First joystick: 0 )"))
        .AddParameter("joyaxis", _("Axis"))
        .AddParameter("relationalOperator", _("Sign of the test"))
        .AddParameter("expression", _("Value to test"))
        .SetFunctionName("GetJoystickAxisValue").SetManipulatedType("number").SetIncludeFile("GDCpp/Extensions/Builtin/JoystickTools.h");



    AddAction("GetJoystickAxis",
                   _("Get the value of the axis of a joystick"),
                   _("Save in the variable the value of the axis of the joystick ( from -100 to 100 )."),
                   _("Save in _PARAM3_ the value of axis _PARAM2_ of joystick _PARAM1_"),
                   _("Joystick"),
                   "res/actions/joystick24.png",
                   "res/actions/joystick.png")
        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("expression", _("Joystick number ( First joystick: 0 )"))
        .AddParameter("joyaxis", _("Axis"))
        .AddParameter("scenevar", _("Save result to scene variable"))
        .SetFunctionName("JoystickAxisValueToVariable").SetManipulatedType("number").SetIncludeFile("GDCpp/Extensions/Builtin/JoystickTools.h");

    AddExpression("GetJoystickAxis",
                   _("Joystick axis"),
                   _("Value of an axis of a joystick"),
                   _("Joystick"),
                   "res/conditions/joystick.png")

        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("expression", _("Joystick number ( First joystick: 0 )"))
        .AddParameter("joyaxis", _("Axis"))
        .SetFunctionName("GetJoystickAxisValue").SetIncludeFile("GDCpp/Extensions/Builtin/JoystickTools.h");

    */
}

}
