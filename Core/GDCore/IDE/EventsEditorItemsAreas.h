/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#ifndef EVENTSEDITORITEMSAREAS_H
#define EVENTSEDITORITEMSAREAS_H
#include <wx/gdicmn.h>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include "GDCore/Events/InstructionsList.h"
namespace gd { class EventsList; }
namespace gd { class BaseEvent; }
namespace gd { class Instruction; }
namespace gd { class Expression; }

namespace gd
{

/**
 * \brief Base class for all classes declaring event editor items.
 */
struct GD_CORE_API EventsEditorItem
{
    EventsEditorItem() {};
    virtual ~EventsEditorItem() {};


};

/**
 * \brief Tool class to store information about an event.
 *
 * Used by events editor to indicate to EventsEditorItemsAreas that an event is displayed somewhere.
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API EventItem : public EventsEditorItem
{
    EventItem(std::shared_ptr<gd::BaseEvent> event_, gd::EventsList * eventsList_, std::size_t positionInList_ );
    EventItem();
    virtual ~EventItem() {};

    bool operator==(const gd::EventItem & other) const;

    std::shared_ptr<gd::BaseEvent> event;
    gd::EventsList * eventsList;
    std::size_t positionInList;
};

/**
 * \brief Used to indicate to EventsEditorItemsAreas that an instruction is displayed somewhere
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API InstructionItem : public EventsEditorItem
{
    /**
     * Use this constructor to declare the instruction, the list it belongs to and its position in this list.
     */
    InstructionItem(gd::Instruction * instruction_, bool isCondition, gd::InstructionsList* instructionList_, std::size_t positionInList_, gd::BaseEvent * event );
    InstructionItem();
    virtual ~InstructionItem() {};

    bool operator==(const gd::InstructionItem & other) const;

    gd::Instruction * instruction;
    bool isCondition;
    gd::InstructionsList* instructionList;
    std::size_t positionInList;
    gd::BaseEvent * event;
};

/**
 * \brief Used to indicate to EventsEditorItemsAreas that an instruction list is displayed somewhere
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API InstructionListItem : public EventsEditorItem
{
    /**
     * Use this constructor to declare the instruction, the list it belongs to and its position in this list.
     */
    InstructionListItem(bool isConditionList, gd::InstructionsList* instructionList_, gd::BaseEvent * event );
    InstructionListItem();
    virtual ~InstructionListItem() {};

    bool operator==(const InstructionListItem & other) const;

    bool isConditionList;
    gd::InstructionsList* instructionList;
    gd::BaseEvent * event;
};

/**
 * \brief Used to indicate to EventsEditorItemsAreas that a parameter is displayed somewhere
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API ParameterItem : public EventsEditorItem
{
    ParameterItem(gd::Expression * parameter_, gd::BaseEvent * event);
    ParameterItem();
    virtual ~ParameterItem() {};

    bool operator==(const ParameterItem & other) const;

    gd::Expression * parameter;
    gd::BaseEvent * event;
};

/**
 * \brief Used to indicate to EventsEditorItemsAreas that a fold/unfold button is displayed somewhere
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API FoldingItem : public EventsEditorItem
{
    FoldingItem(gd::BaseEvent * event);
    FoldingItem();
    virtual ~FoldingItem() {};

    bool operator==(const FoldingItem & other) const;

    gd::BaseEvent * event;
};

/**
 * \brief Used to indicate to EventsEditorItemsAreas that a button to insert instructions is displayed somewhere
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API InstructionAdderItem : public EventsEditorItem
{
    InstructionAdderItem(bool isConditionList, gd::InstructionsList* instructionList_, gd::BaseEvent * event );
    InstructionAdderItem();
    virtual ~InstructionAdderItem() {};

    bool operator==(const InstructionAdderItem & other) const;

    bool isConditionList; ///< If the instructions list the button is related to is a conditions list.
    gd::InstructionsList* instructionList;
    gd::BaseEvent * event;
};

/**
 * \brief Tool class to store information about an event adder button.
 *
 * Used by events editor to indicate to EventsEditorItemsAreas that an event adder button is displayed somewhere.
 * \ingroup IDEDialogsEventsEditor
 */
struct GD_CORE_API EventAdderItem : public EventsEditorItem
{
    EventAdderItem(std::shared_ptr<gd::BaseEvent> event_, gd::EventsList * eventsList_, unsigned int positionInList_ );
    EventAdderItem();
    virtual ~EventAdderItem() {};

    bool operator==(const gd::EventAdderItem & other) const;

    std::shared_ptr<gd::BaseEvent> event;
    gd::EventsList * eventsList;
    unsigned int positionInList;
};

/**
 * \brief Allow events to indicate where is displayed an instruction or parameter.
 *
 * Events editor also uses this internally to indicate where events are displayed.
 * \ingroup IDEDialogsEventsEditor
 */
class GD_CORE_API EventsEditorItemsAreas
{
public:

    /**
     * \brief Declare an item in an area.
     */
    template<class T>
    void AddItem(wxRect area, T item)
    {
        areas[typeid(T)].push_back(
            std::make_pair(
                area,
                std::unique_ptr<EventsEditorItem>(new T(item))
                )
            );
    }

    template<class T>
    bool IsOnItemAt(int x, int y) const
    {
        if(areas.count(typeid(T)) == 0 || areas.at(typeid(T)).size() == 0)
            return false;

        return std::any_of(
            areas.at(typeid(T)).begin(),
            areas.at(typeid(T)).end(),
            [x, y](const std::pair<wxRect, std::unique_ptr<EventsEditorItem>> &pair){ return pair.first.Contains(x, y); }
            );
    }

    template<class T>
    T GetItemAt(int x, int y) const
    {
        return *dynamic_cast<T*>(std::find_if(
            areas.at(typeid(T)).begin(),
            areas.at(typeid(T)).end(),
            [x, y](const std::pair<wxRect, std::unique_ptr<EventsEditorItem>> &pair)
            {
                return pair.first.Contains(x, y);
            }
            )->second.get());
    }

    template<class T>
    wxRect GetAreaOfItem(T item)
    {
        return std::find_if(
            areas.at(typeid(T)).begin(),
            areas.at(typeid(T)).end(),
            [item](const std::pair<wxRect, std::unique_ptr<EventsEditorItem>> &pair)
            {
                return (*dynamic_cast<T*>(pair.second.get()) == item); //Convert the object back to his type and compare it
            }
        )->first;
    }

    template<class T>
    wxRect GetAreaOfItemAt(int x, int y)
    {
        return GetAreaOfItem<T>(GetItemAt<T>(x, y));
    }

    /**
     * \brief Clear all areas ( typically before redraw )
     */
    void Clear()
    {
        areas.clear();
    }

    /**
     * \brief Default constructor doing nothing.
     */
    EventsEditorItemsAreas() {};

    /**
     * \brief Default destructor doing nothing.
     */
    virtual ~EventsEditorItemsAreas() {};

    std::map<std::type_index, std::vector<std::pair<wxRect, std::unique_ptr<EventsEditorItem>>>> areas;

private:
};

}

//Hash for EventItem, ParameterItem, InstructionItem, InstructionListItem and FoldingItem
namespace std
{
    template<>
    struct hash<gd::EventItem>
    {
        std::size_t operator()(gd::EventItem const& item) const
        {
            return (std::hash<gd::BaseEvent*>()(item.event.get())) ^
                   (std::hash<gd::EventsList*>()(item.eventsList) << 1) ^
                   (std::hash<std::size_t>()(item.positionInList) << 2);
        }
    };

    template<>
    struct hash<gd::InstructionItem>
    {
        std::size_t operator()(gd::InstructionItem const& item) const
        {
            return (std::hash<gd::Instruction*>()(item.instruction)) ^
                   (std::hash<gd::InstructionsList*>()(item.instructionList) << 1) ^
                   (std::hash<std::size_t>()(item.positionInList) << 2) ^
                   (std::hash<gd::BaseEvent*>()(item.event) << 3) ^
                   (std::hash<bool>()(item.isCondition) << 4);
        }
    };

    template<>
    struct hash<gd::InstructionListItem>
    {
        std::size_t operator()(gd::InstructionListItem const& item) const
        {
            return (std::hash<gd::InstructionsList*>()(item.instructionList)) ^
                   (std::hash<gd::BaseEvent*>()(item.event) << 1) ^
                   (std::hash<bool>()(item.isConditionList) << 2);
        }
    };

    template<>
    struct hash<gd::ParameterItem>
    {
        std::size_t operator()(gd::ParameterItem const& item) const
        {
            return (std::hash<gd::Expression*>()(item.parameter)) ^
                   (std::hash<gd::BaseEvent*>()(item.event) << 1);
        }
    };

    template<>
    struct hash<gd::FoldingItem>
    {
        std::size_t operator()(gd::FoldingItem const& item) const
        {
            return std::hash<gd::BaseEvent*>()(item.event);
        }
    };
}

#endif // EVENTSEDITORITEMSAREAS_H
#endif
