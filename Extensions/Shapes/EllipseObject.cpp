/**

GDevelop - AdMob Object Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#include "EllipseObject.h"

#include <SFML/Graphics.hpp>
#include "GDCpp/Project/Object.h"
#include "GDCore/Tools/Localization.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/Serialization/SerializerElement.h"
#include "GDCpp/Project/InitialInstance.h"
#include "GDCpp/CommonTools.h"
#include "Tools/EllipseShape.h"
#include "Tools/ShapeTools.h"

#if defined(GD_IDE_ONLY)
#include "GDCore/IDE/Project/ArbitraryResourceWorker.h"
#include "GDCore/IDE/Dialogs/PropertyDescriptor.h"
#endif

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/bitmap.h>
#endif

sf::Texture EllipseObject::edittimeIconImage;
sf::Sprite EllipseObject::edittimeIcon;

EllipseObject::EllipseObject(gd::String name_) :
    gd::Object(name_),
    m_fillColor(255, 255, 255, 255),
    m_outlineColor(0, 0, 0, 255),
    m_outlineThickness(2)
{
}

#if defined(GD_IDE_ONLY)
std::map<gd::String, gd::PropertyDescriptor> EllipseObject::GetProperties(gd::Project & project) const
{
    std::map<gd::String, gd::PropertyDescriptor> properties;

    properties.emplace(_("Fill color"), gd::PropertyDescriptor(shapetools::ColorToString(m_fillColor)).SetType("Color"));
    properties.emplace(_("Fill color opacity"), gd::PropertyDescriptor(gd::String::From<unsigned int>(m_fillColor.a)));
    properties.emplace(_("Outline color"), gd::PropertyDescriptor(shapetools::ColorToString(m_outlineColor)).SetType("Color"));
    properties.emplace(_("Outline color opacity"), gd::PropertyDescriptor(gd::String::From<unsigned int>(m_outlineColor.a)));
    properties.emplace(_("Outline thickness"), gd::PropertyDescriptor(gd::String::From<int>(m_outlineThickness)));

    return properties;
}

bool EllipseObject::UpdateProperty(const gd::String & name, const gd::String & value, gd::Project & project)
{
    if(name == _("Fill color"))
    {
        try
        {
            unsigned int opacity = m_fillColor.a;
            m_fillColor = shapetools::ParseColor(value);
            m_fillColor.a = opacity;
        }
        catch(std::exception &e)
        {
            std::cout << "Can't set fill color: " << e.what() << std::endl;
        }
    }
    else if(name == _("Fill color opacity"))
    {
        m_fillColor.a = std::min(255u, std::max(0u, value.To<unsigned int>()));
    }
    else if(name == _("Outline color"))
    {
        try
        {
            unsigned int opacity = m_outlineColor.a;
            m_outlineColor = shapetools::ParseColor(value);
            m_outlineColor.a = opacity;
        }
        catch(std::exception &e)
        {
            std::cout << "Can't set the outline color: " << e.what() << std::endl;
        }
    }
    else if(name == _("Outline color opacity"))
    {
        m_outlineColor.a = std::min(255u, std::max(0u, value.To<unsigned int>()));
    }
    else if(name == _("Outline thickness"))
    {
        m_outlineThickness = value.To<int>();
    }

    return true;
}
#endif

void EllipseObject::DoUnserializeFrom(gd::Project & project, const gd::SerializerElement & element)
{

}

#if defined(GD_IDE_ONLY)
void EllipseObject::DoSerializeTo(gd::SerializerElement & element) const
{

}

sf::Vector2f EllipseObject::GetInitialInstanceDefaultSize(gd::InitialInstance & instance, gd::Project & project, gd::Layout & layout) const
{
    return sf::Vector2f(100.f, 100.f);
}

void EllipseObject::DrawInitialInstance(gd::InitialInstance & instance, sf::RenderTarget & renderTarget, gd::Project & project, gd::Layout & layout)
{
    //The instance's size
    sf::Vector2f realInstanceSize(
        instance.HasCustomSize() ? instance.GetCustomWidth() : 100,
        instance.HasCustomSize() ? instance.GetCustomHeight() : 100
    );

    //Automatically calculate the pointCount depending on the ellipse size
    unsigned int pointCount = std::min(30u * (unsigned int)(std::max(realInstanceSize.x, realInstanceSize.y)/200.f + 1.f), 2048u);

    EllipseShape shape(realInstanceSize.x, realInstanceSize.y, pointCount);
    shape.setFillColor(m_fillColor);
    shape.setOutlineColor(m_outlineColor);
    shape.setOutlineThickness(m_outlineThickness);

    shape.setOrigin(realInstanceSize/2.f);
    shape.rotate(instance.GetAngle());
    shape.move(instance.GetX() + realInstanceSize.x/2.f, instance.GetY() + realInstanceSize.y/2.f);

    renderTarget.draw(shape);
}

void EllipseObject::LoadEdittimeIcon()
{
    edittimeIconImage.loadFromFile("JsPlatform/Extensions/admobicon.png");
    edittimeIcon.setTexture(edittimeIconImage);
}

bool EllipseObject::GenerateThumbnail(const gd::Project & project, wxBitmap & thumbnail) const
{
#if !defined(GD_NO_WX_GUI)
    thumbnail = wxBitmap("JsPlatform/Extensions/admobicon24.png", wxBITMAP_TYPE_ANY);
#endif

    return true;
}
#endif

gd::Object * CreateEllipseObject(gd::String name)
{
    return new EllipseObject(name);
}
