/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#include "GDCpp/Extensions/Builtin/MouseExtension.h"
#include "GDCpp/Extensions/ExtensionBase.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#if !defined(GD_IDE_ONLY)
#include "GDCore/Extensions/Builtin/MouseExtension.cpp"
#endif

MouseExtension::MouseExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsMouseExtension(*this);

    #if defined(GD_IDE_ONLY)
    GetAllActions()["CentreSourisX"].SetFunctionName("CenterCursorHorizontally").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllActions()["CentreSourisY"].SetFunctionName("CenterCursorVertically").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllActions()["CacheSouris"].SetFunctionName("HideCursor").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllActions()["MontreSouris"].SetFunctionName("ShowCursor").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllActions()["SetSourisXY"].SetFunctionName("SetCursorPosition").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllActions()["CentreSouris"].SetFunctionName("CenterCursor").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");

    GetAllConditions()["SourisX"].SetFunctionName("GetCursorXPosition").SetManipulatedType("number").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllConditions()["SourisY"].SetFunctionName("GetCursorYPosition").SetManipulatedType("number").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllConditions()["SourisBouton"].SetFunctionName("MouseButtonPressed").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllConditions()["MouseButtonReleased"].SetFunctionName("MouseButtonReleased").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");

    GetAllConditions()["SourisSurObjet"].SetFunctionName("CursorOnObject").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");

    GetAllExpressions()["MouseX"].SetFunctionName("GetCursorXPosition").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllExpressions()["SourisX"].SetFunctionName("GetCursorXPosition").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllExpressions()["MouseY"].SetFunctionName("GetCursorYPosition").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllExpressions()["SourisY"].SetFunctionName("GetCursorYPosition").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");
    GetAllExpressions()["MouseWheelDelta"].SetFunctionName("GetMouseWheelDelta").SetIncludeFile("GDCpp/Extensions/Builtin/MouseTools.h");

    StripUnimplementedInstructionsAndExpressions(); //Touch support is not implemented
    #endif
}

