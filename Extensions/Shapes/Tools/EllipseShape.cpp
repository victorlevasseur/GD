#include "EllipseShape.h"

#include <cmath>

EllipseShape::EllipseShape(float width, float height, std::size_t pointCount)
 : m_width(width),
   m_height(height),
   m_pointCount(pointCount)
{
    update();
}

void EllipseShape::SetWidth(float width)
{
    m_width = width;
    update();
}

float EllipseShape::GetWidth() const
{
    return m_width;
}

void EllipseShape::SetHeight(float height)
{
    m_height = height;
    update();
}

float EllipseShape::GetHeight() const
{
    return m_height;
}

void EllipseShape::setPointCount(std::size_t count)
{
    m_pointCount = count;
    update();
}

unsigned int EllipseShape::getPointCount() const
{
    return m_pointCount;
}

sf::Vector2f EllipseShape::getPoint(unsigned int index) const
{
    static const float pi = 3.141592654f;

    float angle = index * 2 * pi / m_pointCount - pi / 2;
    float x = std::cos(angle) * m_width/2.f;
    float y = std::sin(angle) * m_height/2.f;

    return sf::Vector2f(m_width/2.f + x, m_height/2.f + y);
}
