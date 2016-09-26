/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#ifndef GDCORE_CLASSWITHOBJECTS_H
#define GDCORE_CLASSWITHOBJECTS_H
#include "GDCore/String.h"
#include <vector>
#include <memory>
namespace gd { class Object; }
namespace gd { class Project; }
namespace gd { class SerializerElement; }
#undef GetObject //Disable an annoying macro

namespace gd
{

/**
 * \brief Used as a base class for classes that will own objects (see gd::Object).
 *
 * For example, gd::Project inherits from this class as it have global objects.<br>
 * gd::Layout also inherits from this class as each layout has specific objects.
 *
 * \see gd::Project
 * \see gd::Layout
 * \see gd::Object
 *
 * \ingroup PlatformDefinition
 */
class GD_CORE_API ClassWithObjects
{
public:
    /**
     * \brief Default constructor creating a container without any objects.
     */
    ClassWithObjects();
    virtual ~ClassWithObjects();

    /** \name Objects management
     * Members functions related to objects management.
     */
    ///@{

    /**
     * \brief Return true if object called \a name exists.
     */
    bool HasObjectNamed(const gd::String & name) const;

    /**
     * \brief Return a reference to the object called \a name.
     */
    Object & GetObject(const gd::String & name);

    /**
     * \brief Return a reference to the object called \a name.
     */
    const gd::Object & GetObject(const gd::String & name) const;

    /**
     * \brief Return a reference to the object at position \a index in the objects list
     */
    Object & GetObject(std::size_t index);

    /**
     * \brief Return a reference to the object at position \a index in the objects list.
     */
    const gd::Object & GetObject (std::size_t index) const;

    /**
     * \brief Return the position of the object called \a name in the objects list.
     *
     * \warning This has nothing to do with an object position on a layout. Objects put on layouts
     * are represented thanks to the gd::InitialInstance class.
     */
    std::size_t GetObjectPosition(const gd::String & name) const;

    /**
     * \brief Return the number of object.
     */
    std::size_t GetObjectsCount() const;

#if defined(GD_IDE_ONLY)
    /**
     * \brief Add a new empty object of type \a objectType called \a name at the specified position in the list.<br>
     *
     * \note The object is created using the project's current platform.
     * \return A reference to the object in the list.
     */
    gd::Object & InsertNewObject(gd::Project & project, const gd::String & objectType, const gd::String & name, std::size_t position);
#endif

    /**
     * \brief Add a new object to the list
     * \note The object passed by parameter is copied.
     * \param object The object that must be copied and inserted into the project
     * \param position Insertion position. If the position is invalid, the object is inserted at the end of the objects list.
     * \return A reference to the object in the list.
     */
    gd::Object & InsertObject(const gd::Object & object, std::size_t position);

    /**
     * \brief Delete an object.
     * \param name The name of the object to be deleted.
     */
    void RemoveObject(const gd::String & name);

    /**
     * \brief Swap the position of the specified objects.
     */
    void SwapObjects(std::size_t firstObjectIndex, std::size_t secondObjectIndex);

    /**
     * Provide a raw access to the vector containing the objects
     */
    std::vector < std::unique_ptr<gd::Object> > & GetObjects() { return initialObjects; }

    /**
     * Provide a raw access to the vector containing the objects
     */
    const std::vector < std::unique_ptr<gd::Object> > & GetObjects() const  { return initialObjects; }
    ///@}

    /** \name Saving and loading
     * Members functions related to saving and loading the objects of the class.
     */
    ///@{
    /**
     * \brief Serialize instances container.
     */
    void SerializeObjectsTo(SerializerElement & element) const;

    /**
     * \brief Unserialize the instances container.
     */
    void UnserializeObjectsFrom(gd::Project & project, const SerializerElement & element);
    ///@}

protected:
    std::vector < std::unique_ptr<gd::Object> > initialObjects; ///< Objects contained.
};

}

#endif // GDCORE_CLASSWITHOBJECTS_H
