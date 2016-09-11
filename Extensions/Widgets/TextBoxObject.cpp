/**

GDevelop - Text Object Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/bitmap.h>
#include <wx/log.h>
#endif
#include <SFML/Graphics.hpp>
#include "GDCpp/Runtime/Project/Object.h"
#include "GDCore/Tools/Localization.h"
#include "GDCpp/Runtime/ImageManager.h"
#include "GDCpp/Runtime/Serialization/SerializerElement.h"
#include "GDCpp/Runtime/FontManager.h"
#include "GDCpp/Runtime/Project/InitialInstance.h"
#include "GDCpp/Runtime/Polygon2d.h"
#include "GDCpp/Runtime/CommonTools.h"
#include "GDCpp/Runtime/RuntimeLayer.h"
#include "GDCpp/Runtime/RuntimeScene.h"
#include "GDCpp/Runtime/Serialization/SerializerElement.h"
#include "TextBoxObject.h"
#include "GDResourcesGetter.h"
#include "simplgui/TextBox.h"

#if defined(GD_IDE_ONLY)
#include "GDCore/IDE/AbstractFileSystem.h"
#include "GDCore/IDE/Project/ArbitraryResourceWorker.h"
namespace gd { class MainFrameWrapper; }
#endif

using namespace std;

TextBoxObject::TextBoxObject(gd::String name_) :
    Object(name_),
    text("Text"),
    characterSize(20),
    fontName(""),
    smoothed(true),
    bold(false),
    italic(false),
    underlined(false),
    colorR( 255 ),
    colorG( 255 ),
    colorB( 255 )
    #if defined(GD_IDE_ONLY)
    ,m_textbox(nullptr)
    ,font(NULL)
    #endif
{
    InitWidget();
}

TextBoxObject::TextBoxObject(const TextBoxObject &other) : gd::Object(other)
{
    InitWidget();
}

TextBoxObject::~TextBoxObject()
{

}

TextBoxObject& TextBoxObject::operator=(const TextBoxObject &other)
{
    gd::Object::operator=(other);
    InitWidget();

    return *this;
}

void TextBoxObject::InitWidget()
{
    #if defined(GD_IDE_ONLY)
    m_textbox = simplgui::TextBox::create(GDResourcesGetter::create());
    m_textbox->setMinSize(sf::Vector2f(150.f, 0.f));
    m_textbox->setText(U"Text");
    #endif
}

void TextBoxObject::DoUnserializeFrom(gd::Project & project, const gd::SerializerElement & element)
{
    SetString(element.GetChild("string", 0,"String").GetValue().GetString());
    SetFontFilename(element.GetChild("font", 0,"Font").GetValue().GetString());
    SetCharacterSize(element.GetChild("characterSize", 0, "CharacterSize").GetValue().GetInt());
    SetColor(element.GetChild("color", 0,"Color").GetIntAttribute("r", 255),
        element.GetChild("color", 0,"Color").GetIntAttribute("g", 255),
        element.GetChild("color", 0,"Color").GetIntAttribute("b", 255));


    smoothed = element.GetBoolAttribute("smoothed");
    bold = element.GetBoolAttribute("bold");
    italic = element.GetBoolAttribute("italic");
    underlined = element.GetBoolAttribute("underlined");
}

#if defined(GD_IDE_ONLY)
void TextBoxObject::DrawInitialInstance(gd::InitialInstance & instance, sf::RenderTarget & renderTarget, gd::Project & project, gd::Layout & layout)
{
    /*sf::Text sfText;
    sfText.setString(text);
    sfText.setCharacterSize(characterSize);
    sfText.setStyle((bold ? sf::Text::Bold : 0) |
                 (IsItalic() ? sf::Text::Italic : 0) |
                 (IsUnderlined() ? sf::Text::Underlined : 0) );
    if ( font ) sfText.setFont(*font);
    else sfText.setFont(*FontManager::Get()->GetFont(""));
    sfText.setOrigin(sfText.getLocalBounds().width/2, sfText.getLocalBounds().height/2);
    sfText.setPosition( instance.GetX()+sfText.getOrigin().x, instance.GetY()+sfText.getOrigin().y );
    sfText.setRotation( instance.GetAngle() );
    sfText.setColor(sf::Color(colorR, colorG, colorB));

    renderTarget.draw(sfText);*/

    m_textbox->setPosition(instance.GetX(), instance.GetY());
    //TODO:Size
    renderTarget.draw(*m_textbox);
}

sf::Vector2f TextBoxObject::GetInitialInstanceDefaultSize(gd::InitialInstance & instance, gd::Project & project, gd::Layout & layout) const
{
    /*sf::Text sfText;
    sfText.setString(text);
    sfText.setCharacterSize(characterSize);
    sfText.setStyle((bold ? sf::Text::Bold : 0) |
                 (IsItalic() ? sf::Text::Italic : 0) |
                 (IsUnderlined() ? sf::Text::Underlined : 0) );
    if ( font ) sfText.setFont(*font);
    else sfText.setFont(*FontManager::Get()->GetFont(""));

    return sf::Vector2f(sfText.getLocalBounds().width, sfText.getLocalBounds().height+ sfText.getLocalBounds().top);*/
    return m_textbox->getEffectiveSize();
}

void TextBoxObject::LoadResources(gd::Project & project, gd::Layout & layout)
{
    font = FontManager::Get()->GetFont(fontName);
}

void TextBoxObject::DoSerializeTo(gd::SerializerElement & element) const
{
    element.AddChild("string").SetValue(GetString());
    element.AddChild("font").SetValue(GetFontFilename());
    element.AddChild("characterSize").SetValue(GetCharacterSize());
    element.AddChild("color").SetAttribute("r", (int)GetColorR())
        .SetAttribute("g", (int)GetColorG())
        .SetAttribute("b", (int)GetColorB());

    element.SetAttribute("smoothed", smoothed);
    element.SetAttribute("bold", bold);
    element.SetAttribute("italic", italic);
    element.SetAttribute("underlined", underlined);
}

void TextBoxObject::ExposeResources(gd::ArbitraryResourceWorker & worker)
{
    worker.ExposeFile(fontName);
}

bool TextBoxObject::GenerateThumbnail(const gd::Project & project, wxBitmap & thumbnail) const
{
#if !defined(GD_NO_WX_GUI)
    thumbnail = wxBitmap("CppPlatform/Extensions/texticon24.png", wxBITMAP_TYPE_ANY);
#endif

    return true;
}

void TextBoxObject::EditObject( wxWindow* parent, gd::Project & game, gd::MainFrameWrapper & mainFrameWrapper )
{
#if !defined(GD_NO_WX_GUI)

#endif
}
#endif

void TextBoxObject::SetFontFilename(const gd::String & fontFilename)
{
    fontName = fontFilename;
    #if defined(GD_IDE_ONLY)
    fontName = gd::AbstractFileSystem::NormalizeSeparator(fontName);
    #endif
};

/* RuntimeTextBoxObject : */

RuntimeTextBoxObject::RuntimeTextBoxObject(RuntimeScene & scene, const gd::Object & object) :
    RuntimeObject(scene, object),
    scene(scene),
    opacity(255),
    angle(0)
{
    const TextBoxObject & textboxObject = static_cast<const TextBoxObject&>(object);

    InitWidget();
}

void RuntimeTextBoxObject::InitWidget()
{
    m_textbox = simplgui::TextBox::create(GDResourcesGetter::create());
    m_textbox->setPosition(sf::Vector2f(GetX(), GetY()));
    m_textbox->setSize(sf::Vector2f(GetWidth(), GetHeight()));
    m_textbox->setText(U"Text"); //TODO: Use the text from gd::Object
}

bool RuntimeTextBoxObject::Draw( sf::RenderTarget& renderTarget )
{
    if ( hidden ) return true; //Don't draw anything if hidden

    renderTarget.draw( *m_textbox );
    return true;
}

namespace
{
    std::vector<sf::View> GetViewsUnderMouse(sf::Vector2u windowSize, const RuntimeLayer &layer, sf::Vector2i mousePosition)
    {
        std::vector<sf::View> views;
        for(std::size_t i = 0; i < layer.GetCameraCount(); ++i)
        {
            const RuntimeCamera &camera = layer.GetCamera(i);

            sf::IntRect cameraViewport(
                static_cast<float>(windowSize.x) * camera.GetSFMLView().getViewport().left,
                static_cast<float>(windowSize.y) * camera.GetSFMLView().getViewport().top,
                static_cast<float>(windowSize.x) * camera.GetSFMLView().getViewport().width,
                static_cast<float>(windowSize.y) * camera.GetSFMLView().getViewport().height
            );

            if(cameraViewport.contains(mousePosition))
                views.push_back(camera.GetSFMLView());
        }

        return views;
    }

    std::vector<simplgui::Event> GetEvents(const sf::RenderWindow &window, const RuntimeLayer &layer, const std::vector<sf::Event> &sfmlEvents)
    {
        std::vector<simplgui::Event> events;
        for(auto &sfmlEvent : sfmlEvents)
        {
            if(sfmlEvent.type == sf::Event::MouseButtonPressed || sfmlEvent.type == sf::Event::MouseButtonReleased || sfmlEvent.type == sf::Event::MouseMoved)
            {
                sf::Vector2i mousePosition(
                    sfmlEvent.type == sf::Event::MouseMoved ?
                        sf::Vector2i(sfmlEvent.mouseMove.x, sfmlEvent.mouseMove.y) :
                        sf::Vector2i(sfmlEvent.mouseButton.x, sfmlEvent.mouseButton.y)
                );

                for(auto &view : GetViewsUnderMouse(window.getSize(), layer, mousePosition))
                {
                    events.emplace_back(
                        sfmlEvent,
                        window,
                        view
                    );
                }
            }
            else
            {
                events.emplace_back(
                    sfmlEvent,
                    window,
                    layer.GetCamera(0).GetSFMLView()
                );
            }
        }

        return events;
    }
}

void RuntimeTextBoxObject::UpdateTime(float dt)
{
    //Process the events
    for(auto &event : GetEvents(*scene.renderWindow, scene.GetRuntimeLayer(GetLayer()), scene.GetInputManager().GetLastFrameEvents()))
        m_textbox->processEvent(event);

    //Update the textbox
    m_textbox->update(sf::seconds(dt));
}

void RuntimeTextBoxObject::OnPositionChanged()
{
    m_textbox->setPosition(sf::Vector2f(GetX(), GetY()));
}

/**
 * RuntimeTextBoxObject provides a basic bounding box.
 */
std::vector<Polygon2d> RuntimeTextBoxObject::GetHitBoxes() const
{
    std::vector<Polygon2d> mask;
    Polygon2d rectangle = Polygon2d::CreateRectangle(GetWidth(), GetHeight());
    rectangle.Rotate(GetAngle()/180*3.14159);
    rectangle.Move(GetX()+GetCenterX(), GetY()+GetCenterY());

    mask.push_back(rectangle);
    return mask;
}

/**
 * Get the real X position of the sprite
 */
float RuntimeTextBoxObject::GetDrawableX() const
{
    return text.getPosition().x-text.getOrigin().x;
}

/**
 * Get the real Y position of the text
 */
float RuntimeTextBoxObject::GetDrawableY() const
{
    return text.getPosition().y-text.getOrigin().y;
}

/**
 * Width is the width of the current sprite.
 */
float RuntimeTextBoxObject::GetWidth() const
{
    return 200.f;
}

/**
 * Height is the height of the current sprite.
 */
float RuntimeTextBoxObject::GetHeight() const
{
    return 48.f;
}

void RuntimeTextBoxObject::SetString(const gd::String & str)
{
    text.setString(str);
    text.setOrigin(text.getLocalBounds().width/2, text.getLocalBounds().height/2);
}

gd::String RuntimeTextBoxObject::GetString() const
{
    return text.getString();
}

/**
 * Change the color filter of the sprite object
 */
void RuntimeTextBoxObject::SetColor( unsigned int r, unsigned int g, unsigned int b )
{
    text.setColor(sf::Color(r, g, b, opacity));
}

void RuntimeTextBoxObject::SetColor(const gd::String & colorStr)
{
    std::vector < gd::String > colors = colorStr.Split(U';');

    if ( colors.size() < 3 ) return; //La couleur est incorrecte

    SetColor(  colors[0].To<int>(),
               colors[1].To<int>(),
               colors[2].To<int>() );
}

void RuntimeTextBoxObject::SetOpacity(float val)
{
    if ( val > 255 ) val = 255;
    else if ( val < 0 ) val = 0;

    opacity = val;
    const sf::Color & currentColor = text.getColor();
    text.setColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, opacity));
}

void RuntimeTextBoxObject::ChangeFont(const gd::String & fontName_)
{
    if ( !text.getFont() || fontName_ != fontName )
    {

    }
}

void RuntimeTextBoxObject::SetFontStyle(int style)
{
    text.setStyle(style);
}

int RuntimeTextBoxObject::GetFontStyle()
{
    return text.getStyle();
}

bool RuntimeTextBoxObject::HasFontStyle(sf::Text::Style style)
{
    return (text.getStyle() & style) != 0;
}

bool RuntimeTextBoxObject::IsBold()
{
    return HasFontStyle(sf::Text::Bold);
}

void RuntimeTextBoxObject::SetBold(bool bold)
{
    SetFontStyle((bold ? sf::Text::Bold : 0) |
                 (IsItalic() ? sf::Text::Italic : 0) |
                 (IsUnderlined() ? sf::Text::Underlined : 0) );
}

bool RuntimeTextBoxObject::IsItalic()
{
    return HasFontStyle(sf::Text::Italic);
}

void RuntimeTextBoxObject::SetItalic(bool italic)
{
    SetFontStyle((IsBold() ? sf::Text::Bold : 0) |
                 (italic ? sf::Text::Italic : 0) |
                 (IsUnderlined() ? sf::Text::Underlined : 0) );
}

bool RuntimeTextBoxObject::IsUnderlined()
{
    return HasFontStyle(sf::Text::Underlined);
}

void RuntimeTextBoxObject::SetUnderlined(bool underlined)
{
    SetFontStyle((IsBold() ? sf::Text::Bold : 0) |
                 (IsItalic() ? sf::Text::Italic : 0) |
                 (underlined ? sf::Text::Underlined : 0) );
}

void RuntimeTextBoxObject::SetSmooth(bool smooth)
{
    smoothed = smooth;

    if ( text.getFont() )
        const_cast<sf::Texture&>(text.getFont()->getTexture(GetCharacterSize())).setSmooth(smooth);
}

#if defined(GD_IDE_ONLY)
void RuntimeTextBoxObject::GetPropertyForDebugger(std::size_t propertyNb, gd::String & name, gd::String & value) const
{
    if      ( propertyNb == 0 ) {name = _("Text");                     value = GetString();}
    else if ( propertyNb == 1 ) {name = _("Font");                    value = GetFontFilename();}
    else if ( propertyNb == 2 ) {name = _("Font Size");      value = gd::String::From(GetCharacterSize());}
    else if ( propertyNb == 3 ) {name = _("Color");       value = gd::String::From(GetColorR())+";"+gd::String::From(GetColorG())+";"+gd::String::From(GetColorB());}
    else if ( propertyNb == 4 ) {name = _("Opacity");       value = gd::String::From(GetOpacity());}
    else if ( propertyNb == 5 ) {name = _("Smoothing");       value = smoothed ? _("Yes") : _("No");}
}

bool RuntimeTextBoxObject::ChangeProperty(std::size_t propertyNb, gd::String newValue)
{
    if      ( propertyNb == 0 ) { SetString(newValue); return true; }
    else if ( propertyNb == 1 ) { ChangeFont(newValue); }
    else if ( propertyNb == 2 ) { SetCharacterSize(newValue.To<int>()); }
    else if ( propertyNb == 3 )
    {
        gd::String r, gb, g, b;
        {
            size_t separationPos = newValue.find(";");

            if ( separationPos > newValue.length())
                return false;

            r = newValue.substr(0, separationPos);
            gb = newValue.substr(separationPos+1, newValue.length());
        }

        {
            size_t separationPos = gb.find(";");

            if ( separationPos > gb.length())
                return false;

            g = gb.substr(0, separationPos);
            b = gb.substr(separationPos+1, gb.length());
        }

        SetColor(r.To<int>(), g.To<int>(), b.To<int>());
    }
    else if ( propertyNb == 4 ) { SetOpacity(newValue.To<float>()); }
    else if ( propertyNb == 5 ) { SetSmooth(!(newValue == _("No"))); }

    return true;
}

std::size_t RuntimeTextBoxObject::GetNumberOfProperties() const
{
    return 6;
}
#endif


RuntimeObject * CreateRuntimeTextBoxObject(RuntimeScene & scene, const gd::Object & object)
{
    return new RuntimeTextBoxObject(scene, object);
}

gd::Object * CreateTextBoxObject(gd::String name)
{
    return new TextBoxObject(name);
}
