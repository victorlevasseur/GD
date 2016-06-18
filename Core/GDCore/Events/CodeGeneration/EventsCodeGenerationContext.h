/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#ifndef EVENTSCODEGENERATIONCONTEXT_H
#define EVENTSCODEGENERATIONCONTEXT_H

#include "GDCore/String.h"
#include <set>
#include <map>
#include <memory>
namespace gd { class Layout; }

namespace gd
{

/**
 * \brief Used to manage the context when generating code for events.
 *
 * The context refers to :
 * - The objects lists being available.
 * - The "current object", i.e the object being used by an action or a condition.
 * - If conditions are being generated, the context keeps track of the depth of the conditions ( see GetCurrentConditionDepth )
 * - You can also get the context depth of the last use of an object list.
 */
class GD_CORE_API EventsCodeGenerationContext
{
    friend class EventsCodeGenerator;
public:
    /**
     * Default constructor. You may want to call InheritsFrom just after.
     * \param maxDepthLevel Optional pointer to an unsigned integer that will be updated to contain the maximal scope depth reached.
     */
    EventsCodeGenerationContext(unsigned int * maxDepthLevel_ = NULL) : contextDepth(0), customConditionDepth(0), maxDepthLevel(maxDepthLevel_), parent(NULL) {};
    virtual ~EventsCodeGenerationContext() {};

    /**
     * Call this method to make an EventsCodeGenerationContext as a "child" of another one.
     * The child will then for example not declare again objects already declared by its parent.
     */
    void InheritsFrom(const EventsCodeGenerationContext & parent);

    /**
     * \brief Returns the depth of the inheritance of the context.
     *
     * A context created from scratch will returns 0, and a context inheriting from a context with depth n will returns n+1.
     */
    size_t GetContextDepth() const { return contextDepth; }

    /**
     * \brief Get the parent context, if any.
     * \return A pointer to the parent context, or NULL if the context has no parent.
     */
    const EventsCodeGenerationContext * GetParentContext() const { return parent; }

    /**
     * Mark the object has being the object being handled by the instruction
     */
    void SetCurrentObject(const gd::String & objectName) { currentObject = objectName; };

    /**
     * Set that no particular object is being handled by an instruction
     */
    void SetNoCurrentObject() { currentObject = ""; };

    /**
     * Get the object being handled by the instruction
     */
    const gd::String & GetCurrentObject() const { return currentObject; };

    /**
     * \brief Call this when an instruction in the event need an object list.
     *
     * The list will be filled with objects from the scene if it is the first time it is requested, unless there is
     * already an object list with this name ( i.e. ObjectAlreadyDeclared(objectName) returns true ).
     */
    void ObjectsListNeeded(const gd::String & objectName);

    /**
     * Call this when an instruction in the event need an object list.
     * An empty event list will be declared, without filling it with objects from the scene. If there is already an object
     * list with this name, no new list will be declared again.
     */
    void EmptyObjectsListNeeded(const gd::String & objectName);

    /**
     * Return true if an object list has already been declared (or is going to be declared).
     */
    bool ObjectAlreadyDeclared(const gd::String & objectName) const { return (alreadyDeclaredObjectsLists.find(objectName) != alreadyDeclaredObjectsLists.end()); };

    /**
     * \brief Consider that \a objectName is now declared in the context.
     */
    void SetObjectDeclared(const gd::String & objectName ) { alreadyDeclaredObjectsLists.insert(objectName); }

    /**
     * Return all the objects lists which will be declared by the current context
     * ( the non empty as well as the empty objects lists )
     */
    std::set<gd::String> GetAllObjectsToBeDeclared() const;

    /**
     * Return the objects lists which will be declared by the current context
     */
    const std::set<gd::String> & GetObjectsListsToBeDeclared() const { return objectsListsToBeDeclared; };

    /**
     * Return the objects lists which will be declared, but no filled, by the current context
     */
    const std::set<gd::String> & GetObjectsListsToBeDeclaredEmpty() const { return emptyObjectsListsToBeDeclared; };

    /**
     * Return the objects lists which are already declared and can be used in the current context without declaration.
     */
    const std::set<gd::String> & GetObjectsListsAlreadyDeclared() const { return alreadyDeclaredObjectsLists; };

    /**
     * \brief Get the depth of the context that was in effect when \a objectName was needed.
     *
     * If \a objectName is needed in this context, it will return the depth of this context.
     */
    unsigned int GetLastDepthObjectListWasNeeded(const gd::String & objectName) const;

    /**
     * \brief Called when a custom condition code is generated.
     */
    void EnterCustomCondition() { customConditionDepth++; };

    /**
     * \brief Called when a custom condition code generation is over.
     */
    void LeaveCustomCondition() { customConditionDepth--; };

    /**
     * \brief Get the current condition depth : The depth is increased each time a custom condition code is generated,
     * and decreased when the condition generation is done.
     *
     * This can be useful to generate sub conditions booleans with a different name than the parent's conditions.
     */
    size_t GetCurrentConditionDepth() const { return customConditionDepth; }

private:
    std::set<gd::String> alreadyDeclaredObjectsLists; ///< Objects lists already needed in a parent context.
    std::set<gd::String> objectsListsToBeDeclared; ///< Objects lists that will be declared in this context.
    std::set<gd::String> emptyObjectsListsToBeDeclared; ///< Objects lists that will be declared in this context, but not filled with scene's objects.
    std::map<gd::String, unsigned int> depthOfLastUse; ///< The context depth when an object was last used.
    gd::String currentObject; ///< The object being used by an action or condition.
    unsigned int contextDepth; ///< The depth of the context : 0 for a newly created context, n+1 for any context inheriting from context with depth n.
    unsigned int customConditionDepth; ///< The depth of the conditions being generated.
    unsigned int * maxDepthLevel; ///< A pointer to a unsigned int updated with the maximum depth reached.
    const EventsCodeGenerationContext * parent; ///< The parent of the current context. Can be NULL.
};

}
#endif // EVENTSCODEGENERATIONCONTEXT_H
