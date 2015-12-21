#ifndef EXTENSION_SHAPES_TOOLS
#define EXTENSION_SHAPES_TOOLS

#include <SFML/Graphics/Color.hpp>
#include "GDCpp/String.h"

namespace shapetools
{

sf::Color GD_EXTENSION_API ParseColor(const gd::String &str);
gd::String GD_EXTENSION_API ColorToString(const sf::Color &color);

}

#endif
