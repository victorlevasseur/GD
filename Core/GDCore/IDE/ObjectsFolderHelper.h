#ifndef GDCORE_OBJECTSFOLDERHELPER_H
#define GDCORE_OBJECTSFOLDERHELPER_H

#include "GDCore/String.h"

namespace gd { class ClassWithObjects; }
namespace gd { class Layout; }
namespace gd { class Object; }
namespace gd { class Project; }

namespace gd
{

class ObjectsFolderHelper
{
public:
    static std::size_t GetObjectsCount(const gd::String & folder, const gd::ClassWithObjects & objectsContainer);
    static std::size_t GetObjectsCount(const gd::String & folder, const gd::Layout & layout, const gd::Project & project);

    static const gd::Object & GetObjectAt(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder);
    static gd::Object & GetObjectAt(const gd::String & folder, gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder);

    static std::size_t GetObjectPositionInFolder(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, const gd::String & objectName);

    static std::size_t GetObjectAbsolutePosition(const gd::String & folder, const gd::ClassWithObjects & objectsContainer, std::size_t positionInFolder);

    static void ChangeObjectFolder(gd::ClassWithObjects & objectsContainer, const gd::String & objectName, const gd::String & newFolder, std::size_t newPositionInFolder = gd::String::npos);
};

}

#endif
