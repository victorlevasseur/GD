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

bool EventsEditorItemsAreas::IsOnEvent(int x, int y)
{
    for (unsigned int i = 0;i<eventsAreas.size();++i)
    {
        if ( eventsAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

EventItem EventsEditorItemsAreas::GetEventAt(int x, int y)
{
    for (unsigned int i = 0;i<eventsAreas.size();++i)
    {
        if ( eventsAreas[i].first.Contains(x,y) )
            return eventsAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY EVENT";

    EventItem dummy;
    return dummy;
}
wxRect EventsEditorItemsAreas::GetAreaOfEventAt(int x, int y)
{
    for (unsigned int i = 0;i<eventsAreas.size();++i)
    {
        if ( eventsAreas[i].first.Contains(x,y) )
            return eventsAreas[i].first;
    }

    std::cout << "WARNING, RETURNING DUMMY EVENT rect";

    wxRect dummy;
    return dummy;
}

wxRect EventsEditorItemsAreas::GetAreaOfEvent(const EventItem &eventItem)
{
    for (unsigned int i = 0;i<eventsAreas.size();++i)
    {
        if ( eventsAreas[i].second == eventItem )
            return eventsAreas[i].first;
    }

    std::cout << "WARNING, CAN'T FIND THE AREA OF AN EVENT ITEM";

    return wxRect();
}

bool EventsEditorItemsAreas::IsOnInstruction(int x, int y)
{
    for (unsigned int i = 0;i<instructionsAreas.size();++i)
    {
        if ( instructionsAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

InstructionItem EventsEditorItemsAreas::GetInstructionAt(int x, int y)
{
    for (unsigned int i = 0;i<instructionsAreas.size();++i)
    {
        if ( instructionsAreas[i].first.Contains(x,y) )
            return instructionsAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY Instruction";

    gd::InstructionItem dummy;
    return dummy;
}

wxRect EventsEditorItemsAreas::GetAreaOfInstructionAt(int x, int y)
{
    for (unsigned int i = 0;i<instructionsAreas.size();++i)
    {
        if ( instructionsAreas[i].first.Contains(x,y) )
            return instructionsAreas[i].first;
    }

    std::cout << "WARNING, RETURNING DUMMY Instruction area";

    wxRect dummy;
    return dummy;
}

bool EventsEditorItemsAreas::IsOnInstructionList(int x, int y)
{
    for (unsigned int i = 0;i<instructionListsAreas.size();++i)
    {
        if ( instructionListsAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

InstructionListItem EventsEditorItemsAreas::GetInstructionListAt(int x, int y)
{
    wxRect rect;
    InstructionListItem instructionList;
    for (unsigned int i = 0;i<instructionListsAreas.size();++i)
    {
        if ( instructionListsAreas[i].first.Contains(x,y) )
        {
            if ( (instructionListsAreas[i].first.width*instructionListsAreas[i].first.height < rect.width*rect.height) || rect.width == 0 )
            {
                rect = instructionListsAreas[i].first;
                instructionList = instructionListsAreas[i].second;
            }
        }
    }

    if ( rect.width == 0)
        std::cout << "WARNING, RETURNING DUMMY InstructionListItem";

    return instructionList;
}

wxRect EventsEditorItemsAreas::GetAreaOfInstructionListAt(int x, int y)
{
    wxRect rect;
    for (unsigned int i = 0;i<instructionListsAreas.size();++i)
    {
        if ( instructionListsAreas[i].first.Contains(x,y) )
        {
            if ( (instructionListsAreas[i].first.width*instructionListsAreas[i].first.height < rect.width*rect.height) || rect.width == 0 )
                rect = instructionListsAreas[i].first;
        }
    }

    if ( rect.width == 0)
        std::cout << "WARNING, RETURNING DUMMY InstructionListItem Rect";

    return rect;
}

bool EventsEditorItemsAreas::IsOnParameter(int x, int y)
{
    for (unsigned int i = 0;i<parametersAreas.size();++i)
    {
        if ( parametersAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

ParameterItem EventsEditorItemsAreas::GetParameterAt(int x, int y)
{
    for (unsigned int i = 0;i<parametersAreas.size();++i)
    {
        if ( parametersAreas[i].first.Contains(x,y) )
            return parametersAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY Parameter";

    ParameterItem dummy;
    return dummy;
}

wxRect EventsEditorItemsAreas::GetAreaOfParameterAt(int x, int y)
{
    for (unsigned int i = 0;i<parametersAreas.size();++i)
    {
        if ( parametersAreas[i].first.Contains(x,y) )
            return parametersAreas[i].first;
    }

    std::cout << "WARNING, RETURNING DUMMY Parameter Rect";

    wxRect dummy;
    return dummy;
}

bool EventsEditorItemsAreas::IsOnFoldingItem(int x, int y)
{
    for (unsigned int i = 0;i<foldingAreas.size();++i)
    {
        if ( foldingAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

FoldingItem EventsEditorItemsAreas::GetFoldingItemAt(int x, int y)
{
    for (unsigned int i = 0;i<foldingAreas.size();++i)
    {
        if ( foldingAreas[i].first.Contains(x,y) )
            return foldingAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY FoldingItem";

    FoldingItem dummy;
    return dummy;
}

bool EventsEditorItemsAreas::IsOnInstructionAdderItem(int x, int y)
{
    for (unsigned int i = 0;i<instructionAdderAreas.size();++i)
    {
        if ( instructionAdderAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

InstructionAdderItem EventsEditorItemsAreas::GetInstructionAdderItemAt(int x, int y)
{
    for (unsigned int i = 0;i<instructionAdderAreas.size();++i)
    {
        if ( instructionAdderAreas[i].first.Contains(x,y) )
            return instructionAdderAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY InstructionAdderItem";

    InstructionAdderItem dummy;
    return dummy;
}

bool EventsEditorItemsAreas::IsOnEventAdderItem(int x, int y)
{
    for (unsigned int i = 0;i<eventAdderAreas.size();++i)
    {
        if ( eventAdderAreas[i].first.Contains(x,y) )
            return true;
    }

    return false;
}

EventAdderItem EventsEditorItemsAreas::GetEventAdderItemAt(int x, int y)
{
    for (unsigned int i = 0;i<eventAdderAreas.size();++i)
    {
        if ( eventAdderAreas[i].first.Contains(x,y) )
            return eventAdderAreas[i].second;
    }

    std::cout << "WARNING, RETURNING DUMMY InstructionAdderItem";

    EventAdderItem dummy;
    return dummy;
}

wxRect EventsEditorItemsAreas::GetAreaOfEventAdderItem(const gd::EventAdderItem &eventAdderItem)
{
    for (unsigned int i = 0;i<eventAdderAreas.size();++i)
    {
        if ( eventAdderAreas[i].second == eventAdderItem )
            return eventAdderAreas[i].first;
    }

    std::cout << "WARNING, CAN'T FIND THE AREA OF AN EVENT ADDER ITEM";

    return wxRect();
}

void EventsEditorItemsAreas::Clear()
{
    eventsAreas.clear();
    instructionsAreas.clear();
    instructionListsAreas.clear();
    parametersAreas.clear();
    foldingAreas.clear();
    instructionAdderAreas.clear();
    eventAdderAreas.clear();
}

void EventsEditorItemsAreas::AddInstructionArea(wxRect area, gd::InstructionItem & instruction)
{
    instructionsAreas.push_back(std::make_pair(area, instruction));
}

void EventsEditorItemsAreas::AddParameterArea(wxRect area, ParameterItem & parameter)
{
    parametersAreas.push_back(std::make_pair(area, parameter));
}

void EventsEditorItemsAreas::AddEventArea(wxRect area, gd::EventItem & event)
{
    eventsAreas.push_back(std::make_pair(area, event));
}

void EventsEditorItemsAreas::AddFoldingItem(wxRect area, FoldingItem & item)
{
    foldingAreas.push_back(std::make_pair(area, item));
}

void EventsEditorItemsAreas::AddInstructionListArea(wxRect area, InstructionListItem & item)
{
    instructionListsAreas.push_back(std::make_pair(area, item));
}

void EventsEditorItemsAreas::AddInstructionAdderItem(wxRect area, InstructionAdderItem & item)
{
    instructionAdderAreas.push_back(std::make_pair(area, item));
}

void EventsEditorItemsAreas::AddEventAdderItem(wxRect area, EventAdderItem & item)
{
    eventAdderAreas.push_back(std::make_pair(area, item));
}

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
