/**

GDevelop - Tile Map Extension
Copyright (c) 2014-2015 Victor Levasseur (victorlevasseur52@gmail.com)
This project is released under the MIT License.
*/

#ifndef RUNTIMEELLIPSISSHAPE_H
#define RUNTIMEELLIPSISSHAPE_H

#include <string>

#include "GDCpp/Project/Object.h"
#include "GDCpp/RuntimeObject.h"
#include "GDCpp/Polygon2d.h"
#include <memory>
#include <SFML/Graphics/VertexArray.hpp>


class SFMLTextureWrapper;
class RuntimeScene;
namespace gd { class ImageManager; }
namespace gd { class InitialInstance; }
#if defined(GD_IDE_ONLY)
namespace gd { class Project; }
namespace gd { class MainFrameWrapper; }
class wxBitmap;
class wxWindow;
#endif

class GD_EXTENSION_API RuntimeEllipseObject : public RuntimeObject
{

public :

    RuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object);
    virtual ~RuntimeEllipseObject() {};
    virtual RuntimeObject * Clone() const { return new RuntimeEllipseObject(*this);}

    virtual bool Draw(sf::RenderTarget & renderTarget);

    virtual float GetWidth() const;
    virtual float GetHeight() const;

    virtual float GetAngle() const {return 0;};
    virtual bool SetAngle(float ang) {return false;};

    virtual void SetWidth(float newWidth) {};
    virtual void SetHeight(float newHeight) {};

    virtual void OnPositionChanged();

    #if defined(GD_IDE_ONLY)
    virtual void GetPropertyForDebugger (std::size_t propertyNb, gd::String & name, gd::String & value) const;
    virtual bool ChangeProperty(std::size_t propertyNb, gd::String newValue);
    virtual std::size_t GetNumberOfProperties() const;
    #endif

private:

};

RuntimeObject * CreateRuntimeEllipseObject(RuntimeScene & scene, const gd::Object & object);

#endif
