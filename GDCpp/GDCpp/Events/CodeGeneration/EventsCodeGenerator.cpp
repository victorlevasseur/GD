/*
 * GDevelop C++ Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#if defined(GD_IDE_ONLY)
#include "GDCpp/IDE/DependenciesAnalyzer.h"
#include "GDCpp/Extensions/CppPlatform.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerationContext.h"
#include "GDCore/Extensions/Metadata/EventMetadata.h"
#include "GDCore/Extensions/Metadata/InstructionMetadata.h"
#include "GDCore/Extensions/Metadata/ExpressionMetadata.h"
#include "GDCore/Project/Project.h"
#include "GDCore/Project/ExternalEvents.h"
#include "GDCore/Project/Layout.h"
#include "GDCore/Events/Tools/EventsCodeNameMangler.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/CommonTools.h"
#include "GDCpp/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCpp/IDE/BaseProfiler.h"
#include "GDCpp/Runtime/SceneNameMangler.h"
#include "GDCpp/Events/Builtin/ProfileEvent.h"
#include "GDCpp/Events/CodeGeneration/VariableParserCallbacks.h"

using namespace std;

gd::String EventsCodeGenerator::GenerateObjectFunctionCall(gd::String objectListName,
                                                      const gd::ObjectMetadata & objMetadata,
                                                      const gd::ExpressionCodeGenerationInformation & codeInfo,
                                                      gd::String parametersStr,
                                                      gd::String defaultOutput,
                                                      gd::EventsCodeGenerationContext & context)
{
    bool castNeeded = !objMetadata.className.empty();

    if ( codeInfo.staticFunction )
    {
        if ( !castNeeded )
            return "(RuntimeObject::"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "("+objMetadata.className+"::"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
    else if ( context.GetCurrentObject() == objectListName && !context.GetCurrentObject().empty())
    {
        if ( !castNeeded )
            return "("+ManObjListName(objectListName)+"[i]->"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "(static_cast<"+objMetadata.className+"*>("+ManObjListName(objectListName)+"[i])->"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
    else
    {
        if ( !castNeeded )
            return "(( "+ManObjListName(objectListName)+".empty() ) ? "+defaultOutput+" :"+ ManObjListName(objectListName)+"[0]->"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "(( "+ManObjListName(objectListName)+".empty() ) ? "+defaultOutput+" : "+"static_cast<"+objMetadata.className+"*>("+ManObjListName(objectListName)+"[0])->"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
}

gd::String EventsCodeGenerator::GenerateObjectBehaviorFunctionCall(gd::String objectListName,
                                                                             gd::String behaviorName,
                                                      const gd::BehaviorMetadata & autoInfo,
                                                      const gd::ExpressionCodeGenerationInformation & codeInfo,
                                                      gd::String parametersStr,
                                                      gd::String defaultOutput,
                                                      gd::EventsCodeGenerationContext & context)
{
    bool castNeeded = !autoInfo.className.empty();

    if ( codeInfo.staticFunction )
    {
        if ( !castNeeded )
            return "(gd::Behavior::"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "("+autoInfo.className+"::"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
    else if ( context.GetCurrentObject() == objectListName && !context.GetCurrentObject().empty())
    {
        if ( !castNeeded )
            return "("+ManObjListName(objectListName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\")->"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "(static_cast<"+autoInfo.className+"*>("+ManObjListName(objectListName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\"))->"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
    else
    {
        if ( !castNeeded )
            return "(( "+ManObjListName(objectListName)+".empty() ) ? "+defaultOutput+" :"+ManObjListName(objectListName)+"[0]->GetBehaviorRawPointer(\""+behaviorName+"\")->"+codeInfo.functionCallName+"("+parametersStr+"))";
        else
            return "(( "+ManObjListName(objectListName)+".empty() ) ? "+defaultOutput+" : "+"static_cast<"+autoInfo.className+"*>("+ManObjListName(objectListName)+"[0]->GetBehaviorRawPointer(\""+behaviorName+"\"))->"+codeInfo.functionCallName+"("+parametersStr+"))";
    }
}

gd::String EventsCodeGenerator::GenerateObjectCondition(const gd::String & objectName,
                                                                   const gd::ObjectMetadata & objInfo,
                                                                   const std::vector<gd::String> & arguments,
                                                                   const gd::InstructionMetadata & instrInfos,
                                                                   const gd::String & returnBoolean,
                                                                   bool conditionInverted,
                                                                   gd::EventsCodeGenerationContext & context)
{
    gd::String conditionCode;

    //Prepare call
    //Add a static_cast if necessary
    gd::String objectFunctionCallNamePart =
    ( !instrInfos.parameters[0].supplementaryInformation.empty() ) ?
        "static_cast<"+objInfo.className+"*>("+ManObjListName(objectName)+"[i])->"+instrInfos.codeExtraInformation.functionCallName
    :   ManObjListName(objectName)+"[i]->"+instrInfos.codeExtraInformation.functionCallName;

    //Create call
    gd::String predicat;
    if ( (instrInfos.codeExtraInformation.type == "number" || instrInfos.codeExtraInformation.type == "string") )
    {
        predicat = GenerateRelationalOperatorCall(instrInfos, arguments, objectFunctionCallNamePart, 1);
    }
    else
    {
        gd::String argumentsStr;
        for (std::size_t i = 1;i<arguments.size();++i)
        {
            if ( i != 1 ) argumentsStr += ", ";
            argumentsStr += arguments[i];
        }

        predicat = objectFunctionCallNamePart+"("+argumentsStr+")";
    }
    if ( conditionInverted ) predicat = GenerateNegatedPredicat(predicat);

    //Generate whole condition code
    conditionCode += "for(std::size_t i = 0;i < "+ManObjListName(objectName)+".size();)\n";
    conditionCode += "{\n";
    conditionCode += "    if ( "+predicat+" )\n";
    conditionCode += "    {\n";
    conditionCode += "        "+returnBoolean+" = true;\n";
    conditionCode += "        ++i;\n";
    conditionCode += "    }\n";
    conditionCode += "    else\n";
    conditionCode += "    {\n";
    conditionCode += "        "+ManObjListName(objectName)+".erase("+ManObjListName(objectName)+".begin()+i);\n";
    conditionCode += "    }\n";
    conditionCode += "}\n";

    return conditionCode;
}

gd::String EventsCodeGenerator::GenerateBehaviorCondition(const gd::String & objectName,
                                                                       const gd::String & behaviorName,
                                                                   const gd::BehaviorMetadata & autoInfo,
                                                                   const std::vector<gd::String> & arguments,
                                                                   const gd::InstructionMetadata & instrInfos,
                                                                   const gd::String & returnBoolean,
                                                                   bool conditionInverted,
                                                                   gd::EventsCodeGenerationContext & context)
{
    gd::String conditionCode;

    //Prepare call
    //Add a static_cast if necessary
    gd::String objectFunctionCallNamePart =
    ( !instrInfos.parameters[1].supplementaryInformation.empty() ) ?
        "static_cast<"+autoInfo.className+"*>("+ManObjListName(objectName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\"))->"+instrInfos.codeExtraInformation.functionCallName
    :   ManObjListName(objectName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\")->"+instrInfos.codeExtraInformation.functionCallName;

    //Create call
    gd::String predicat;
    if ( (instrInfos.codeExtraInformation.type == "number" || instrInfos.codeExtraInformation.type == "string") )
    {
        predicat = GenerateRelationalOperatorCall(instrInfos, arguments, objectFunctionCallNamePart, 2);
    }
    else
    {
        gd::String argumentsStr;
        for (std::size_t i = 2;i<arguments.size();++i)
        {
            if ( i != 2 ) argumentsStr += ", ";
            argumentsStr += arguments[i];
        }

        predicat = objectFunctionCallNamePart+"("+argumentsStr+")";
    }
    if ( conditionInverted ) predicat = GenerateNegatedPredicat(predicat);

    //Verify that object has behavior.
    vector < gd::String > behaviors = gd::GetBehaviorsOfObject(project, scene, objectName);
    if ( find(behaviors.begin(), behaviors.end(), behaviorName) == behaviors.end() )
    {
        cout << "Bad behavior requested" << endl;
    }
    else
    {
        conditionCode += "for(std::size_t i = 0;i < "+ManObjListName(objectName)+".size();)\n";
        conditionCode += "{\n";
        conditionCode += "    if ( "+predicat+" )\n";
        conditionCode += "    {\n";
        conditionCode += "        "+returnBoolean+" = true;\n";
        conditionCode += "        ++i;\n";
        conditionCode += "    }\n";
        conditionCode += "    else\n";
        conditionCode += "    {\n";
        conditionCode += "        "+ManObjListName(objectName)+".erase("+ManObjListName(objectName)+".begin()+i);\n";
        conditionCode += "    }\n";
        conditionCode += "}";
    }


    return conditionCode;
}

gd::String EventsCodeGenerator::GenerateObjectAction(const gd::String & objectName,
                                                       const gd::ObjectMetadata & objInfo,
                                                       const std::vector<gd::String> & arguments,
                                                       const gd::InstructionMetadata & instrInfos,
                                                       gd::EventsCodeGenerationContext & context)
{
    gd::String actionCode;

    //Prepare call
    //Add a static_cast if necessary
    gd::String objectPart = ( !instrInfos.parameters[0].supplementaryInformation.empty() ) ? "static_cast<"+objInfo.className+"*>("+ManObjListName(objectName)+"[i])->" : ManObjListName(objectName)+"[i]->" ;

    //Create call
    gd::String call;
    if ( instrInfos.codeExtraInformation.type == "number" || instrInfos.codeExtraInformation.type == "string")
    {
        if ( instrInfos.codeExtraInformation.accessType == gd::InstructionMetadata::ExtraInformation::MutatorAndOrAccessor )
            call = GenerateOperatorCall(instrInfos, arguments, objectPart+instrInfos.codeExtraInformation.functionCallName, objectPart+instrInfos.codeExtraInformation.optionalAssociatedInstruction,1);
        else
            call = GenerateCompoundOperatorCall(instrInfos, arguments, objectPart+instrInfos.codeExtraInformation.functionCallName,1);
    }
    else
    {
        gd::String argumentsStr;
        for (std::size_t i = 1;i<arguments.size();++i)
        {
            if ( i != 1 ) argumentsStr += ", ";
            argumentsStr += arguments[i];
        }

        call = objectPart+instrInfos.codeExtraInformation.functionCallName+"("+argumentsStr+")";
    }

    actionCode += "for(std::size_t i = 0;i < "+ManObjListName(objectName)+".size();++i)\n";
    actionCode += "{\n";
    actionCode += "    "+call+";\n";
    actionCode += "}\n";


    return actionCode;
}

gd::String EventsCodeGenerator::GenerateBehaviorAction(const gd::String & objectName,
                                                            const gd::String & behaviorName,
                                                            const gd::BehaviorMetadata & autoInfo,
                                                            const std::vector<gd::String> & arguments,
                                                            const gd::InstructionMetadata & instrInfos,
                                                            gd::EventsCodeGenerationContext & context)
{
    gd::String actionCode;

    //Prepare call
    //Add a static_cast if necessary
    gd::String objectPart =
    ( !instrInfos.parameters[1].supplementaryInformation.empty() ) ?
        "static_cast<"+autoInfo.className+"*>("+ManObjListName(objectName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\"))->"
    :   ManObjListName(objectName)+"[i]->GetBehaviorRawPointer(\""+behaviorName+"\")->";

    //Create call
    gd::String call;
    if ( (instrInfos.codeExtraInformation.type == "number" || instrInfos.codeExtraInformation.type == "string") )
    {
        if ( instrInfos.codeExtraInformation.accessType == gd::InstructionMetadata::ExtraInformation::MutatorAndOrAccessor )
            call = GenerateOperatorCall(instrInfos, arguments, objectPart+instrInfos.codeExtraInformation.functionCallName, objectPart+instrInfos.codeExtraInformation.optionalAssociatedInstruction,2);
        else
            call = GenerateCompoundOperatorCall(instrInfos, arguments, objectPart+instrInfos.codeExtraInformation.functionCallName,2);
    }
    else
    {
        gd::String argumentsStr;
        for (std::size_t i = 2;i<arguments.size();++i)
        {
            if ( i != 2 ) argumentsStr += ", ";
            argumentsStr += arguments[i];
        }

        call = objectPart+instrInfos.codeExtraInformation.functionCallName+"("+argumentsStr+")";
    }

    //Verify that object has behavior.
    vector < gd::String > behaviors = gd::GetBehaviorsOfObject(project, scene, objectName);
    if ( find(behaviors.begin(), behaviors.end(), behaviorName) == behaviors.end() )
    {
        cout << "Bad behavior requested for an action" << endl;
    }
    else
    {
        actionCode += "for(std::size_t i = 0;i < "+ManObjListName(objectName)+".size();++i)\n";
        actionCode += "{\n";
        actionCode += "    "+call+";\n";
        actionCode += "}\n";
    }


    return actionCode;
}

gd::String EventsCodeGenerator::GenerateParameterCodes(const gd::String & parameter, const gd::ParameterMetadata & metadata,
                                                        gd::EventsCodeGenerationContext & context,
                                                        const gd::String & previousParameter,
                                                        std::vector < std::pair<gd::String, gd::String> > * supplementaryParametersTypes)
{
    gd::String argOutput;

    //Code only parameter type
    if ( metadata.type == "currentScene" )
    {
        argOutput += "*runtimeContext->scene";
    }
    //Code only parameter type
    else if ( metadata.type == "objectList" )
    {
        std::vector<gd::String> realObjects = ExpandObjectsName(parameter, context);

        argOutput += "runtimeContext->ClearObjectListsMap()";
        for (std::size_t i = 0;i<realObjects.size();++i)
        {
            context.ObjectsListNeeded(realObjects[i]);
            argOutput += ".AddObjectListToMap(\""+ConvertToString(realObjects[i])+"\", "+ManObjListName(realObjects[i])+")";
        }
        argOutput += ".ReturnObjectListsMap()";
    }
    //Code only parameter type
    else if ( metadata.type == "objectListWithoutPicking" )
    {
        std::vector<gd::String> realObjects = ExpandObjectsName(parameter, context);

        argOutput += "runtimeContext->ClearObjectListsMap()";
        for (std::size_t i = 0;i<realObjects.size();++i)
        {
            context.EmptyObjectsListNeeded(realObjects[i]);
            argOutput += ".AddObjectListToMap(\""+ConvertToString(realObjects[i])+"\", "+ManObjListName(realObjects[i])+")";
        }
        argOutput += ".ReturnObjectListsMap()";
    }
    //Code only parameter type
    else if ( metadata.type == "objectPtr")
    {
        std::vector<gd::String> realObjects = ExpandObjectsName(parameter, context);

        if ( find(realObjects.begin(), realObjects.end(), context.GetCurrentObject()) != realObjects.end() && !context.GetCurrentObject().empty())
        {
            //If object currently used by instruction is available, use it directly.
            argOutput += ManObjListName(context.GetCurrentObject())+"[i]";
        }
        else
        {
            for (std::size_t i = 0;i<realObjects.size();++i)
            {
                context.ObjectsListNeeded(realObjects[i]);
                argOutput += "(!"+ManObjListName(realObjects[i])+".empty() ? "+ManObjListName(realObjects[i])+"[0] : ";
            }
            argOutput += "NULL";
            for (std::size_t i = 0;i<realObjects.size();++i)
                argOutput += ")";
        }
    }
    else if (metadata.type == "scenevar")
    {
        VariableCodeGenerationCallbacks callbacks(argOutput, *this, context, VariableCodeGenerationCallbacks::LAYOUT_VARIABLE);

        gd::VariableParser parser(parameter);
        if ( !parser.Parse(callbacks) )
        {
            cout << "Error :" << parser.firstErrorStr << " in: "<< parameter << endl;
            argOutput = "runtimeContext->GetSceneVariables().GetBadVariable()";
        }
    }
    else if (metadata.type == "globalvar")
    {
        VariableCodeGenerationCallbacks callbacks(argOutput, *this, context, VariableCodeGenerationCallbacks::PROJECT_VARIABLE);

        gd::VariableParser parser(parameter);
        if ( !parser.Parse(callbacks) )
        {
            cout << "Error :" << parser.firstErrorStr << " in: "<< parameter << endl;
            argOutput = "runtimeContext->GetGameVariables().GetBadVariable()";
        }
    }
    else if (metadata.type == "objectvar")
    {
        //Object is either the object of the previous parameter or, if it is empty,
        //the object being picked by the instruction.
        gd::String object = previousParameter;
        if ( object.empty() ) object = context.GetCurrentObject();

        VariableCodeGenerationCallbacks callbacks(argOutput, *this, context, object);

        gd::VariableParser parser(parameter);
        if ( !parser.Parse(callbacks) )
        {
            cout << "Error :" << parser.firstErrorStr << " in: "<< parameter << endl;
            argOutput = "runtimeContext->GetGameVariables().GetBadVariable()";
        }
    }
    else
    {
        argOutput += gd::EventsCodeGenerator::GenerateParameterCodes(parameter, metadata, context, previousParameter, supplementaryParametersTypes);
    }

    return argOutput;
}

gd::String EventsCodeGenerator::GenerateSceneEventsCompleteCode(gd::Project & project, gd::Layout & scene, gd::EventsList & events, bool compilationForRuntime)
{
    gd::String output;

    //Prepare the global context ( Used to get needed header files )
    gd::EventsCodeGenerationContext context;
    EventsCodeGenerator codeGenerator(project, scene);
    codeGenerator.PreprocessEventList(scene.GetEvents());
    codeGenerator.SetGenerateCodeForRuntime(compilationForRuntime);

    //Generate whole events code
    gd::String wholeEventsCode = codeGenerator.GenerateEventsListCode(events, context);

    //Generate default code around events:
    //Includes
    output += "#include <vector>\n#include <map>\n#include <string>\n#include <algorithm>\n#include <SFML/System/Clock.hpp>\n#include <SFML/System/Vector2.hpp>\n#include <SFML/Graphics/Color.hpp>\n#include \"GDCpp/Runtime/RuntimeContext.h\"\n#include \"GDCpp/Runtime/RuntimeObject.h\"\n";
    for ( set<gd::String>::iterator include = codeGenerator.GetIncludeFiles().begin() ; include != codeGenerator.GetIncludeFiles().end(); ++include )
        output += "#include \""+*include+"\"\n";

    //Extra declarations needed by events
    for ( set<gd::String>::iterator declaration = codeGenerator.GetCustomGlobalDeclaration().begin() ; declaration != codeGenerator.GetCustomGlobalDeclaration().end(); ++declaration )
        output += *declaration+"\n";

    output +=
    codeGenerator.GetCustomCodeOutsideMain()+
    "\n"
    "extern \"C\" int GDSceneEvents"+gd::SceneNameMangler::GetMangledSceneName(scene.GetName())+"(RuntimeContext * runtimeContext)\n"
    "{\n"+
    "runtimeContext->StartNewFrame();\n"+
    codeGenerator.GetCustomCodeInMain()+
    wholeEventsCode+
    "return 0;\n"
    "}\n";

    return output;
}

gd::String EventsCodeGenerator::GenerateExternalEventsCompleteCode(gd::Project & project, gd::ExternalEvents & events, bool compilationForRuntime)
{
    DependenciesAnalyzer analyzer(project, events);
    gd::String associatedSceneName = analyzer.ExternalEventsCanBeCompiledForAScene();
    if ( associatedSceneName.empty() || !project.HasLayoutNamed(associatedSceneName) )
    {
        std::cout << "ERROR: Cannot generate code for an external event: No unique associated scene." << std::endl;
        return "";
    }
    gd::Layout & associatedScene = project.GetLayout(project.GetLayoutPosition(associatedSceneName));

    gd::String output;

    //Prepare the global context ( Used to get needed header files )
    gd::EventsCodeGenerationContext context;
    EventsCodeGenerator codeGenerator(project, associatedScene);
    codeGenerator.PreprocessEventList(events.GetEvents());
    codeGenerator.SetGenerateCodeForRuntime(compilationForRuntime);

    //Generate whole events code
    gd::String wholeEventsCode = codeGenerator.GenerateEventsListCode(events.GetEvents(), context);

    //Generate default code around events:
    //Includes
    output += "#include <vector>\n#include <map>\n#include <string>\n#include <algorithm>\n#include <SFML/System/Clock.hpp>\n#include <SFML/System/Vector2.hpp>\n#include <SFML/Graphics/Color.hpp>\n#include \"GDCpp/Runtime/RuntimeContext.h\"\n#include \"GDCpp/Runtime/RuntimeObject.h\"\n";
    for ( set<gd::String>::iterator include = codeGenerator.GetIncludeFiles().begin() ; include != codeGenerator.GetIncludeFiles().end(); ++include )
        output += "#include \""+*include+"\"\n";

    //Extra declarations needed by events
    for ( set<gd::String>::iterator declaration = codeGenerator.GetCustomGlobalDeclaration().begin() ; declaration != codeGenerator.GetCustomGlobalDeclaration().end(); ++declaration )
        output += *declaration+"\n";

    output +=
    codeGenerator.GetCustomCodeOutsideMain()+
    "\n"
    "void "+EventsCodeNameMangler::Get()->GetExternalEventsFunctionMangledName(events.GetName())+"(RuntimeContext * runtimeContext)\n"
    "{\n"
	+codeGenerator.GetCustomCodeInMain()
    +wholeEventsCode+
    "return;\n"
    "}\n";

    return output;
}

EventsCodeGenerator::EventsCodeGenerator(gd::Project & project, const gd::Layout & layout) :
    gd::EventsCodeGenerator(project, layout, CppPlatform::Get())
{
}

EventsCodeGenerator::~EventsCodeGenerator()
{
}

void EventsCodeGenerator::PreprocessEventList( gd::EventsList & eventsList )
{
    #if !defined(GD_NO_WX_GUI) //No support for profiling when wxWidgets is disabled.
    std::shared_ptr<ProfileEvent> previousProfileEvent;
    #endif

    for ( std::size_t i = 0;i < eventsList.size();++i )
    {
        eventsList[i].Preprocess(*this, eventsList, i);
        if ( i < eventsList.size() ) { //Be sure that that there is still an event! ( Preprocess can remove it. )
            if ( eventsList[i].CanHaveSubEvents() )
                PreprocessEventList( eventsList[i].GetSubEvents());

            #if !defined(GD_NO_WX_GUI) //No support for profiling when wxWidgets is disabled.
            if ( scene.GetProfiler() && scene.GetProfiler()->profilingActivated && eventsList[i].IsExecutable() )
            {
                //Define a new profile event
                std::shared_ptr<ProfileEvent> profileEvent = std::shared_ptr<ProfileEvent>(new ProfileEvent);
                profileEvent->originalEvent = eventsList[i].originalEvent;
                profileEvent->SetPreviousProfileEvent(previousProfileEvent);

                //Add it before the event to profile
                eventsList.InsertEvent(profileEvent, i);

                previousProfileEvent = profileEvent;
                ++i; //Don't preprocess the newly added profile event
            }
            #endif
        }
    }

    #if !defined(GD_NO_WX_GUI) //No support for profiling when wxWidgets is disabled.
    if ( !eventsList.IsEmpty() && scene.GetProfiler() && scene.GetProfiler()->profilingActivated )
    {
        //Define a new profile events
        std::shared_ptr<ProfileEvent> profileEvent = std::shared_ptr<ProfileEvent>(new ProfileEvent);
        profileEvent->SetPreviousProfileEvent(previousProfileEvent);

        //Add it at the end of the events list
        eventsList.InsertEvent(profileEvent, eventsList.GetEventsCount());
    }
    #endif
}

#endif
