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

std::size_t ObjectsFolderHelper::GetObjectsCount(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, bool subFolders)
{
    return std::count_if(
        objectsContainer.cbegin(),
        objectsContainer.cend(),
        [&folder, subFolders](const std::unique_ptr<gd::Object> & object) {
            return !subFolders && object->GetFolder() == folder || subFolders && object->GetFolder().substr( 0, folder.size() ) == folder;
        } );
}

const gd::Object & ObjectsFolderHelper::GetObjectAt(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t position, bool subFolders)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( !subFolders && objectsContainer.GetObject(i).GetFolder() == folder || subFolders && objectsContainer.GetObject(i).GetFolder().substr( 0, folder.size() ) == folder )
        {
            if( position > 0 )
                --position;
            else
                return objectsContainer.GetObject(i);
        }
    }

    throw std::out_of_range("");
}

gd::Object & ObjectsFolderHelper::GetObjectAt(const gd::String & folder, gd::ClassWithObjects & objectsContainer, std::size_t position, bool subFolders)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( !subFolders && objectsContainer.GetObject(i).GetFolder() == folder || subFolders && objectsContainer.GetObject(i).GetFolder().substr( 0, folder.size() ) == folder )
        {
            if( position > 0 )
                --position;
            else
                return objectsContainer.GetObject(i);
        }
    }

    throw std::out_of_range("");
}

std::size_t ObjectsFolderHelper::GetObjectPositionInFolder(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, const gd::String & objectName, bool subFolders)
{
    std::size_t absolutePosition = objectsContainer.GetObjectPosition( objectName );

    if( absolutePosition == gd::String::npos || objectsContainer.GetObject( absolutePosition ).GetFolder() != folder )
        return gd::String::npos;

    return std::count_if( // Count the objects of the same folder before it
        objectsContainer.cbegin(),
        objectsContainer.cbegin() + absolutePosition,
        [&folder, &subFolders](const std::unique_ptr<gd::Object> & object) { return !subFolders && object->GetFolder() == folder || subFolders && object->GetFolder().substr( 0, folder.size() ) == folder; } );
}

std::size_t ObjectsFolderHelper::GetObjectAbsolutePosition(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder, bool subFolders)
{
    for( std::size_t i = 0; i < objectsContainer.GetObjectsCount(); ++i )
    {
        if( !subFolders && objectsContainer.GetObject(i).GetFolder() == folder || subFolders && objectsContainer.GetObject(i).GetFolder().substr( 0, folder.size() ) == folder )
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

bool ObjectsFolderHelper::HasFolder( const gd::ClassWithObjects & objectsContainer, const gd::String & folder )
{
    if( folder == "" )
        return true;

    return std::any_of(
        objectsContainer.cbegin(),
        objectsContainer.cend(),
        [&folder]( const std::unique_ptr<gd::Object> & object ) { return object->GetFolder().substr( 0, folder.size() ) == folder; } );
}

bool ObjectsFolderHelper::IsSubFolder( const gd::String & folder, const gd::String & parentFolder, bool subFolders )
{
    return folder != parentFolder &&
        folder.substr( 0, parentFolder.size() ) == parentFolder &&
        ( subFolders || folder.substr( parentFolder.size() + 1 ).find( "/" ) == gd::String::npos );
}

bool ObjectsFolderHelper::MoveFolder( gd::ClassWithObjects & objectsContainer, const gd::String & folder, const gd::String & newFolder, const gd::String & beforeSubFolder )
{
    if( !HasFolder( objectsContainer, folder ) ||
        newFolder != folder && HasFolder( objectsContainer, newFolder ) ||
        IsSubFolder( newFolder, folder ) ||
        beforeSubFolder == folder )
        return false;

    std::size_t lastSepPos = newFolder.rfind("/");
    gd::String newParentFolder = newFolder.substr(0, lastSepPos);

    if( beforeSubFolder != "" && !IsSubFolder( beforeSubFolder, newParentFolder, false ) )
        return false;

    // Remove the objects that are about to be moved (done before so that they can be the subfolders listed afterwards)
    std::vector<std::unique_ptr<gd::Object>> objectsToMove;
    while( GetObjectsCount( folder, objectsContainer, true ) > 0 )
    {
        objectsToMove.push_back( std::move( objectsContainer.RemoveObject( GetObjectAt( folder, objectsContainer, 0, true ).GetName() ) ) );
        // Note: they are pushed in reverse order!
    }

    std::size_t insertionPosition =
        beforeSubFolder != "" ?
            GetFirstObjectInFolderAbsolutePosition( objectsContainer, beforeSubFolder, true ) + 1 /* to insert just after it */ :
            gd::String::npos;

    // Insert them back and set their folder
    for( auto & object : objectsToMove )
    {
        gd::String newFolderFull = newFolder;
        if( IsSubFolder( object->GetFolder(), folder, true ) )
            newFolderFull += object->GetFolder().substr( folder.size() );

        object->SetFolder( newFolderFull );
        objectsContainer.InsertObject( std::move( object ), insertionPosition ); // Invert again their order to get the new order
    }

    return true;
}

std::size_t ObjectsFolderHelper::GetFirstObjectInFolderAbsolutePosition( const gd::ClassWithObjects & objectsContainer, const gd::String & folder, bool subFolders )
{
    auto foundIt = objectsContainer.cend();

    if( subFolders )
    {
        foundIt = std::find_if(
            objectsContainer.cbegin(),
            objectsContainer.cend(),
            [&folder]( const std::unique_ptr<gd::Object> & object )
            {
                return object->GetFolder().substr( 0, folder.size() ) == folder;
            });
    }
    else
    {
        foundIt = std::find_if(
            objectsContainer.cbegin(),
            objectsContainer.cend(),
            [&folder]( const std::unique_ptr<gd::Object> & object )
            {
                return object->GetFolder() == folder;
            });
    }

    if( foundIt == objectsContainer.cend() )
        return gd::String::npos;

    return std::distance( objectsContainer.cbegin(), foundIt );
}

std::vector<gd::String> ObjectsFolderHelper::GetSubFolders( const gd::ClassWithObjects & objectsContainer, const gd::String & folder )
{
    std::vector<gd::String> subFolders; // std::set would be more efficient, but we need to preserve the order of the subfolders

    for( const auto & object : objectsContainer )
    {
        if( object->GetFolder().substr( 0, folder.size() ) == folder )
        {
            std::size_t sepPos = object->GetFolder().find( "/", folder.size() + 1 /* To avoid the "/" after the parent folder */ );
            gd::String subFolder = object->GetFolder().substr( folder.size(), sepPos - folder.size() );

            if( std::find( subFolders.cbegin(), subFolders.cend(), subFolder ) == subFolders.cend() )
                subFolders.push_back( object->GetFolder() );
        }
    }

    return subFolders;
}

}
