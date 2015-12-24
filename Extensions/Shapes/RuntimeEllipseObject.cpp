/**

GDevelop - Tile Map Extension
Copyright (c) 2014-2015 Victor Levasseur (victorlevasseur52@gmail.com)
This project is released under the MIT License.
*/

#include "RuntimeEllipseObject.h"

#include <cmath>
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
#include "EllipseObject.h"
#include "Tools/ShapeTools.h"

RuntimeEllipseObject::RuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object) :
    RuntimeObject(scene, object),
    m_ellipse(100.f, 100.f),
    m_hitbox()
{
    const EllipseObject & ellipseObject = dynamic_cast<const EllipseObject &>(object);

    m_ellipse.setOrigin(50.f, 50.f);
    m_ellipse.setFillColor(ellipseObject.GetFillColor());
    m_ellipse.setOutlineColor(ellipseObject.GetOutlineColor());
    m_ellipse.setOutlineThickness(ellipseObject.GetOutlineThickness());

    UpdatePointCount();
}

/**
 * Render object at runtime
 */
bool RuntimeEllipseObject::Draw( sf::RenderTarget& window )
{
    if(hidden)
        return true;

    window.draw(m_ellipse);

    return true;
}


float RuntimeEllipseObject::GetWidth() const
{
    return m_ellipse.GetWidth();
}

float RuntimeEllipseObject::GetHeight() const
{
    return m_ellipse.GetHeight();
}

void RuntimeEllipseObject::SetWidth(float newWidth)
{
    m_ellipse.SetWidth(newWidth);
    OnSizeChanged();
}

void RuntimeEllipseObject::SetHeight(float newHeight)
{
    m_ellipse.SetHeight(newHeight);
    OnSizeChanged();
}

float RuntimeEllipseObject::GetAngle() const
{
    return m_ellipse.getRotation();
}

bool RuntimeEllipseObject::SetAngle(float angle)
{
    m_ellipse.setRotation(angle);
    return true;
}

void RuntimeEllipseObject::OnPositionChanged()
{
    m_ellipse.setPosition(GetX() + GetWidth()/2.f, GetY() + GetHeight()/2.f);
}

void RuntimeEllipseObject::OnSizeChanged()
{
    UpdatePointCount();
    OnPositionChanged();
}

#ifdef GD_IDE_ONLY
void RuntimeEllipseObject::GetPropertyForDebugger(std::size_t propertyNb, gd::String & name, gd::String & value) const
{
    if(propertyNb == 0)
    {
        name = _("Fill color");
        value = shapetools::ColorToString(m_ellipse.getFillColor());
    }
    else if(propertyNb == 1)
    {
        name = _("Outline color");
        value = shapetools::ColorToString(m_ellipse.getOutlineColor());
    }
    else if(propertyNb == 2)
    {
        name = _("Outline thickness");
        value = gd::String::From<float>(m_ellipse.getOutlineThickness());
    }
}

bool RuntimeEllipseObject::ChangeProperty(std::size_t propertyNb, gd::String newValue)
{
    try
    {
        if(propertyNb == 0)
        {
            m_ellipse.setFillColor(shapetools::ParseColor(newValue));
        }
        else if(propertyNb == 1)
        {
            m_ellipse.setOutlineColor(shapetools::ParseColor(newValue));
        }
        else if(propertyNb == 2)
        {
            m_ellipse.setOutlineThickness(newValue.To<float>());
        }
    }
    catch(std::domain_error &e)
    {
        return false;
    }

    return true;
}

std::size_t RuntimeEllipseObject::GetNumberOfProperties() const
{
    return 3;
}
#endif

std::vector<Polygon2d> RuntimeEllipseObject::GetHitBoxes() const
{
    Polygon2d movedHitbox(m_hitbox);
    movedHitbox.Rotate((GetAngle()/180.f)*M_PI);
    movedHitbox.Move(GetX() + GetWidth()/2.f, GetY() + GetHeight()/2.f);
    movedHitbox.ComputeEdges();

    return std::vector<Polygon2d>{movedHitbox};
}

void RuntimeEllipseObject::UpdatePointCount()
{
    unsigned int pointCount = std::min(30u * (unsigned int)(std::max(GetWidth(), GetHeight())/200.f + 1.f), 2048u);
    m_ellipse.setPointCount(pointCount);
    m_ellipse.setOrigin(GetWidth()/2.f, GetHeight()/2.f);

    m_hitbox.vertices.clear();
    for(unsigned int i = 0; i < m_ellipse.getPointCount(); i++)
    {
        m_hitbox.vertices.push_back(sf::Vector2f(m_ellipse.getPoint(i).x - GetWidth()/2.f, m_ellipse.getPoint(i).y - GetHeight()/2.f));
    }
}

RuntimeObject * CreateRuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object)
{
    return new RuntimeEllipseObject(scene, object);
}
