#include "GDResourcesGetter.h"

#include "GDCpp/Runtime/FontManager.h"

GDResourcesGetter::Ptr GDResourcesGetter::create()
{
    return std::shared_ptr<GDResourcesGetter>(new GDResourcesGetter());
}

std::shared_ptr<sf::Font> GDResourcesGetter::loadFont(const std::string &name)
{
    return std::shared_ptr<sf::Font>(new sf::Font(*FontManager::Get()->GetFont(gd::String::FromUTF8(name))));
}
