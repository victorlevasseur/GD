#include "Tools/ShapeTools.h"

#include <stdexcept>

namespace shapetools
{

sf::Color GD_EXTENSION_API ParseColor(const gd::String &str)
{
    std::vector<gd::String> colorComponents = str.Split(U';');

    if(colorComponents.size() != 3 && colorComponents.size() != 4)
    {
        throw std::domain_error("shapestools::ParseColor: can't parse the string " + str.ToUTF8() + " !");
    }

    return sf::Color(
        colorComponents[0].To<int>(),
        colorComponents[1].To<int>(),
        colorComponents[2].To<int>(),
        colorComponents.size() == 4 ? colorComponents[3].To<int>() : 255
    );
}

gd::String GD_EXTENSION_API ColorToString(const sf::Color &color)
{
    return gd::String::From<int>(color.r) + ";" +
        gd::String::From<int>(color.g) + ";" +
        gd::String::From<int>(color.b) +
        (color.a != 255 ? ";" + gd::String::From<int>(color.a) : "");
}

}
