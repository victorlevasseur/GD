/**

GDevelop - Tile Map Extension
Copyright (c) 2014-2015 Victor Levasseur (victorlevasseur52@gmail.com)
This project is released under the MIT License.
*/

#include "RuntimeEllipseObject.h"

#include <SFML/Graphics.hpp>
#include "GDCore/Tools/Localization.h"
#include "GDCpp/Project/Object.h"
#include "GDCpp/Project/Project.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/RuntimeGame.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/FontManager.h"
#include "GDCpp/Project/InitialInstance.h"
#include "GDCpp/Polygon2d.h"
#include "GDCpp/PolygonCollision.h"
#include "GDCpp/RuntimeObjectsListsTools.h"
#include "GDCpp/Serialization/SerializerElement.h"
#include "GDCpp/CommonTools.h"

RuntimeEllipseObject::RuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object) :
    RuntimeObject(scene, object)
{

}

/**
 * Render object at runtime
 */
bool RuntimeEllipseObject::Draw( sf::RenderTarget& window )
{

    return true;
}


float RuntimeEllipseObject::GetWidth() const
{
    return 100.f;
}

float RuntimeEllipseObject::GetHeight() const
{
    return 100.f;
}

void RuntimeEllipseObject::OnPositionChanged()
{

}

#ifdef GD_IDE_ONLY
void RuntimeEllipseObject::GetPropertyForDebugger(std::size_t propertyNb, gd::String & name, gd::String & value) const
{

}

bool RuntimeEllipseObject::ChangeProperty(std::size_t propertyNb, gd::String newValue)
{
    return true;
}

std::size_t RuntimeEllipseObject::GetNumberOfProperties() const
{
    return 0;
}
#endif

RuntimeObject * CreateRuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object)
{
    return new RuntimeEllipseObject(scene, object);
}
