/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#include "GDCore/Events/Event.h"
#include "GDCore/Events/EventsList.h"
#include "GDCore/Extensions/Platform.h"
#include "GDCore/Extensions/PlatformExtension.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"

namespace gd
{

EventsList BaseEvent::badSubEvents;

BaseEvent::BaseEvent() :
    eventHeightNeedUpdate(true),
    totalTimeDuringLastSession(0),
    percentDuringLastSession(0),
    disabled(false),
    folded(false)
{
}

bool BaseEvent::HasSubEvents() const
{
    return !GetSubEvents().IsEmpty();
}

gd::String BaseEvent::GenerateEventCode(gd::EventsCodeGenerator & codeGenerator, gd::EventsCodeGenerationContext & context)
{
    if ( IsDisabled() ) return "";

    try
    {
        if ( type.empty() ) return "";

        const gd::Platform & platform = codeGenerator.GetPlatform();

        //First try to guess the extension used
        gd::String eventNamespace = type.substr(0, type.find("::"));
        std::shared_ptr<gd::PlatformExtension> guessedExtension = platform.GetExtension(eventNamespace);
        if ( guessedExtension )
        {
            std::map<gd::String, gd::EventMetadata > & allEvents = guessedExtension->GetAllEvents();
            if ( allEvents.find(type) != allEvents.end() )
                return allEvents[type].codeGeneration(*this, codeGenerator, context);
        }


        //Else make a search in all the extensions
        for (std::size_t i = 0; i < platform.GetAllPlatformExtensions().size(); ++i)
        {
            std::shared_ptr<gd::PlatformExtension> extension = platform.GetAllPlatformExtensions()[i];
            if ( !extension ) continue;

            std::map<gd::String, gd::EventMetadata > & allEvents = extension->GetAllEvents();
            if ( allEvents.find(type) != allEvents.end() )
                return allEvents[type].codeGeneration(*this, codeGenerator, context);
        }
    }
    catch(...)
    {
        std::cout << "ERROR: Exception caught during code generation for event \"" << type <<"\"." << std::endl;
    }

    return "";
}

void BaseEvent::Preprocess(gd::EventsCodeGenerator & codeGenerator, gd::EventsList & eventList, std::size_t indexOfTheEventInThisList)
{
    if ( IsDisabled() || !MustBePreprocessed() ) return;

    try
    {
        if ( type.empty() ) return;

        const gd::Platform & platform = codeGenerator.GetPlatform();

        //First try to guess the extension used
        gd::String eventNamespace = type.substr(0, type.find("::"));
        std::shared_ptr<gd::PlatformExtension> guessedExtension = platform.GetExtension(eventNamespace);
        if ( guessedExtension )
        {
            std::map<gd::String, gd::EventMetadata > & allEvents = guessedExtension->GetAllEvents();
            if ( allEvents.find(type) != allEvents.end() )
                return allEvents[type].preprocessing(*this, codeGenerator, eventList, indexOfTheEventInThisList);
        }

        //Else make a search in all the extensions
        for (std::size_t i = 0;i<platform.GetAllPlatformExtensions().size();++i)
        {
            std::shared_ptr<gd::PlatformExtension> extension = platform.GetAllPlatformExtensions()[i];
            if ( !extension ) continue;

            std::map<gd::String, gd::EventMetadata > & allEvents = extension->GetAllEvents();
            if ( allEvents.find(type) != allEvents.end() )
                return allEvents[type].preprocessing(*this, codeGenerator, eventList, indexOfTheEventInThisList);
        }
    }
    catch(...)
    {
        std::cout << "ERROR: Exception caught during preprocessing of event \"" << type <<"\"." << std::endl;
    }
}

BaseEventSPtr GD_CORE_API CloneRememberingOriginalEvent(BaseEventSPtr event)
{
    gd::BaseEventSPtr copy(event->Clone());
    //Original event is either the original event of the copied event, or the event copied.
    copy->originalEvent = event->originalEvent.expired() ? event : event->originalEvent;

    return copy;
}

}
