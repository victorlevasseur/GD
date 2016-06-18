/*
 * GDevelop JS Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "KeyboardExtension.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerationContext.h"
#include "GDCore/Events/CodeGeneration/ExpressionsCodeGeneration.h"
#include "GDCore/Events/Tools/EventsCodeNameMangler.h"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/Localization.h"

namespace gdjs
{

KeyboardExtension::KeyboardExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsKeyboardExtension(*this);

    GetAllConditions()["KeyPressed"].SetFunctionName("gdjs.evtTools.input.isKeyPressed");
    GetAllConditions()["KeyReleased"].SetFunctionName("gdjs.evtTools.input.wasKeyReleased");
    GetAllConditions()["KeyFromTextPressed"].SetFunctionName("gdjs.evtTools.input.isKeyPressed");
    GetAllConditions()["KeyFromTextReleased"].SetFunctionName("gdjs.evtTools.input.wasKeyReleased");
    GetAllConditions()["AnyKeyPressed"].SetFunctionName("gdjs.evtTools.input.anyKeyPressed");
    GetAllStrExpressions()["LastPressedKey"].SetFunctionName("gdjs.evtTools.input.lastPressedKey");
}

}
