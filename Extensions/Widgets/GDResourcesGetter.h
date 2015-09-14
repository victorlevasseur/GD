#include <memory>
#include "simplgui/ResourcesGetter.h"

class GDResourcesGetter : public simplgui::ResourcesGetter
{
public:
    using Ptr = std::shared_ptr<GDResourcesGetter>;

    static Ptr create();

    virtual std::shared_ptr<sf::Font> loadFont(const std::string &name);

protected:
    GDResourcesGetter() : simplgui::ResourcesGetter() {};
};
