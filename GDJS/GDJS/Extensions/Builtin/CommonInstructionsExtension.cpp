/*
 * GDevelop JS Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "CommonInstructionsExtension.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#include "GDCore/Extensions/Platform.h"
#include "GDCore/Project/Project.h"
#include "GDCore/Project/Layout.h"
#include "GDCore/Extensions/Metadata/MetadataProvider.h"
#include "GDCore/IDE/SceneNameMangler.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/Events/Builtin/StandardEvent.h"
#include "GDCore/Events/Builtin/CommentEvent.h"
#include "GDCore/Events/Builtin/ForEachEvent.h"
#include "GDCore/Events/Builtin/WhileEvent.h"
#include "GDCore/Events/Builtin/RepeatEvent.h"
#include "GDCore/Events/Builtin/LinkEvent.h"
#include "GDCore/Events/Tools/EventsCodeNameMangler.h"
#include "GDCore/CommonTools.h"
#include "GDCore/String.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerationContext.h"
#include "GDCore/Events/CodeGeneration/ExpressionsCodeGeneration.h"
#include <algorithm>
#include <set>
#include "GDCore/Tools/Localization.h"
#include "GDJS/Events/Builtin/JsCodeEvent.h"

using namespace std;
using namespace gd;

namespace gdjs
{

CommonInstructionsExtension::CommonInstructionsExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsCommonInstructionsExtension(*this);

    SetExtensionInformation("BuiltinCommonInstructions",
                          _("Standard events"),
                          _("Built-in extension providing standard events."),
                          "Florian Rival",
                          "Open source (MIT License)");

    GetAllEvents()["BuiltinCommonInstructions::Link"]
        .SetPreprocessing([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator,
                                gd::EventsList & eventList, unsigned int indexOfTheEventInThisList) {
            gd::LinkEvent & event = dynamic_cast<gd::LinkEvent&>(event_);
            event.ReplaceLinkByLinkedEvents(codeGenerator.GetProject(), eventList, indexOfTheEventInThisList);
        });

    GetAllEvents()["BuiltinCommonInstructions::Standard"]
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context) {
            gd::String outputCode;
            gd::StandardEvent & event = dynamic_cast<gd::StandardEvent&>(event_);

            outputCode += codeGenerator.GenerateConditionsListCode(event.GetConditions(), context);

            gd::String ifPredicat = event.GetConditions().empty() ? "" : codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(event.GetConditions().size()-1)+"IsTrue", context)+".val";

            if ( !ifPredicat.empty() ) outputCode += "if (" +ifPredicat+ ") {\n";
            outputCode += codeGenerator.GenerateActionsListCode(event.GetActions(), context);
            if ( event.HasSubEvents() ) //Sub events
            {
                outputCode += "\n{ //Subevents\n";
                outputCode += codeGenerator.GenerateEventsListCode(event.GetSubEvents(), context);
                outputCode += "} //End of subevents\n";
            }

            if ( !ifPredicat.empty() ) outputCode += "}\n";

            return outputCode;
        });

    GetAllEvents()["BuiltinCommonInstructions::Comment"]
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context) {
            //If we do not add a code generator to the comments,
            //they will be stripped as considered as not implemented by the platform.
            return "";
        });

    GetAllConditions()["BuiltinCommonInstructions::Or"].codeExtraInformation
        .SetCustomCodeGenerator([](gd::Instruction & instruction, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            gd::String codeNamespace = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())+"Code.";

            //Conditions code
            gd::String conditionsCode;
            gd::InstructionsList & conditions = instruction.GetSubInstructions();

            //"OR" condition must declare objects list, but without picking the objects from the scene.
            //Lists are either empty or come from a parent event.
            set<gd::String> emptyListsNeeded;
            for (unsigned int cId =0;cId < conditions.size();++cId)
            {
                //Each condition inherits the context from the "Or" condition:
                //For example, two sub conditions using an object called "MyObject" will both have to declare a "MyObject" object list.
                gd::EventsCodeGenerationContext context;
                context.InheritsFrom(parentContext);

                gd::String conditionCode = codeGenerator.GenerateConditionCode(conditions[cId], "condition"+gd::String::From(cId)+"IsTrue", context);

                conditionsCode += "{\n";

                //Create new objects lists and generate condition
                conditionsCode += codeGenerator.GenerateObjectsDeclarationCode(context);
                if ( !conditions[cId].GetType().empty() ) conditionsCode += conditionCode;

                //If the condition is true : merge all objects picked in the final object lists.
                conditionsCode += "if( "+codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(cId)+"IsTrue", context)+".val ) {\n";
                conditionsCode += "    "+codeGenerator.GenerateBooleanFullName("conditionTrue", context)+".val = true;\n";
                std::set<gd::String> objectsListsToBeDeclared = context.GetAllObjectsToBeDeclared();
                for ( set<gd::String>::iterator it = objectsListsToBeDeclared.begin() ; it != objectsListsToBeDeclared.end(); ++it )
                {
                    emptyListsNeeded.insert(*it);
                    gd::String objList = codeGenerator.GetObjectListName(*it, context);
                    gd::String finalObjList = codeNamespace+ManObjListName(*it)+gd::String::From(parentContext.GetContextDepth())
                        +"_"+gd::String::From(parentContext.GetCurrentConditionDepth())+"final";
                    conditionsCode += "    for(var j = 0, jLen = "+objList+".length;j<jLen;++j) {\n";
                    conditionsCode += "        if ( "+finalObjList+".indexOf("+objList+"[j]) === -1 )\n";
                    conditionsCode += "            "+finalObjList+".push("+objList+"[j]);\n";
                    conditionsCode += "    }\n";
                }
                conditionsCode += "}\n";

                conditionsCode += "}\n";
            }

            gd::String declarationsCode;

            //Declarations code
            for ( set<gd::String>::iterator it = emptyListsNeeded.begin() ; it != emptyListsNeeded.end(); ++it )
            {
                //"OR" condition must declare objects list, but without getting the objects from the scene. Lists are either empty or come from a parent event.
                parentContext.EmptyObjectsListNeeded(*it);
                //We need to duplicate the object lists : The "final" ones will be filled with objects by conditions,
                //but they will have no incidence on further conditions, as conditions use "normal" ones.
                gd::String finalObjList = codeNamespace+ManObjListName(*it)+gd::String::From(parentContext.GetContextDepth())+"_"
                    +gd::String::From(parentContext.GetCurrentConditionDepth())+"final";
                codeGenerator.AddGlobalDeclaration(finalObjList+" = [];\n");
                declarationsCode += finalObjList+".length = 0;";
            }
            for (unsigned int i = 0;i<conditions.size();++i)
                declarationsCode += codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(i)+"IsTrue", parentContext) +".val = false;\n";

            //Generate code
            gd::String code;
            code += declarationsCode;
            code += conditionsCode;

            //When condition is finished, "final" objects lists become the "normal" ones.
            code += "{\n";
            for ( set<gd::String>::iterator it = emptyListsNeeded.begin() ; it != emptyListsNeeded.end(); ++it )
            {
                gd::String finalObjList = codeNamespace+ManObjListName(*it)+gd::String::From(parentContext.GetContextDepth())+"_"
                    +gd::String::From(parentContext.GetCurrentConditionDepth())+"final";
                code += codeGenerator.GetObjectListName(*it, parentContext)+".createFrom("+finalObjList+");\n";
            }
            code += "}\n";

            return code;
        });

    GetAllConditions()["BuiltinCommonInstructions::And"].codeExtraInformation
        .SetCustomCodeGenerator([](gd::Instruction & instruction, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            gd::String outputCode;

            outputCode += codeGenerator.GenerateConditionsListCode(instruction.GetSubInstructions(), parentContext);

            gd::String predicat = "true";
            for (unsigned int i = 0;i<instruction.GetSubInstructions().size();++i)
                predicat += " && "+codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(i)+"IsTrue", parentContext)+".val";

            outputCode += codeGenerator.GenerateBooleanFullName("conditionTrue", parentContext)+".val = "+predicat+";\n";

            return outputCode;
        });

    GetAllConditions()["BuiltinCommonInstructions::Not"].codeExtraInformation
        .SetCustomCodeGenerator([](gd::Instruction & instruction, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context) {
            gd::InstructionsList & conditions = instruction.GetSubInstructions();
            gd::String outputCode;

            for (unsigned int i = 0;i<conditions.size();++i)
            {
                outputCode += codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(i)+"IsTrue", context)+".val = true;\n";
            }

            for (unsigned int cId =0;cId < conditions.size();++cId)
            {
                if (cId != 0) outputCode += "if ( !"+codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(cId-1)+"IsTrue", context)+".val ) {\n";

                gd::InstructionMetadata instrInfos = gd::MetadataProvider::GetConditionMetadata(codeGenerator.GetPlatform(), conditions[cId].GetType());

                gd::String conditionCode = codeGenerator.GenerateConditionCode(conditions[cId], "condition"+gd::String::From(cId)+"IsTrue", context);
                if ( !conditions[cId].GetType().empty() )
                {
                    outputCode += "{\n";
                    outputCode += conditionCode;
                    outputCode += "}";
                }
            }

            for (unsigned int cId =0;cId < conditions.size();++cId)
            {
                if (cId != 0) outputCode += "}\n";
            }

            if ( !conditions.empty() )
            {
                outputCode += codeGenerator.GenerateBooleanFullName("conditionTrue", context)+".val = !";
                outputCode += codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(conditions.size()-1)+"IsTrue", context)+".val;\n";
            }

            return outputCode;
        });

    GetAllConditions()["BuiltinCommonInstructions::Once"].codeExtraInformation
        .SetCustomCodeGenerator([](gd::Instruction & instruction, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context) {
            size_t uniqueId = (size_t)&instruction;
            gd::String outputCode = codeGenerator.GenerateBooleanFullName("conditionTrue", context)+".val = ";
            outputCode += "context.triggerOnce("+gd::String::From(uniqueId)+");\n";
            return outputCode;
        });

    GetAllEvents()["BuiltinCommonInstructions::While"]
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            gd::String outputCode;
            gd::WhileEvent & event = dynamic_cast<gd::WhileEvent&>(event_);

            //Context is "reset" each time the event is repeated ( i.e. objects are picked again )
            gd::EventsCodeGenerationContext context;
            context.InheritsFrom(parentContext);

            //Prepare codes
            gd::String whileConditionsStr = codeGenerator.GenerateConditionsListCode(event.GetWhileConditions(), context);
            gd::String whileIfPredicat = "true";
            if ( !event.GetWhileConditions().empty() )
                whileIfPredicat = codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(event.GetWhileConditions().size()-1)+"IsTrue", context)+".val";

            gd::String conditionsCode = codeGenerator.GenerateConditionsListCode(event.GetConditions(), context);
            gd::String actionsCode = codeGenerator.GenerateActionsListCode(event.GetActions(), context);
            gd::String ifPredicat = "true";
            if ( !event.GetConditions().empty() )
                ifPredicat = codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(event.GetConditions().size()-1)+"IsTrue", context)+".val";

            //Write final code
            gd::String whileBoolean = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.stopDoWhile"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(whileBoolean+" = false;\n");
            outputCode += whileBoolean+" = false;\n";
            outputCode += "do {";
            outputCode += codeGenerator.GenerateObjectsDeclarationCode(context);
            outputCode +=  whileConditionsStr;
            outputCode += "if ("+whileIfPredicat+") {\n";
            outputCode += conditionsCode;
            outputCode += "if (" +ifPredicat+ ") {\n";
            outputCode += actionsCode;
            outputCode += "\n{ //Subevents: \n";
            outputCode += codeGenerator.GenerateEventsListCode(event.GetSubEvents(), context);
            outputCode += "} //Subevents end.\n";
            outputCode += "}\n";
            outputCode += "} else "+whileBoolean+" = true; \n";

            outputCode += "} while ( !"+whileBoolean+" );\n";

            return outputCode;
        });

    GetAllEvents()["BuiltinCommonInstructions::Repeat"]
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            gd::String outputCode;
            gd::RepeatEvent & event = dynamic_cast<gd::RepeatEvent&>(event_);

            const gd::Layout & scene = codeGenerator.GetLayout();

            gd::String repeatNumberExpression = event.GetRepeatExpression();

            //Prepare expression containing how many times event must be repeated
            gd::String repeatCountCode;
            gd::CallbacksForGeneratingExpressionCode callbacks(repeatCountCode, codeGenerator, parentContext);
            gd::ExpressionParser parser(repeatNumberExpression);
            if (!parser.ParseMathExpression(codeGenerator.GetPlatform(), codeGenerator.GetProject(), scene, callbacks) || repeatCountCode.empty()) repeatCountCode = "0";

            //Context is "reset" each time the event is repeated ( i.e. objects are picked again )
            gd::EventsCodeGenerationContext context;
            context.InheritsFrom(parentContext);

            //Prepare conditions/actions codes
            gd::String conditionsCode = codeGenerator.GenerateConditionsListCode(event.GetConditions(), context);
            gd::String actionsCode = codeGenerator.GenerateActionsListCode(event.GetActions(), context);
            gd::String ifPredicat = "true";
            if ( !event.GetConditions().empty() )
                ifPredicat = codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(event.GetConditions().size()-1)+"IsTrue", context)+".val";

            //Prepare object declaration and sub events
            gd::String subevents = codeGenerator.GenerateEventsListCode(event.GetSubEvents(), context);
            gd::String objectDeclaration = codeGenerator.GenerateObjectsDeclarationCode(context)+"\n";

            //Write final code
            gd::String repeatCountVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.repeatCount"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(repeatCountVar+" = 0;\n");
            gd::String repeatIndexVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.repeatIndex"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(repeatIndexVar+" = 0;\n");
            outputCode += repeatCountVar+" = "+repeatCountCode+";\n";
            outputCode += "for("+repeatIndexVar+" = 0;"+repeatIndexVar+" < "+repeatCountVar+";++"+repeatIndexVar+") {\n";
            outputCode += objectDeclaration;
            outputCode += conditionsCode;
            outputCode += "if (" +ifPredicat+ ")\n";
            outputCode += "{\n";
            outputCode += actionsCode;
            if ( event.HasSubEvents() )
            {
                outputCode += "\n{ //Subevents: \n";
                outputCode += subevents;
                outputCode += "} //Subevents end.\n";
            }
            outputCode += "}\n";

            outputCode += "}\n";

            return outputCode;
        });

    GetAllEvents()["BuiltinCommonInstructions::ForEach"]
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            gd::String outputCode;
            gd::ForEachEvent & event = dynamic_cast<gd::ForEachEvent&>(event_);

            std::vector<gd::String> realObjects = codeGenerator.ExpandObjectsName(
                event.GetObjectToPick(), parentContext);

            if ( realObjects.empty() ) return gd::String("");
            for (unsigned int i = 0;i<realObjects.size();++i)
                parentContext.ObjectsListNeeded(realObjects[i]);

            //Context is "reset" each time the event is repeated ( i.e. objects are picked again )
            gd::EventsCodeGenerationContext context;
            context.InheritsFrom(parentContext);

            for (unsigned int i = 0;i<realObjects.size();++i)
                context.ObjectsListNeeded(realObjects[i]);

            //Prepare conditions/actions codes
            gd::String conditionsCode = codeGenerator.GenerateConditionsListCode(event.GetConditions(), context);
            gd::String actionsCode = codeGenerator.GenerateActionsListCode(event.GetActions(), context);
            gd::String ifPredicat = "true";
            if ( !event.GetConditions().empty() )
                ifPredicat = codeGenerator.GenerateBooleanFullName("condition"+gd::String::From(event.GetConditions().size()-1)+"IsTrue", context)+".val";

            //Prepare object declaration and sub events
            gd::String subevents = codeGenerator.GenerateEventsListCode(event.GetSubEvents(), context);

            gd::String objectDeclaration = codeGenerator.GenerateObjectsDeclarationCode(context)+"\n";

            gd::String forEachTotalCountVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.forEachTotalCount"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(forEachTotalCountVar+" = 0;\n");
            gd::String forEachIndexVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.forEachIndex"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(forEachIndexVar+" = 0;\n");
            gd::String forEachObjectsList = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                +"Code.forEachObjects"+gd::String::From(context.GetContextDepth());
            codeGenerator.AddGlobalDeclaration(forEachObjectsList+" = [];\n");

            if ( realObjects.size() != 1) //(We write a slighty more simple ( and optimized ) output code when only one object list is used.)
            {
                outputCode += forEachTotalCountVar + " = 0;\n";
                outputCode += forEachObjectsList+".length = 0;\n";
                for (unsigned int i = 0;i<realObjects.size();++i)
                {
                    gd::String forEachCountVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                        +"Code.forEachCount"+gd::String::From(i)+"_"+gd::String::From(context.GetContextDepth());
                    codeGenerator.AddGlobalDeclaration(forEachCountVar+" = 0;\n");

                    outputCode += forEachCountVar+" = "+codeGenerator.GetObjectListName(realObjects[i], parentContext)+".length;\n";
                    outputCode += forEachTotalCountVar + " += " + forEachCountVar+";\n";
                    outputCode += forEachObjectsList + ".push.apply("+forEachObjectsList+","+codeGenerator.GetObjectListName(realObjects[i], parentContext)+");\n";
                }
            }

            //Write final code :

            //For loop declaration
            if ( realObjects.size() == 1 ) //We write a slighty more simple ( and optimized ) output code when only one object list is used.
                outputCode += "for("+forEachIndexVar+" = 0;"+forEachIndexVar+" < "+codeGenerator.GetObjectListName(realObjects[0], parentContext)+".length;++"+forEachIndexVar+") {\n";
            else
                outputCode += "for("+forEachIndexVar+" = 0;"+forEachIndexVar+" < "+forEachTotalCountVar+";++"+forEachIndexVar+") {\n";

            outputCode += objectDeclaration;

            //Clear all concerned objects lists and keep only one object
            if ( realObjects.size() == 1 )
            {
                gd::String temporary = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                    +"Code.forEachTemporary"+gd::String::From(context.GetContextDepth());
                codeGenerator.AddGlobalDeclaration(temporary+" = null;\n");
                outputCode += temporary+" = "+codeGenerator.GetObjectListName(realObjects[0], parentContext)+"["+forEachIndexVar+"];\n";
                outputCode += codeGenerator.GetObjectListName(realObjects[0], context)+".length = 0;\n";
                outputCode += codeGenerator.GetObjectListName(realObjects[0], context)+".push("+temporary+");\n";
            }
            else
            {
                //Declare all lists of concerned objects empty
                for (unsigned int j = 0;j<realObjects.size();++j)
                    outputCode += codeGenerator.GetObjectListName(realObjects[j], context) + ".length = 0;\n";

                for (unsigned int i = 0;i<realObjects.size();++i) //Pick then only one object
                {
                    gd::String count;
                    for (unsigned int j = 0;j<=i;++j)
                    {
                        gd::String forEachCountVar = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(codeGenerator.GetLayout().GetName())
                            +"Code.forEachCount"+gd::String::From(j)+"_"+gd::String::From(context.GetContextDepth());

                        if (j!=0) count+= "+";
                        count += forEachCountVar;
                    }

                    if ( i != 0 ) outputCode += "else ";
                    outputCode += "if ("+forEachIndexVar+" < "+count+") {\n";
                    outputCode += "    "+codeGenerator.GetObjectListName(realObjects[i], context)+".push("+forEachObjectsList+"["+forEachIndexVar+"]);\n";
                    outputCode += "}\n";
                }
            }

            outputCode += conditionsCode;
            outputCode += "if (" +ifPredicat+ ") {\n";
            outputCode += actionsCode;
            if ( event.HasSubEvents() )
            {
                outputCode += "\n{ //Subevents: \n";
                outputCode += subevents;
                outputCode += "} //Subevents end.\n";
            }
            outputCode += "}\n";

            outputCode += "}\n"; //End of for loop

            return outputCode;
        });

    GetAllEvents()["BuiltinCommonInstructions::Group"]
        .SetCodeGenerator([](gd::BaseEvent & event, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context) {
            return codeGenerator.GenerateEventsListCode(event.GetSubEvents(), context);
        });

    AddEvent("JsCode",
              _("Javascript code (Web platform only)"),
              _("Insert some Javascript code into events"),
              "",
              "res/source_cpp16.png",
              std::shared_ptr<gd::BaseEvent>(new JsCodeEvent))
        .SetCodeGenerator([](gd::BaseEvent & event_, gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & parentContext) {
            JsCodeEvent & event = dynamic_cast<JsCodeEvent&>(event_);
            const gd::Layout & scene = codeGenerator.GetLayout();

            gd::String functionName = "gdjs."+gd::SceneNameMangler::GetMangledSceneName(scene.GetName())+"Code.userFunc"
                + gd::String::From(&event);
            gd::String callArguments = "runtimeScene";
            if (!event.GetParameterObjects().empty()) callArguments += ", objects";

            //Generate the function code
            gd::String functionCode;
            functionCode += functionName + " = function("+callArguments+") {\n";
            functionCode += event.GetInlineCode();
            functionCode += "\n};\n";
            codeGenerator.AddCustomCodeOutsideMain(functionCode);

            //Generate the code to call the function
            gd::String callingCode;
            if (!event.GetParameterObjects().empty())
            {
                std::vector<gd::String> realObjects =
                    codeGenerator.ExpandObjectsName(event.GetParameterObjects(), parentContext);

                callingCode += "var objects = [];\n";
                for (unsigned int i = 0;i<realObjects.size();++i)
                {
                    parentContext.ObjectsListNeeded(realObjects[i]);
                    callingCode += "objects.push.apply(objects,"
                        + codeGenerator.GetObjectListName(realObjects[i], parentContext)+");\n";
                }
            }

            callingCode += functionName + "(" + callArguments + ");\n";
            return callingCode;
        });
}

}
