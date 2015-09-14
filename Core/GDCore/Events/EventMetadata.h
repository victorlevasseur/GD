/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#if defined(GD_IDE_ONLY)
#ifndef EVENTMETADATA_H
#define EVENTMETADATA_H
#include "GDCore/String.h"
#include <vector>
#include <memory>
#include <functional>
#if !defined(GD_NO_WX_GUI)
#include <wx/bitmap.h>
#endif
class wxBitmap;
namespace gd { class EventsList; }
namespace gd { class BaseEvent; }
namespace gd { class EventsCodeGenerator; }
namespace gd { class EventsCodeGenerationContext; }

namespace gd
{

/**
 * \brief Describe an event provided by an extension of a platform.
 *
 * Extensions writers, you should most of the time be using PlatformExtension::AddEvent method
 * to add events.
 */
class GD_CORE_API EventMetadata
{
public:
    /**
     * \brief Set the code generator used when generating code from events.
     */
    EventMetadata & SetCodeGenerator(std::function<gd::String(gd::BaseEvent & event, gd::EventsCodeGenerator & codeGenerator,
        gd::EventsCodeGenerationContext & context)> function) {
        hasCustomCodeGenerator = true;
        codeGeneration = function;
        return *this;
    }

    /**
     * \brief Set the code to preprocess the event.
     */
    EventMetadata & SetPreprocessing(std::function<void(gd::BaseEvent & event, gd::EventsCodeGenerator & codeGenerator,
        gd::EventsList & eventList, std::size_t indexOfTheEventInThisList)> function) {
        preprocessing = function;
        return *this;
    }

    /**
     * \brief Reset the code generation and preprocessing functions of the event.
     */
    void ClearCodeGenerationAndPreprocessing();

    /**
     * \brief Return true if SetCodeGenerator was called to set a function to call
     * to generate the event code.
     */
    bool HasCustomCodeGenerator() const { return hasCustomCodeGenerator; }

    EventMetadata(const gd::String & name_,
                 const gd::String & fullname_,
                 const gd::String & description_,
                 const gd::String & group_,
                 const gd::String & smallicon_,
                 std::shared_ptr<gd::BaseEvent> instance);

    EventMetadata() {};
    virtual ~EventMetadata() {};

    const gd::String & GetFullName() const { return fullname; }
    const gd::String & GetDescription() const { return description; }
    const gd::String & GetGroup() const { return group; }
#if !defined(GD_NO_WX_GUI)
    const wxBitmap & GetBitmapIcon() const { return smallicon; }
#endif

    gd::String fullname;
    gd::String description;
    gd::String group;
#if !defined(GD_NO_WX_GUI)
    wxBitmap smallicon;
#endif

    std::shared_ptr<gd::BaseEvent> instance;
    bool hasCustomCodeGenerator;
    std::function<gd::String(gd::BaseEvent & event, gd::EventsCodeGenerator & codeGenerator,
        gd::EventsCodeGenerationContext & context)> codeGeneration;
    std::function<void(gd::BaseEvent & event, gd::EventsCodeGenerator & codeGenerator,
        gd::EventsList & eventList, std::size_t indexOfTheEventInThisList)> preprocessing;
};

}

#endif // EVENTMETADATA_H
#endif
