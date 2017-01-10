/*
 * GDevelop Core
 * Copyright 2016 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#include "ObjectsFolderHelper.h"

#include <algorithm>
#include <stdexcept>

#include "GDCore/Project/ClassWithObjects.h"
#include "GDCore/Project/Layout.h"
#include "GDCore/Project/Object.h"
#include "GDCore/Project/Project.h"

namespace gd
{

std::size_t ObjectsFolderHelper::GetObjectsCount(const gd::String & folder, const gd::ClassWithObjects & objectsContainer)
{
    return std::count_if(
        objectsContainer.cbegin(),
        objectsContainer.cend(),
        [&folder](const std::unique_ptr<gd::Object> & object) { return object->GetFolder() == folder; } );
}

std::size_t ObjectsFolderHelper::GetObjectsCount(const gd::String & folder, const gd::Layout & layout, const gd::Project & project)
{
    return GetObjectsCount(folder, layout) + GetObjectsCount(folder, project);
}

const gd::Object & ObjectsFolderHelper::GetObjectAt(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t position)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( objectsContainer.GetObject(i).GetFolder() == folder )
        {
            if( position > 0 )
                --position;
            else
                return objectsContainer.GetObject(i);
        }
    }

    throw std::out_of_range("");
}

gd::Object & ObjectsFolderHelper::GetObjectAt(const gd::String & folder, gd::ClassWithObjects & objectsContainer, std::size_t position)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( objectsContainer.GetObject(i).GetFolder() == folder )
        {
            if( position > 0 )
                --position;
            else
                return objectsContainer.GetObject(i);
        }
    }

    throw std::out_of_range("");
}

std::size_t ObjectsFolderHelper::GetObjectPositionInFolder(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, const gd::String & objectName)
{
    std::size_t absolutePosition = objectsContainer.GetObjectPosition( objectName );

    if( absolutePosition == gd::String::npos || objectsContainer.GetObject( absolutePosition ).GetFolder() != folder )
        return gd::String::npos;

    return std::count_if( // Count the objects of the same folder before it
        objectsContainer.cbegin(),
        objectsContainer.cbegin() + absolutePosition,
        [&folder](const std::unique_ptr<gd::Object> & object) { return object->GetFolder() == folder; } );
}

std::size_t ObjectsFolderHelper::GetObjectAbsolutePosition(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( objectsContainer.GetObject(i).GetFolder() == folder )
        {
            if( positionInFolder > 0 )
                --positionInFolder;
            else
                return i;
        }
    }

    return gd::String::npos;
}

void ObjectsFolderHelper::ChangeObjectFolder(gd::ClassWithObjects & objectsContainer, const gd::String & objectName, const gd::String & newFolder, std::size_t newPositionInFolder)
{
    if( !objectsContainer.HasObjectNamed( objectName ) )
        return;

    // First step:
    // If the object's old folder contain another object after it,
    // swap it to keep the old folder positioning.
    const gd::String oldFolder = objectsContainer.GetObject( objectName ).GetFolder();
    std::size_t objectPositionInFolder = GetObjectPositionInFolder( oldFolder, objectsContainer, objectName );

    if( objectPositionInFolder < GetObjectsCount( oldFolder, objectsContainer ) - 1 ) // If it contains an object after it
    {
        objectsContainer.SwapObjects(
            GetObjectAbsolutePosition( oldFolder, objectsContainer, objectPositionInFolder ),
            GetObjectAbsolutePosition( oldFolder, objectsContainer, objectPositionInFolder + 1 ) );
    }

    // Second step:
    // Remove the object from the list
    std::unique_ptr<gd::Object> object = objectsContainer.RemoveObject( objectName );
    object->SetFolder( newFolder );

    // Third step:
    // Insert it as "newPositionInFolder" relative to the newFolder
    objectsContainer.InsertObject( std::move(object), GetObjectAbsolutePosition( newFolder, objectsContainer, newPositionInFolder ) );
}

}
