/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include <iostream>
#include "GDCore/String.h"
#include <vector>
#include "GDCore/Extensions/Builtin/SpriteExtension/Direction.h"
#include "GDCore/Extensions/Builtin/SpriteExtension/Sprite.h"
#include "GDCore/Serialization/SerializerElement.h"
#include "GDCore/CommonTools.h"

using namespace std;

namespace gd {

Direction::Direction() :
loop(false),
timeBetweenFrame(1)
{
}

Direction::~Direction()
{
};

void Direction::SetLoop( bool loop_ )
{
    loop = loop_;
}

void  Direction::SetTimeBetweenFrames( float time )
{
    timeBetweenFrame = time;
}

void Direction::AddSprite( const Sprite & sprite )
{
    sprites.push_back(sprite);
}

const Sprite & Direction::GetSprite(std::size_t nb) const
{
    return sprites[nb];
}

Sprite & Direction::GetSprite(std::size_t nb)
{
    return sprites[nb];
}

void Direction::RemoveSprite(std::size_t index)
{
    if ( index < sprites.size() )
        sprites.erase(sprites.begin()+index);
}

void Direction::SwapSprites(std::size_t firstSpriteIndex, std::size_t secondSpriteIndex)
{
    if ( firstSpriteIndex < sprites.size() && secondSpriteIndex < sprites.size() && firstSpriteIndex != secondSpriteIndex)
        swap(sprites[firstSpriteIndex], sprites[secondSpriteIndex]);
}

bool Direction::HasNoSprites() const
{
    return sprites.empty();
}

std::size_t Direction::GetSpritesCount() const
{
    return sprites.size();
}

void Direction::RemoveAllSprites()
{
    sprites.clear();
}

void OpenPoint(Point & point, const gd::SerializerElement & element)
{
    point.SetName(element.GetStringAttribute("name", "", "nom"));
    point.SetX(element.GetDoubleAttribute("x", 0, "X"));
    point.SetY(element.GetDoubleAttribute("y", 0, "Y"));
}

void OpenPointsSprites(vector < Point > & points, const gd::SerializerElement & element)
{
    element.ConsiderAsArrayOf("point", "Point");
    for (std::size_t i = 0; i < element.GetChildrenCount(); ++i)
    {
        Point point("");
        OpenPoint(point, element.GetChild(i));

        points.push_back(point);
    }
}

void Direction::UnserializeFrom(const gd::SerializerElement & element)
{
    SetTimeBetweenFrames(element.GetDoubleAttribute("timeBetweenFrames", 1, "tempsEntre"));
    SetLoop(element.GetBoolAttribute("looping", false, "boucle"));

    const gd::SerializerElement & spritesElement = element.GetChild("sprites", 0, "Sprites");
    spritesElement.ConsiderAsArrayOf("sprite", "Sprite");
    for (std::size_t i = 0; i < spritesElement.GetChildrenCount(); ++i)
    {
        const gd::SerializerElement & spriteElement = spritesElement.GetChild(i);
        Sprite sprite;

        sprite.SetImageName(spriteElement.GetStringAttribute("image"));
        OpenPointsSprites(sprite.GetAllNonDefaultPoints(), spriteElement.GetChild("points", 0, "Points"));

        OpenPoint(sprite.GetOrigin(), spriteElement.GetChild("originPoint" , 0, "PointOrigine"));
        OpenPoint(sprite.GetCenter(), spriteElement.GetChild("centerPoint" , 0, "PointCentre"));
        sprite.SetDefaultCenterPoint(spriteElement.GetChild("centerPoint" , 0, "PointCentre").GetBoolAttribute("automatic", true));

        if (spriteElement.HasChild("CustomCollisionMask"))
            sprite.SetCollisionMaskAutomatic(!spriteElement.GetChild("CustomCollisionMask").GetBoolAttribute("custom", false));
        else
            sprite.SetCollisionMaskAutomatic(!spriteElement.GetBoolAttribute("hasCustomCollisionMask", false));

        std::vector<Polygon2d> mask;
        const gd::SerializerElement & collisionMaskElement = spriteElement.GetChild("customCollisionMask", 0, "CustomCollisionMask");
        collisionMaskElement.ConsiderAsArrayOf("polygon", "Polygon");
        for (std::size_t j = 0; j < collisionMaskElement.GetChildrenCount(); ++j)
        {
            Polygon2d polygon;

            const gd::SerializerElement & polygonElement = collisionMaskElement.GetChild(j);
            polygonElement.ConsiderAsArrayOf("vertice", "Point");
            for (std::size_t k = 0; k < polygonElement.GetChildrenCount(); ++k)
            {
                const gd::SerializerElement & verticeElement = polygonElement.GetChild(k);

                polygon.vertices.push_back(sf::Vector2f(verticeElement.GetDoubleAttribute("x"),
                    verticeElement.GetDoubleAttribute("y")));
            }

            mask.push_back(polygon);
        }
        sprite.SetCustomCollisionMask(mask);

        sprites.push_back(sprite);
    }
};

#if defined(GD_IDE_ONLY)
void SavePoint(const Point & point, gd::SerializerElement & element)
{
    element.SetAttribute("name", point.GetName());
    element.SetAttribute("x", point.GetX());
    element.SetAttribute("y", point.GetY());
}

void SavePointsSprites(const vector < Point > & points, gd::SerializerElement & element)
{
    element.ConsiderAsArrayOf("point");
    for (std::size_t i = 0;i<points.size();++i)
        SavePoint(points[i], element.AddChild("point"));
}

void SaveSpritesDirection(const vector < Sprite > & sprites, gd::SerializerElement & element)
{
    element.ConsiderAsArrayOf("sprite");
    for (std::size_t i = 0;i<sprites.size();++i)
    {
        gd::SerializerElement & spriteElement = element.AddChild("sprite");

        spriteElement.SetAttribute("image", sprites[i].GetImageName());
        SavePointsSprites(sprites[i].GetAllNonDefaultPoints(), spriteElement.AddChild("points"));

        SavePoint(sprites[i].GetOrigin(), spriteElement.AddChild("originPoint"));
        SavePoint(sprites[i].GetCenter(), spriteElement.AddChild("centerPoint"));
        spriteElement.GetChild("centerPoint").SetAttribute("automatic", sprites[i].IsDefaultCenterPoint());

        spriteElement.SetAttribute("hasCustomCollisionMask", !sprites[i].IsCollisionMaskAutomatic());

        gd::SerializerElement & collisionMaskElement = spriteElement.AddChild("customCollisionMask");
        collisionMaskElement.ConsiderAsArrayOf("polygon");
        std::vector<Polygon2d> polygons = sprites[i].GetCollisionMask();
        for (std::size_t j = 0;j<polygons.size();++j)
        {
            gd::SerializerElement & polygonElement = collisionMaskElement.AddChild("polygon");
            polygonElement.ConsiderAsArrayOf("vertice");
            for (std::size_t k = 0;k<polygons[j].vertices.size();++k)
            {
                polygonElement.AddChild("vertice")
                    .SetAttribute("x", polygons[j].vertices[k].x)
                    .SetAttribute("y", polygons[j].vertices[k].y);
            }
        }
    }
}

void Direction::SerializeTo(gd::SerializerElement & element) const
{
    element.SetAttribute("looping", IsLooping());
    element.SetAttribute("timeBetweenFrames", GetTimeBetweenFrames());
    SaveSpritesDirection(sprites, element.AddChild("sprites"));
}
#endif

}
