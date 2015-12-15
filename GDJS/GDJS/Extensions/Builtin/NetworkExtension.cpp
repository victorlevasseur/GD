/*
 * GDevelop JS Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "NetworkExtension.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#include "GDCore/IDE/Project/ArbitraryResourceWorker.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/Localization.h"

namespace gdjs
{

NetworkExtension::NetworkExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsNetworkExtension(*this);

    SetExtensionInformation("BuiltinNetwork",
                          _("Basic internet features"),
                          _("Built-in extension providing network features."),
                          "Florian Rival",
                          "Open source (MIT License)");

    GetAllActions()["SendRequest"].SetFunctionName("gdjs.evtTools.network.sendHttpRequest").SetIncludeFile("networktools.js");
    GetAllActions()["JSONToVariableStructure"].SetFunctionName("gdjs.evtTools.network.jsonToVariableStructure").SetIncludeFile("networktools.js");

    GetAllStrExpressions()["ToJSON"].SetFunctionName("gdjs.evtTools.network.variableStructureToJSON").SetIncludeFile("networktools.js");

    StripUnimplementedInstructionsAndExpressions();
}

}
