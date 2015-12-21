#ifndef EXTENSION_SHAPES_ELLIPSIS
#define EXTENSION_SHAPES_ELLIPSIS

#include <cstddef>
#include <SFML/Graphics/Shape.hpp>

/**
 * Shape representing a ellipse.
 */
class GD_EXTENSION_API EllipseShape : public sf::Shape
{
public:

    explicit EllipseShape(float width = 0, float height = 0, std::size_t pointCount = 30);

    void SetWidth(float width);

    float GetWidth() const;

    void SetHeight(float height);

    float GetHeight() const;

    void setPointCount(std::size_t count);

    virtual unsigned int getPointCount() const;

    virtual sf::Vector2f getPoint(unsigned int index) const;

private:

    float m_width;
    float m_height;
    std::size_t m_pointCount;
};

#endif
