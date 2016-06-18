/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef GDCORE_REPEATEVENT_H
#define GDCORE_REPEATEVENT_H
#include "GDCore/Events/Event.h"
#include "GDCore/Events/EventsList.h"
namespace gd { class Instruction; }
namespace gd { class Project; }
class RuntimeScene;
class TiXmlElement;
namespace gd { class EventsEditorItemsAreas; }
namespace gd { class EventsEditorSelection; }

namespace gd
{

/**
 * \brief Event being repeated a specified number of times.
 */
class GD_CORE_API RepeatEvent : public gd::BaseEvent
{
public:
    RepeatEvent();
    virtual ~RepeatEvent() {};
    virtual gd::RepeatEvent * Clone() const { return new RepeatEvent(*this);}

    virtual bool IsExecutable() const {return true;}

    virtual bool CanHaveSubEvents() const {return true;}
    virtual const gd::EventsList & GetSubEvents() const {return events;};
    virtual gd::EventsList & GetSubEvents() {return events;};

    const gd::InstructionsList & GetConditions() const { return conditions; };
    gd::InstructionsList & GetConditions() { return conditions; };

    const gd::InstructionsList & GetActions() const { return actions; };
    gd::InstructionsList & GetActions() { return actions; };

    const gd::String & GetRepeatExpression() const { return repeatNumberExpression.GetPlainString(); };
    void SetRepeatExpression(gd::String repeatNumberExpression_) { repeatNumberExpression = gd::Expression(repeatNumberExpression_); };

    virtual std::vector < gd::InstructionsList* > GetAllConditionsVectors();
    virtual std::vector < gd::InstructionsList* > GetAllActionsVectors();
    virtual std::vector < gd::Expression* > GetAllExpressions();
    virtual std::vector < const gd::InstructionsList* > GetAllConditionsVectors() const;
    virtual std::vector < const gd::InstructionsList* > GetAllActionsVectors() const;
    virtual std::vector < const gd::Expression* > GetAllExpressions() const;

    virtual void SerializeTo(SerializerElement & element) const;
    virtual void UnserializeFrom(gd::Project & project, const SerializerElement & element);

    /**
     * Called by event editor to draw the event.
     */
    virtual void Render(wxDC & dc, int x, int y, unsigned int width, gd::EventsEditorItemsAreas & areas, gd::EventsEditorSelection & selection, const gd::Platform & platform);

    /**
     * Must return the height of the event when rendered
     */
    virtual unsigned int GetRenderedHeight(unsigned int width, const gd::Platform & platform) const;

    /**
     * Called when the user want to edit the event
     */
    virtual EditEventReturnType EditEvent(wxWindow* parent_, gd::Project & game_, gd::Layout & scene_, gd::MainFrameWrapper & mainFrameWrapper_);

private:
    gd::Expression repeatNumberExpression;
    gd::InstructionsList conditions;
    gd::InstructionsList actions;
    EventsList events;

    bool repeatNumberExpressionSelected;
};

}

#endif // GDCORE_REPEATEVENT_H
