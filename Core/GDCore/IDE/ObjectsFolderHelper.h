/*
 * GDevelop Core
 * Copyright 2016 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#ifndef GDCORE_OBJECTSFOLDERHELPER_H
#define GDCORE_OBJECTSFOLDERHELPER_H

#include <vector>

#include "GDCore/String.h"

namespace gd { class ClassWithObjects; }
namespace gd { class Layout; }
namespace gd { class Object; }
namespace gd { class Project; }

namespace gd
{

/**
 * \brief This class contains functions to ease the manipulation of objects folders.
 *
 * To not disturb the game engine with extra stuff to manage, objects folders are only
 * managed and taken into account by the editors (objects editor and objects choosers).
 * There's no real hierarchy stored in gd::ClassWithObjects to know which object is in
 * which folder and what are their order (of the folders and of the objects in the
 * folders themselves). The folder is only specified in the folder field of gd::Object
 * (see gd::Object::GetFolder() and gd::Object::SetFolder()). **The order of the folders
 * is basically the same as the order of the first objects of every folders.**
 *
 * So, manipulating folder requires extra steps to feel user-friendly in the editors
 * For example, just changing the folder of an object can alter the order of its old and
 * new folder in a undetermined way. This class provides functions to overcome these
 * problematics, for example, ChangeObjectFolder() can change the position/index and the
 * folder of an object without causing strange behaviors.
 */
class GD_CORE_API ObjectsFolderHelper
{
public:
    /**
     * Get the number of objects in a folder.
     *
     * \param folder the folder
     * \param objectsContainer the container holding all the objects
     * \param subFolders true to take objects in subfolders of **folder** into account
     *
     * \return the number of objects in the specified folder
     */
    static std::size_t GetObjectsCount(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, bool subFolders = false);

    /**
     * Get an object according to its position in a folder.
     *
     * \param folder the folder where the object is
     * \param objectsContainer the container holding the objects
     * \param positionInFolder the index of the object in the folder
     * \param subFolders true to take objects in subfolders of **folder** into account
     *
     * \return the specified object
     */
    static const gd::Object & GetObjectAt(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder, bool subFolders = false);

    /**
     * Get an object according to its position in a folder.
     *
     * \param folder the folder where the object is
     * \param objectsContainer the container holding the objects
     * \param positionInFolder the index of the object in the folder
     * \param subFolders true to take objects in subfolders of **folder** into account
     *
     * \return the specified object
     */
    static gd::Object & GetObjectAt(const gd::String & folder, gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder, bool subFolders = false);

    /**
     * Get an object index relative to its folder.
     *
     * \param folder the folder where the object is
     * \param objectsContainer the container holding the objects
     * \param objectName the object's name
     * \param subFolders true to take objects in subfolders of **folder** into account
     *
     * \return the index (position) of the object in the folder, npos if not in the folder
     */
    static std::size_t GetObjectPositionInFolder(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, const gd::String & objectName, bool subFolders = false);

    /**
     * Get an object absolute index (index in its container).
     *
     * \param folder the folder where the object is
     * \param objectsContainer the container holding the objects
     * \param positionInFolder the relative position (index) of the object in its folder
     * \param subFolders true to take objects in subfolders of **folder** into account
     *
     * \return the absolute index of the object in the container
     */
    static std::size_t GetObjectAbsolutePosition(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder, bool subFolders = false);

    /**
     * Move an object at a relative position in a specified folder.
     *
     * This function makes sure that the order of the objects and folders is kept
     * in the object editors and objects list displays.
     *
     * \param objectsContainer the container holding the objects
     * \param objectName the object's name
     * \param newFolder the folder where the object must be moved in
     * \param newPositionInFolder the relative position where the object
     * will be inserted at
     *
     * \note the new folder can be the same as the old one. The object will
     * be just moved in that folder.
     */
    static void ChangeObjectFolder(gd::ClassWithObjects & objectsContainer, const gd::String & objectName, const gd::String & newFolder, std::size_t newPositionInFolder = gd::String::npos);

    /**
     * Delete an object.
     */
    static void RemoveObject(gd::ClassWithObjects & objectsContainer, const gd::String & objectName);

    /**
     * Checks if a folder exists.
     *
     * This function checks if at least one object in inside a folder or one
     * of its subfolders.
     *
     * \param objectsContainer the container holding the objects
     * \param folder the folder name
     *
     * \return true if the folder exists (at least one object in it or one
     * of its subfolders).
     */
    static bool HasFolder(const gd::ClassWithObjects & objectsContainer, const gd::String & folder);

    static gd::String GetParentFolder(const gd::String & folder);

    static std::vector<gd::String> GetSubFolders(const gd::ClassWithObjects & objectsContainer, const gd::String & folder);

    static bool IsSubFolder(const gd::String & folder, const gd::String & parentFolder, bool subFolders = true);

    static bool MoveFolder(gd::ClassWithObjects & objectsContainer, const gd::String & folder, const gd::String & newFolder, const gd::String & beforeSubFolder = gd::String());

    static void OffsetFolder(gd::ClassWithObjects & objectsContainer, const gd::String & folder, std::size_t offset);

    static bool RenameFolder(gd::ClassWithObjects & objectsContainer, const gd::String & folder, const gd::String & newName);

    static void RemoveFolder(gd::ClassWithObjects & objectsContainer, const gd::String & folder, bool deleteObjects);
private:
    static std::size_t GetFirstObjectInFolderAbsolutePosition(const gd::ClassWithObjects & objectsContainer, const gd::String & folder, bool subFolders = false);
};

}

#endif
