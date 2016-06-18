/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef GDCORE_ANIMATION_H
#define GDCORE_ANIMATION_H
#include <vector>
#include "GDCore/String.h"
namespace gd { class Direction; }

namespace gd
{

/**
 * \brief Class representing an animation of a SpriteObject.
 *
 * \see SpriteObject
 * \see Direction
 * \ingroup SpriteObjectExtension
 */
class GD_CORE_API Animation
{
public:
    Animation();
    virtual ~Animation();

    /**
     * \brief Set the name of the animation
     */
    const gd::String & GetName() const { return name; }

    /**
     * \brief Change the name of the animation
     */
    void SetName(const gd::String & name_) { name = name_; }

    /**
     * \brief Return the n-th direction
     */
    const Direction & GetDirection(std::size_t n) const;

    /**
     * \brief Return the n-th direction
     */
    Direction & GetDirection(std::size_t n);

    /**
     * \brief Change a direction
     */
    void SetDirection(const Direction & direction, std::size_t nb);

    /**
     * \brief Change direction count
     */
    void SetDirectionsCount(std::size_t nb);

    /**
     * \brief Get direction count
     */
    std::size_t GetDirectionsCount() const;

    /**
     * \brief Return true if there isn't any direction in the animation
     */
    bool HasNoDirections() const;

    bool useMultipleDirections;

private:
    std::vector < Direction > directions;
    gd::String name;

    static Direction badDirection;
};

}
#endif // GDCORE_ANIMATION_H
