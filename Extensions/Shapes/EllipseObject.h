/**

GDevelop - AdMob Object Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#ifndef ELLIPSISSHAPE_H
#define ELLIPSISSHAPE_H

#include <SFML/Graphics/Color.hpp>
#include "GDCore/IDE/Dialogs/PropertyDescriptor.h"
#include "GDCpp/Project/Object.h"
#include "GDCpp/String.h"

namespace gd { class InitialInstance; }
namespace gd { class Project; }
namespace sf { class Texture; }
namespace sf { class Sprite; }
class wxBitmap;

class GD_EXTENSION_API EllipseObject : public gd::Object
{
public:
    EllipseObject(gd::String name_);
    virtual ~EllipseObject() {};
    virtual gd::Object * Clone() const { return new EllipseObject(*this); }

    #if defined(GD_IDE_ONLY)
    sf::Vector2f GetInitialInstanceDefaultSize(gd::InitialInstance & instance, gd::Project & project, gd::Layout & layout) const;
    void DrawInitialInstance(gd::InitialInstance & instance, sf::RenderTarget & renderTarget, gd::Project & project, gd::Layout & layout) override;
    bool GenerateThumbnail(const gd::Project & project, wxBitmap & thumbnail) const override;
    static void LoadEdittimeIcon();

    std::map<gd::String, gd::PropertyDescriptor> GetProperties(gd::Project & project) const override;
    bool UpdateProperty(const gd::String & name, const gd::String & value, gd::Project & project) override;
    #endif

    sf::Color GetFillColor() const { return m_fillColor; };
    sf::Color GetOutlineColor() const { return m_outlineColor; };
    int GetOutlineThickness() const { return m_outlineThickness; };

private:
    void DoUnserializeFrom(gd::Project & project, const gd::SerializerElement & element) override;
    #if defined(GD_IDE_ONLY)
    void DoSerializeTo(gd::SerializerElement & element) const override;
    #endif

    sf::Color m_fillColor;
    sf::Color m_outlineColor;
    int m_outlineThickness;
};

gd::Object * CreateEllipseObject(gd::String name);

#endif // ELLIPSISSHAPE_H
