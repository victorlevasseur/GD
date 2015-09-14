#ifndef SIMPLGUI_STATEDEPPROPERTY_H
#define SIMPLGUI_STATEDEPPROPERTY_H

#include <initializer_list>

namespace simplgui
{

template<typename T>
class StateDepProperty
{
public:
    /**
     * Create a StateDepProperty object and initialize each state properties
     * (normal, focused, disabled, hovered) with **all**.
     */
    StateDepProperty(const T &all) : normal(all), focused(all), disabled(all), hovered(all) { }
    
    /**
     * Create a StateDepProperty object and initialize the normal, hovered and
     * disabled properties with **normal** and the focused property with **focused**.
     */
    StateDepProperty(const T &normal, const T &focused) : normal(normal), focused(focused), disabled(normal), hovered(normal) { }
    
    /**
     * Create a StateDepProperty object and initialize the normal, hovered
     * properties with **normal**, the focused property with **focused** and
     * the disabled property with **disabled**.
     */
    StateDepProperty(const T &normal, const T &focused, const T &disabled) : normal(normal), focused(focused), disabled(disabled), hovered(normal) { }
    
    /**
     * Create a StateDepProperty object and initialize the normal property with 
     * **normal**, the focused property with **focused**, the disabled property 
     * with **disabled** and the hovered property with **hovered**.
     */
    StateDepProperty(const T &normal, const T &focused, const T &disabled, const T &hovered) : normal(normal), focused(focused), disabled(disabled), hovered(hovered) { }

    T normal;
    T focused;
    T disabled;
    T hovered;
};

}

#endif

