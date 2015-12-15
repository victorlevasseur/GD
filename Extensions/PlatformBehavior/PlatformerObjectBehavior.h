/**

GDevelop - Platform Behavior Extension
Copyright (c) 2013-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#ifndef PLATFORMEROBJECTBEHAVIOR_H
#define PLATFORMEROBJECTBEHAVIOR_H
#include "GDCpp/Project/Behavior.h"
#include "GDCpp/Project/Object.h"
#include <SFML/System/Vector2.hpp>
#include <map>
#include <set>
namespace gd { class Layout; }
class RuntimeScene;
class PlatformBehavior;
class ScenePlatformObjectsManager;
namespace gd { class SerializerElement; }
class RuntimeScenePlatformData;

/**
 * \brief Allows objects to jump and stand on platforms.
 */
class GD_EXTENSION_API PlatformerObjectBehavior : public Behavior
{
public:
    PlatformerObjectBehavior();
    virtual ~PlatformerObjectBehavior();
    virtual Behavior* Clone() const { return new PlatformerObjectBehavior(*this); }

    double GetGravity() const { return gravity; };
    double GetMaxFallingSpeed() const { return maxFallingSpeed; };
    double GetAcceleration() const { return acceleration; };
    double GetDeceleration() const { return deceleration; };
    double GetMaxSpeed() const { return maxSpeed; };
    double GetJumpSpeed() const { return jumpSpeed; };
    double GetSlopeMaxAngle() const { return slopeMaxAngle; };

    void SetGravity(double gravity_) { gravity = gravity_; };
    void SetMaxFallingSpeed(double maxFallingSpeed_) { maxFallingSpeed = maxFallingSpeed_; };
    void SetAcceleration(double acceleration_) { acceleration = acceleration_; };
    void SetDeceleration(double deceleration_) { deceleration = deceleration_; };
    void SetMaxSpeed(double maxSpeed_) { maxSpeed = maxSpeed_; };
    void SetJumpSpeed(double jumpSpeed_) { jumpSpeed = jumpSpeed_; };
    bool SetSlopeMaxAngle(double slopeMaxAngle_);
    void SetCanJump() { canJump = true; };

    void IgnoreDefaultControls(bool ignore = true) { ignoreDefaultControls = ignore; };
    void SimulateControl(const gd::String & input);
    void SimulateLeftKey() { leftKey = true; };
    void SimulateRightKey() { rightKey = true; };
    void SimulateLadderKey() { ladderKey = true; };
    void SimulateUpKey() { upKey = true; };
    void SimulateDownKey() { downKey = true; };
    void SimulateJumpKey() { jumpKey = true; };

    bool IsOnFloor() const { return isOnFloor; }
    bool IsOnLadder() const { return isOnLadder; }
    bool IsJumping() const { return jumping; }
    bool IsFalling() const { return !isOnFloor && !isOnLadder && (!jumping || currentJumpSpeed < currentFallSpeed); }
    bool IsMoving() const { return (currentSpeed != 0 && hasReallyMoved) || currentJumpSpeed != 0 || currentFallSpeed != 0; }

    virtual void OnOwnerChanged();

    /**
     * \brief Unserialize the behavior
     */
    virtual void UnserializeFrom(const gd::SerializerElement & element);

    #if defined(GD_IDE_ONLY)
    /**
     * \brief Serialize the behavior
     */
    virtual void SerializeTo(gd::SerializerElement & element) const;

    virtual std::map<gd::String, gd::PropertyDescriptor> GetProperties(gd::Project & project) const;
    virtual bool UpdateProperty(const gd::String & name, const gd::String & value, gd::Project & project);
    #endif

private:
    virtual void DoStepPreEvents(RuntimeScene & scene);
    virtual void DoStepPostEvents(RuntimeScene & scene);

    /**
     * \brief Return a list of all the platforms that could be colliding with the object if it is moved.
     * \param maxMovementLength The maximum length of any movement that could be done by the object, in pixels.
     * \warning sceneManager must be valid and not NULL.
     */
    std::set<PlatformBehavior*> GetPotentialCollidingObjects(double maxMovementLength);

    /**
     * \brief Separate the object from all platforms passed as parameter, except ladders.
     * \param candidates The platform to be tested for collision
     * \param excludeJumpThrus If set to true, the jump thru platform will be excluded.
     */
    bool SeparateFromPlatforms(const std::set<PlatformBehavior*> & candidates, bool excludeJumpThrus);

    /**
     * \brief Among the platforms passed in parameter, return a list of the platforms colliding with the object.
     * \note Ladders are *always* excluded from the test.
     * \param candidates The platform to be tested for collision
     * \param exceptTheseOnes The platforms to be excluded from the test
     */
    std::set<PlatformBehavior*> GetPlatformsCollidingWith(const std::set<PlatformBehavior*> & candidates,
        const std::set<PlatformBehavior*> & exceptTheseOnes);

    /**
     * \brief Among the platforms passed in parameter, return true if there is a platform colliding with the object.
     * \note Ladders are *always* excluded from the test.
     * \param candidates The platform to be tested for collision
     * \param exceptThisOne If not NULL, this platform won't be tested for collision.
     * \param excludeJumpThrus If set to true, the jump thru platform will be excluded.
     */
    bool IsCollidingWith(const std::set<PlatformBehavior*> & candidates,
        PlatformBehavior * exceptThisOne = NULL, bool excludeJumpThrus = false);

    /**
     * \brief Among the platforms passed in parameter, return true if there is a platform colliding with the object.
     * \note Ladders are *always* excluded from the test.
     * \param candidates The platforms to be tested for collision
     * \param exceptTheseOnes The platforms to be excluded from the test
     */
    bool IsCollidingWith(const std::set<PlatformBehavior*> & candidates, const std::set<PlatformBehavior*> & exceptTheseOnes);

    /**
     * \brief Among the platforms passed in parameter, return true if the object is overlapping a ladder.
     * \param candidates The platform to be tested for collision
     */
    bool IsOverlappingLadder(const std::set<PlatformBehavior*> & candidates);

    /**
     * \brief Among the platforms passed in parameter, return a list of the jump thru platforms colliding with the object.
     * \param candidates The platform to be tested for collision
     */
    std::set<PlatformBehavior*> GetJumpthruCollidingWith(const std::set<PlatformBehavior*> & candidates);

    double gravity; ///< In pixels.seconds^-2
    double maxFallingSpeed; ///< In pixels.seconds^-1
    double acceleration; ///< In pixels.seconds^-2
    double deceleration; ///< In pixels.seconds^-2
    double maxSpeed; ///< In pixels.seconds^-1
    double jumpSpeed; ///< In pixels.seconds^-1
    double slopeMaxAngle; ///< In degrees
    double slopeClimbingFactor; ///< Equals to tan(slopeMaxAngle).

    RuntimeScene * parentScene; ///< The scene the object belongs to.
    ScenePlatformObjectsManager * sceneManager; ///< The platform objects manager associated to the scene.
    bool isOnFloor; ///< True if the object is on a floor.
    bool isOnLadder; ///< True if the object is on a ladder.
    PlatformBehavior * floorPlatform; ///< The platform the object is on, when isOnFloor == true.
    double floorLastX; ///< The last X position of the floor platform, when isOnFloor == true.
    double floorLastY; ///< The last Y position of the floor platform, when isOnFloor == true.
    double currentFallSpeed; ///< The current speed of the fall, when isOnFloor == false.
    double currentSpeed; ///< The current speed in the left/right direction.
    bool jumping; ///< True if the object is jumping.
    double currentJumpSpeed; ///< The current speed of the jump, when jumping == true.
    bool canJump; ///< True if the object can jump.
    bool hasReallyMoved; ///< Used for IsMoving(): Only set to true when the object has moved from more than 1 pixel horizontally.

    //Object size tracking:
    bool trackSize; ///< If true, the behavior try to change the object position to avoid glitch when size change.
    float oldHeight; ///< Object old height, used to track changes in height.

    bool ignoreDefaultControls; ///< If set to true, do not track the default inputs.
    bool leftKey;
    bool rightKey;
    bool ladderKey;
    bool upKey;
    bool downKey;
    bool jumpKey;
};
#endif // PLATFORMEROBJECTBEHAVIOR_H

