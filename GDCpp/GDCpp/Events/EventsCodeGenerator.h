/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#if defined(GD_IDE_ONLY)

#ifndef EventsCodeGenerator_H
#define EventsCodeGenerator_H
#include <vector>
#include <string>
#include "GDCore/Events/Event.h"
#include "GDCore/Events/EventsCodeGenerator.h"
namespace gd { class ObjectMetadata; }
namespace gd { class BehaviorMetadata; }
namespace gd { class InstructionMetadata; }
namespace gd { class ExpressionCodeGenerationInformation; }

class GD_API EventsCodeGenerator : public gd::EventsCodeGenerator
{
  friend class VariableCodeGenerationCallbacks;
public:

    /**
     * Generate complete C++ file for compiling events of a scene
     *
     * \param project Game used
     * \param scene Scene used
     * \param events events of the scene
     * \param compilationForRuntime Set this to true if the code is generated for runtime.
     * \return C++ code
     */
    static gd::String GenerateSceneEventsCompleteCode(gd::Project & project, gd::Layout & scene, gd::EventsList & events, bool compilationForRuntime = false);

    /**
     * Generate complete C++ file for compiling external events.
     * \note If events.AreCompiled() == false, no code is generated.
     *
     * \param project Game used
     * \param events External events used.
     * \param compilationForRuntime Set this to true if the code is generated for runtime.
     * \return C++ code
     */
    static gd::String GenerateExternalEventsCompleteCode(gd::Project & project, gd::ExternalEvents & events, bool compilationForRuntime = false);

    /**
     * \brief GD C++ Platform has a specific processing function so as to handle profiling.
     */
    void PreprocessEventList( gd::EventsList & listEvent );

protected:
    virtual gd::String GenerateParameterCodes(const gd::String & parameter, const gd::ParameterMetadata & metadata,
                                               gd::EventsCodeGenerationContext & context,
                                               const gd::String & previousParameter,
                                               std::vector < std::pair<gd::String, gd::String> > * supplementaryParametersTypes);

    virtual gd::String GenerateObjectFunctionCall(gd::String objectListName,
                                                          const gd::ObjectMetadata & objMetadata,
                                                          const gd::ExpressionCodeGenerationInformation & codeInfo,
                                                          gd::String parametersStr,
                                                          gd::String defaultOutput,
                                                          gd::EventsCodeGenerationContext & context);

    virtual gd::String GenerateObjectBehaviorFunctionCall(gd::String objectListName,
                                                                      gd::String behaviorName,
                                                                      const gd::BehaviorMetadata & autoInfo,
                                                                      const gd::ExpressionCodeGenerationInformation & codeInfo,
                                                                      gd::String parametersStr,
                                                                      gd::String defaultOutput,
                                                                      gd::EventsCodeGenerationContext & context);

    virtual gd::String GenerateObjectCondition(const gd::String & objectName,
                                                            const gd::ObjectMetadata & objInfo,
                                                            const std::vector<gd::String> & arguments,
                                                            const gd::InstructionMetadata & instrInfos,
                                                            const gd::String & returnBoolean,
                                                            bool conditionInverted,
                                                            gd::EventsCodeGenerationContext & context);

    virtual gd::String GenerateBehaviorCondition(const gd::String & objectName,
                                                                const gd::String & behaviorName,
                                                                const gd::BehaviorMetadata & autoInfo,
                                                                const std::vector<gd::String> & arguments,
                                                                const gd::InstructionMetadata & instrInfos,
                                                                const gd::String & returnBoolean,
                                                                bool conditionInverted,
                                                                gd::EventsCodeGenerationContext & context);

    virtual gd::String GenerateObjectAction(const gd::String & objectName,
                                                        const gd::ObjectMetadata & objInfo,
                                                        const std::vector<gd::String> & arguments,
                                                        const gd::InstructionMetadata & instrInfos,
                                                        gd::EventsCodeGenerationContext & context);

    virtual gd::String GenerateBehaviorAction(const gd::String & objectName,
                                                            const gd::String & behaviorName,
                                                            const gd::BehaviorMetadata & autoInfo,
                                                            const std::vector<gd::String> & arguments,
                                                            const gd::InstructionMetadata & instrInfos,
                                                            gd::EventsCodeGenerationContext & context);

    /**
     * \brief Construct a code generator for the specified project and layout.
     */
    EventsCodeGenerator(gd::Project & project, const gd::Layout & layout);
    virtual ~EventsCodeGenerator();
};

#endif // EventsCodeGenerator_H
#endif
