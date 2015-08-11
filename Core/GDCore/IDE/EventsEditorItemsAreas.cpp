/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)

#include "EventsEditorItemsAreas.h"
#include <wx/gdicmn.h>
#include <functional>
#include <map>
#include "GDCore/Events/Event.h"

namespace gd {

//EventItem stuff :
bool EventItem::operator==(const gd::EventItem & other) const
{
    return (event == other.event && eventsList == other.eventsList && positionInList == other.positionInList);
}

EventItem::EventItem(std::shared_ptr<gd::BaseEvent> event_, gd::EventsList * eventsList_, unsigned int positionInList_ ) :
    event(event_),
    eventsList(eventsList_),
    positionInList(positionInList_)
{

}

EventItem::EventItem() :
    eventsList(NULL),
    positionInList(0)
{

}

//InstructionItem stuff :
bool gd::InstructionItem::operator==(const gd::InstructionItem & other) const
{
    return (instruction == other.instruction && isCondition == other.isCondition && instructionList == other.instructionList && positionInList == other.positionInList && event == other.event);
}

InstructionItem::InstructionItem(gd::Instruction * instruction_, bool isCondition_, gd::InstructionsList* instructionList_, unsigned int positionInList_, gd::BaseEvent * event_ ) :
    instruction(instruction_),
    isCondition(isCondition_),
    instructionList(instructionList_),
    positionInList(positionInList_),
    event(event_)
{
}

InstructionItem::InstructionItem() :
    instruction(NULL),
    isCondition(true),
    instructionList(NULL),
    positionInList(0),
    event(NULL)
{
}

//InstructionListItem stuff :
bool InstructionListItem::operator==(const InstructionListItem & other) const
{
    return (isConditionList == other.isConditionList && instructionList == other.instructionList && event == other.event);
}

InstructionListItem::InstructionListItem(bool isCondition_, gd::InstructionsList* instructionList_, gd::BaseEvent * event_ ) :
    isConditionList(isCondition_),
    instructionList(instructionList_),
    event(event_)
{
}

InstructionListItem::InstructionListItem() :
    isConditionList(true),
    instructionList(NULL),
    event(NULL)
{
}
//ParameterItem stuff :
bool ParameterItem::operator==(const ParameterItem & other) const
{
    return (parameter == other.parameter && event == other.event);
}

ParameterItem::ParameterItem(gd::Expression * parameter_, gd::BaseEvent * event_) :
    parameter(parameter_),
    event(event_)
{
}

ParameterItem::ParameterItem() :
    parameter(NULL),
    event(NULL)
{
}

//FoldingItem stuff :
bool FoldingItem::operator==(const FoldingItem & other) const
{
    return (event == other.event);
}

FoldingItem::FoldingItem(gd::BaseEvent * event_) :
    event(event_)
{
}

FoldingItem::FoldingItem() :
    event(NULL)
{
}

//InstructionAdderItem stuff :
bool InstructionAdderItem::operator==(const InstructionAdderItem & other) const
{
    return (isConditionList == other.isConditionList && instructionList == other.instructionList && event == other.event);
}

InstructionAdderItem::InstructionAdderItem(bool isCondition_, gd::InstructionsList* instructionList_, gd::BaseEvent * event_ ) :
    isConditionList(isCondition_),
    instructionList(instructionList_),
    event(event_)
{
}

InstructionAdderItem::InstructionAdderItem() :
    isConditionList(true),
    instructionList(NULL),
    event(NULL)
{
}

//EventAdderItem stuff :
bool EventAdderItem::operator==(const gd::EventAdderItem & other) const
{
    return (event == other.event && eventsList == other.eventsList && positionInList == other.positionInList);
}

EventAdderItem::EventAdderItem(std::shared_ptr<gd::BaseEvent> event_, gd::EventsList * eventsList_, unsigned int positionInList_ ) :
    event(event_),
    eventsList(eventsList_),
    positionInList(positionInList_)
{

}

EventAdderItem::EventAdderItem() :
    eventsList(NULL),
    positionInList(0)
{

}


}

#endif
