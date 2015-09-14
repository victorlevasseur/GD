#include "simplgui/Renderer.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "simplgui/Widget.h"
#include "simplgui/Tools.h"

namespace simplgui
{

void Renderer::drawBackgroundRectangle(sf::RenderTarget &target, std::shared_ptr<const Widget> widget, sf::FloatRect rectangle)
{
    drawRectangle(
        target,
        rectangle,
        widget->getTheme().getProperty<float>("border_thickness"),
        widget->isFocused() ? 
            widget->getTheme().getProperty<StateColor>("background_color").focused : 
            widget->getTheme().getProperty<StateColor>("background_color").normal,
        widget->isFocused() ? 
            widget->getTheme().getProperty<StateColor>("border_color").focused : 
            widget->getTheme().getProperty<StateColor>("border_color").normal,
        widget->getGlobalTransform()
    );
}

void Renderer::drawSelectionRectangle(sf::RenderTarget &target, std::shared_ptr<const Widget> widget, sf::FloatRect rectangle)
{
    drawRectangle(
        target,
        rectangle,
        widget->getTheme().getProperty<float>("selection_border_thickness"),
        widget->isFocused() ? 
            widget->getTheme().getProperty<StateColor>("selection_color").focused : 
            widget->getTheme().getProperty<StateColor>("selection_color").normal,
        widget->isFocused() ? 
            widget->getTheme().getProperty<StateColor>("selection_border_color").focused : 
            widget->getTheme().getProperty<StateColor>("selection_border_color").normal,
        widget->getGlobalTransform()
    );
}

void Renderer::drawRectangle(
    sf::RenderTarget &target,
    sf::FloatRect rectangle, 
    float outline,
    sf::Color fillColor,
    sf::Color outlineColor,
    sf::Transform transform
    )
{
    sf::RectangleShape rect(sf::Vector2f(rectangle.width, rectangle.height));
    rect.setPosition(sf::Vector2f(rectangle.left, rectangle.top));
    rect.setFillColor(fillColor);
    rect.setOutlineColor(outlineColor);
    rect.setOutlineThickness(outline);
    
    target.draw(rect, sf::RenderStates(transform));
}

sf::Vector2f Renderer::getTextSize(const std::u32string &str, const sf::Font &font, unsigned int size)
{
    sf::Text text(tools::getSfString(str), font, size);
    
    return sf::Vector2f(
        text.getLocalBounds().left + text.getLocalBounds().width,
        text.getLocalBounds().top + text.getLocalBounds().height
    );
}

sf::Vector2f Renderer::getCharPosInText(const std::u32string &str, const sf::Font &font, unsigned int size, int charIndex)
{
    sf::Text text(tools::getSfString(str), font, size);
    
    if(charIndex <= 0)
        return sf::Vector2f(text.getLocalBounds().left, text.getLocalBounds().top);
    else
        return text.findCharacterPos(charIndex);
}
 
void Renderer::drawText(
    sf::RenderTarget &target,
    std::shared_ptr<const Widget> widget,
    const std::u32string &str,
    const sf::Font &font,
    sf::Vector2f position
    )
{
    drawText(
        target,
        str,
        font,
        widget->getTheme().getProperty<unsigned int>("text_size", 30),
        position,
        widget->isFocused() ? 
            widget->getTheme().getProperty<StateColor>("text_color").focused : 
            widget->getTheme().getProperty<StateColor>("text_color").normal,
        widget->getGlobalTransform()
    );
}

void Renderer::drawText(
    sf::RenderTarget &target,
    const std::u32string &str,
    const sf::Font &font,
    unsigned int size,
    sf::Vector2f position,
    sf::Color color,
    sf::Transform transform
    )
{
    sf::Text text(tools::getSfString(str), font, size);
    text.setColor(color);
    text.setPosition(position);
    
    target.draw(text, transform);
}

}

