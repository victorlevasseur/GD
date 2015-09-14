#ifndef SIMPLGUI_CONNECTOR_H
#define SIMPLGUI_CONNECTOR_H

#include <functional>
#include <map>
#include <vector>

namespace simplgui
{

template<class... Args>
class Connector
{
public:
    Connector() : m_bindedFunctions(), currentId(0) {};
    
    unsigned int bind(std::function<void(Args...)> &function)
    {
        unsigned int newId = currentId++;
        m_bindedFunctions[newId] = function;
        
        return newId;
    }
    
    unsigned int bind(std::function<void(Args...)> &&function)
    {
        unsigned int newId = currentId++;
        m_bindedFunctions[newId] = function;
        
        return newId;
    }
    
    void unbind(unsigned int id)
    {
        m_bindedFunctions.erase(id);
    }
    
    void call(Args... args)
    {
        for(auto it = m_bindedFunctions.begin(); it != m_bindedFunctions.end(); ++it)
        {
            (it->second)(args...);
        }
    }

private:
    std::map<unsigned int, std::function<void(Args...)>> m_bindedFunctions;
    unsigned int currentId;
};

}

#endif
