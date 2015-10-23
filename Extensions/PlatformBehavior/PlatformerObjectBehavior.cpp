/**

GDevelop - Platform Behavior Extension
Copyright (c) 2014-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#include <memory>
#include "PlatformerObjectBehavior.h"
#include "PlatformBehavior.h"
#include "ScenePlatformObjectsManager.h"
#include "GDCore/Tools/Localization.h"
#include "GDCpp/Scene.h"
#include "GDCpp/Serialization/SerializerElement.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/RuntimeObject.h"
#include "GDCpp/CommonTools.h"
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#if defined(GD_IDE_ONLY)
#include <iostream>
#include <map>
#include "GDCore/IDE/Dialogs/PropertyDescriptor.h"
#endif

PlatformerObjectBehavior::PlatformerObjectBehavior() :
    gravity(1000),
    maxFallingSpeed(700),
    acceleration(1500),
    deceleration(1500),
    maxSpeed(250),
    jumpSpeed(600),
    parentScene(NULL),
    sceneManager(NULL),
    isOnFloor(false),
    isOnLadder(false),
    floorPlatform(NULL),
    currentFallSpeed(0),
    currentSpeed(0),
    jumping(false),
    currentJumpSpeed(0),
    canJump(false),
    hasReallyMoved(false),
    trackSize(true),
    ignoreDefaultControls(false),
    leftKey(false),
    rightKey(false),
    ladderKey(false),
    upKey(false),
    downKey(false),
    jumpKey(false)
{
    SetSlopeMaxAngle(60);
}

PlatformerObjectBehavior::~PlatformerObjectBehavior()
{
}

void PlatformerObjectBehavior::OnOwnerChanged()
{
    oldHeight = object->GetHeight();
}

bool PlatformerObjectBehavior::SetSlopeMaxAngle(double newMaxAngle)
{
    if (newMaxAngle < 0 || newMaxAngle >= 90) return false;

    slopeMaxAngle = newMaxAngle;
    if ( slopeMaxAngle == 45 )
        slopeClimbingFactor = 1; //Avoid rounding errors
    else
        slopeClimbingFactor = tan(slopeMaxAngle*3.1415926/180);

    return true;
}

void PlatformerObjectBehavior::DoStepPreEvents(RuntimeScene & scene)
{
    if ( parentScene != &scene ) //Parent scene has changed
    {
        parentScene = &scene;
        sceneManager = parentScene ? &ScenePlatformObjectsManager::managers[&scene] : NULL;
        floorPlatform = NULL;
    }

    if ( !sceneManager ) return;

    double timeDelta = static_cast<double>(scene.GetTimeManager().GetElapsedTime()) / 1000000.0;

    //0.1) Get the player input:

    double requestedDeltaX = 0;
    double requestedDeltaY = 0;

    //Change the speed according to the player's input.
    leftKey |= !ignoreDefaultControls && scene.GetInputManager().IsKeyPressed("Left");
    rightKey |= !ignoreDefaultControls && scene.GetInputManager().IsKeyPressed("Right");
    if ( leftKey )
        currentSpeed -= acceleration*timeDelta;
    if ( rightKey )
        currentSpeed += acceleration*timeDelta;

    //Take deceleration into account only if no key is pressed.
    if ( leftKey == rightKey )
    {
        bool wasPositive = currentSpeed>0;
        currentSpeed -= deceleration*timeDelta*(wasPositive ? 1.0 : -1.0);

        //Set the speed to 0 if the speed was top low.
        if ( wasPositive && currentSpeed < 0 ) currentSpeed = 0;
        if ( !wasPositive && currentSpeed > 0 ) currentSpeed = 0;
    }

    if (currentSpeed > maxSpeed) currentSpeed = maxSpeed;
    if (currentSpeed < -maxSpeed) currentSpeed = -maxSpeed;
    requestedDeltaX += currentSpeed*timeDelta;

    //Compute the list of the objects that will be used
    std::set<PlatformBehavior*> potentialObjects = GetPotentialCollidingObjects(std::max(requestedDeltaX, maxFallingSpeed));
    std::set<PlatformBehavior*> overlappedJumpThru = GetJumpthruCollidingWith(potentialObjects);

    //Check that the floor object still exists and is near the object.
    if ( isOnFloor && potentialObjects.find(floorPlatform) == potentialObjects.end() )
    {
        isOnFloor = false;
        floorPlatform = NULL;
    }

    //0.2) Track changes in object size

    //Stick the object to the floor if its height has changed.
    if ( trackSize && isOnFloor && oldHeight != object->GetHeight() && !scene.GetTimeManager().IsFirstLoop() )
        object->SetY(object->GetY()+oldHeight-object->GetHeight());

    oldHeight = object->GetHeight();

    //1) X axis:

    //Shift the object according to the floor movement.
    if ( isOnFloor )
    {
        requestedDeltaX += floorPlatform->GetObject()->GetX() - floorLastX;
        requestedDeltaY += floorPlatform->GetObject()->GetY() - floorLastY;
    }

    //Ensure the object is not stuck
    if (SeparateFromPlatforms(potentialObjects, true))
    {
        canJump = true; //After being unstuck, the object must be able to jump again.
    }

    //Move the object on x axis.
    double oldX = object->GetX();
    if ( requestedDeltaX != 0 )
    {
        object->SetX(object->GetX()+requestedDeltaX);
        //Colliding: Try to push out from the solid.
        //Note that jump thru are never obstacle on X axis.
        while ( IsCollidingWith(potentialObjects, floorPlatform, /*excludeJumpthrus=*/true) )
        {
            if ( (requestedDeltaX > 0 && object->GetX() <= oldX) ||
                 (requestedDeltaX < 0 && object->GetX() >= oldX) )
            {
                object->SetX(oldX); //Unable to move the object without being stuck in an obstacle.
                break;
            }

            //If on floor: try get up a bit to bypass not perfectly aligned floors.
            if ( isOnFloor )
            {
                object->SetY(object->GetY()-1);
                if ( !IsCollidingWith(potentialObjects, floorPlatform, /*excludeJumpthrus=*/true) )
                    break;
                object->SetY(object->GetY()+1);
            }

            object->SetX(floor(object->GetX())+(requestedDeltaX > 0 ? -1 : 1));
            currentSpeed = 0; //Collided with a wall
        }
    }

    //2) Y axis:

    //Go on a ladder
    ladderKey |= !ignoreDefaultControls && scene.GetInputManager().IsKeyPressed("Up");
    if (ladderKey && IsOverlappingLadder(potentialObjects))
    {
        canJump = true;
        isOnFloor = false;
        floorPlatform = NULL;
        currentJumpSpeed = 0;
        currentFallSpeed = 0;
        isOnLadder = true;
    }

    if ( isOnLadder )
    {
        upKey |= !ignoreDefaultControls && scene.GetInputManager().IsKeyPressed("Up");
        downKey |= !ignoreDefaultControls && scene.GetInputManager().IsKeyPressed("Down");
        if ( upKey )
            requestedDeltaY -= 150*timeDelta;
        if ( downKey )
            requestedDeltaY += 150*timeDelta;

        //Coming to an extremity of a ladder
        if ( !IsOverlappingLadder(potentialObjects) )
        {
            isOnLadder = false;
        }
    }

    //Fall
    if (!isOnFloor && !isOnLadder)
    {
        currentFallSpeed += gravity*timeDelta;
        if ( currentFallSpeed > maxFallingSpeed ) currentFallSpeed = maxFallingSpeed;

        requestedDeltaY += currentFallSpeed*timeDelta;
        requestedDeltaY = std::min(requestedDeltaY, maxFallingSpeed*timeDelta);
    }

    //Jumping
    jumpKey |= !ignoreDefaultControls &&
        (scene.GetInputManager().IsKeyPressed("LShift") || scene.GetInputManager().IsKeyPressed("RShift") ||
        scene.GetInputManager().IsKeyPressed("Space"));
    if ( canJump && jumpKey )
    {
        jumping = true;
        canJump = false;
        //isOnFloor = false; If floor is a very steep slope, the object could go into it.
        isOnLadder = false;
        currentJumpSpeed = jumpSpeed;
        currentFallSpeed = 0;
        //object->SetY(object->GetY()-1);
    }

    if ( jumping )
    {
        requestedDeltaY -= currentJumpSpeed*timeDelta;
        currentJumpSpeed -= gravity*timeDelta;
        if ( currentJumpSpeed < 0 )
        {
            currentJumpSpeed = 0;
            jumping = false;
        }
    }

    //Follow the floor
    if ( isOnFloor )
    {
        if ( object->IsCollidingWith(floorPlatform->GetObject()) )
        {
            //Floor is getting up, as the object is colliding with it.
            double oldY = object->GetY();
            int step = 0;
            bool stillInFloor = false;
            do
            {
                if ( step >= floor(std::abs(requestedDeltaX*slopeClimbingFactor)) ) //Slope is too step ( > max angle )
                {
                    object->SetY(object->GetY()-(std::abs(requestedDeltaX*slopeClimbingFactor)-(double)step)); //Try to add the decimal part.
                    if ( object->IsCollidingWith(floorPlatform->GetObject()) )
                        stillInFloor = true; //Too steep.

                    break;
                }

                //Try to get out of the floor.
                object->SetY(object->GetY()-1);
                step++;
            }
            while ( object->IsCollidingWith(floorPlatform->GetObject() ) );

            if ( stillInFloor )
            {
                object->SetY(oldY); //Unable to follow the floor ( too steep ): Go back to the original position.
                object->SetX(oldX); //And also revert the shift on X axis.
            }
        }
        else
        {
            //Floor is flat or get down.
            double oldY = object->GetY();
            object->SetY(object->GetY()+1);
            int step = 0;
            bool noMoreOnFloor = false;
            while ( !IsCollidingWith(potentialObjects) )
            {
                if ( step > std::abs(requestedDeltaX*slopeClimbingFactor) ) //Slope is too step ( > 50% )
                {
                    noMoreOnFloor = true;
                    break;
                }

                //Object was on floor, but no more: Maybe a slope, try to follow it.
                object->SetY(object->GetY()+1);
                step++;
            }
            if ( noMoreOnFloor )
                object->SetY(oldY); //Unable to follow the floor: Go back to the original position. Fall will be triggered next tick.
            else
                object->SetY(object->GetY()-1); //Floor touched: Go back 1 pixel over.
        }

    }

    //Move the object on Y axis
    if ( requestedDeltaY != 0 )
    {
        double oldY = object->GetY();
        object->SetY(object->GetY()+requestedDeltaY);

        //Stop when colliding with an obstacle.
        while (  (requestedDeltaY < 0 && IsCollidingWith(potentialObjects, NULL, /*excludeJumpThrus=*/true)) //Jumpthru = obstacle <=> Never when going up
              || (requestedDeltaY > 0 && IsCollidingWith(potentialObjects, overlappedJumpThru)) ) //Jumpthru = obstacle <=> Only if not already overlapped when goign down
        {
            jumping = false;
            currentJumpSpeed = 0;
            if ( (requestedDeltaY > 0 && object->GetY() <= oldY) ||
                 (requestedDeltaY < 0 && object->GetY() >= oldY) )
            {
                object->SetY(oldY); //Unable to move the object without being stuck in an obstacle.
                break;
            }

            object->SetY(floor(object->GetY())+(requestedDeltaY > 0 ? -1 : 1));
        }

    }

    //3) Update the current floor data for the next tick:
    overlappedJumpThru = GetJumpthruCollidingWith(potentialObjects);
    if ( !isOnLadder )
    {
        //Check if the object is on a floor:
        //In priority, check if the last floor platform is still the floor.
        double oldY = object->GetY();
        object->SetY(object->GetY()+1);
        if ( isOnFloor && object->IsCollidingWith(floorPlatform->GetObject() ) )
        {
            //Still on the same floor
            floorLastX = floorPlatform->GetObject()->GetX();
            floorLastY = floorPlatform->GetObject()->GetY();
        }
        else
        {
            //Check if landing on a new floor: (Exclude already overlapped jump truh)
            std::set<PlatformBehavior*> collidingObjects = GetPlatformsCollidingWith(potentialObjects, overlappedJumpThru);
            if ( !collidingObjects.empty() ) //Just landed on floor
            {
                isOnFloor = true;
                canJump = true;
                jumping = false;
                currentJumpSpeed = 0;
                floorPlatform = *collidingObjects.begin();
                floorLastX = floorPlatform->GetObject()->GetX();
                floorLastY = floorPlatform->GetObject()->GetY();
                currentFallSpeed = 0;
            }
            else //In the air
            {
                canJump = false;
                isOnFloor = false;
                floorPlatform = NULL;
            }
        }
        object->SetY(oldY);
    }

    //4) Do not forget to reset pressed keys
    leftKey = false;
    rightKey = false;
    ladderKey = false;
    upKey = false;
    downKey = false;
    jumpKey = false;

    //5) Track the movement
    hasReallyMoved = std::abs(object->GetX()-oldX) >= 1;
}

bool PlatformerObjectBehavior::SeparateFromPlatforms(const std::set<PlatformBehavior*> & candidates, bool excludeJumpThrus)
{
    std::vector<RuntimeObject*> objects;
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( (*it)->GetPlatformType() == PlatformBehavior::Ladder ) continue;
        if ( excludeJumpThrus && (*it)->GetPlatformType() == PlatformBehavior::Jumpthru ) continue;

        objects.push_back((*it)->GetObject());
    }

    return object->SeparateFromObjects(objects);
}

std::set<PlatformBehavior*> PlatformerObjectBehavior::GetPlatformsCollidingWith(const std::set<PlatformBehavior*> & candidates,
    const std::set<PlatformBehavior*> & exceptTheseOnes)
{
    //TODO: This function could be refactored to return only the first colliding platform.
    std::set<PlatformBehavior*> result;
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( exceptTheseOnes.find(*it) != exceptTheseOnes.end() ) continue;
        if ( (*it)->GetPlatformType() == PlatformBehavior::Ladder ) continue;

        if ( object->IsCollidingWith((*it)->GetObject()) )
            result.insert(*it);
    }

    return result;
}

bool PlatformerObjectBehavior::IsCollidingWith(const std::set<PlatformBehavior*> & candidates,
    PlatformBehavior * exceptThisOne, bool excludeJumpThrus)
{
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( *it == exceptThisOne ) continue;
        if ( (*it)->GetPlatformType() == PlatformBehavior::Ladder ) continue;
        if ( excludeJumpThrus && (*it)->GetPlatformType() == PlatformBehavior::Jumpthru ) continue;

        if ( object->IsCollidingWith((*it)->GetObject()) )
            return true;
    }

    return false;
}

bool PlatformerObjectBehavior::IsCollidingWith(const std::set<PlatformBehavior*> & candidates,
    const std::set<PlatformBehavior*> & exceptTheseOnes)
{
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( exceptTheseOnes.find(*it) != exceptTheseOnes.end() ) continue;
        if ( (*it)->GetPlatformType() == PlatformBehavior::Ladder ) continue;

        if ( object->IsCollidingWith((*it)->GetObject()) )
            return true;
    }

    return false;
}

std::set<PlatformBehavior*> PlatformerObjectBehavior::GetJumpthruCollidingWith(const std::set<PlatformBehavior*> & candidates)
{
    std::set<PlatformBehavior*> result;
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( (*it)->GetPlatformType() != PlatformBehavior::Jumpthru ) continue;

        if ( object->IsCollidingWith((*it)->GetObject()) )
            result.insert(*it);
    }

    return result;
}

bool PlatformerObjectBehavior::IsOverlappingLadder(const std::set<PlatformBehavior*> & candidates)
{
    for (std::set<PlatformBehavior*>::iterator it = candidates.begin();
         it != candidates.end();
         ++it)
    {
        if ( (*it)->GetPlatformType() != PlatformBehavior::Ladder ) continue;
        if ( object->IsCollidingWith((*it)->GetObject()) )
            return true;
    }

    return false;
}

std::set<PlatformBehavior*> PlatformerObjectBehavior::GetPotentialCollidingObjects(double maxMovementLength)
{
    //Compute the "bounding circle" radius of the object.
    float o1w = object->GetWidth();
    float o1h = object->GetHeight();
    float obj1BoundingRadius = sqrt(o1w*o1w+o1h*o1h)/2.0+maxMovementLength/2.0; //Add to it the maximum magnitude of movement.


    std::set<PlatformBehavior*> potentialObjects;
    for (std::set<PlatformBehavior*>::iterator it = sceneManager->GetAllPlatforms().begin();
         it != sceneManager->GetAllPlatforms().end();
         ++it)
    {
        //First check if bounding circle are too far.
        RuntimeObject * obj2 = (*it)->GetObject();
        float o2w = obj2->GetWidth();
        float o2h = obj2->GetHeight();

        float x = object->GetDrawableX()+object->GetCenterX()-(obj2->GetDrawableX()+obj2->GetCenterX());
        float y = object->GetDrawableY()+object->GetCenterY()-(obj2->GetDrawableY()+obj2->GetCenterY());
        float obj2BoundingRadius = sqrt(o2w*o2w+o2h*o2h)/2.0;

        if ( sqrt(x*x+y*y) <= obj1BoundingRadius + obj2BoundingRadius ) {
            potentialObjects.insert(*it);
        }
    }

    return potentialObjects;
}

void PlatformerObjectBehavior::DoStepPostEvents(RuntimeScene & scene)
{
    if ( parentScene != &scene ) //Parent scene has changed
    {
        parentScene = &scene;
        sceneManager = parentScene ? &ScenePlatformObjectsManager::managers[&scene] : NULL;
        floorPlatform = NULL;
    }
}

void PlatformerObjectBehavior::SimulateControl(const gd::String & input)
{
    if ( input == "Left" ) leftKey = true;
    else if ( input == "Right" ) rightKey = true;
    else if ( input == "Up" ) upKey = true;
    else if ( input == "Down" ) downKey = true;
    else if ( input == "Ladder" ) ladderKey = true;
    else if ( input == "Jump" ) jumpKey = true;
}

void PlatformerObjectBehavior::UnserializeFrom(const gd::SerializerElement & element)
{
    gravity = element.GetDoubleAttribute("gravity");
    maxFallingSpeed = element.GetDoubleAttribute("maxFallingSpeed");
    acceleration = element.GetDoubleAttribute("acceleration");
    deceleration = element.GetDoubleAttribute("deceleration");
    maxSpeed = element.GetDoubleAttribute("maxSpeed");
    jumpSpeed = element.GetDoubleAttribute("jumpSpeed");
    ignoreDefaultControls = element.GetBoolAttribute("ignoreDefaultControls");
    SetSlopeMaxAngle(element.GetDoubleAttribute("slopeMaxAngle"));
}

#if defined(GD_IDE_ONLY)
void PlatformerObjectBehavior::SerializeTo(gd::SerializerElement & element) const
{
    element.SetAttribute("gravity", gravity);
    element.SetAttribute("maxFallingSpeed", maxFallingSpeed);
    element.SetAttribute("acceleration", acceleration);
    element.SetAttribute("deceleration", deceleration);
    element.SetAttribute("maxSpeed", maxSpeed);
    element.SetAttribute("jumpSpeed", jumpSpeed);
    element.SetAttribute("ignoreDefaultControls", ignoreDefaultControls);
    element.SetAttribute("slopeMaxAngle", slopeMaxAngle);

}

std::map<gd::String, gd::PropertyDescriptor> PlatformerObjectBehavior::GetProperties(gd::Project & project) const
{
    std::map<gd::String, gd::PropertyDescriptor> properties;

    properties[_("Gravity")].SetValue(gd::String::From(gravity));
    properties[_("Jump speed")].SetValue(gd::String::From(jumpSpeed));
    properties[_("Max. falling speed")].SetValue(gd::String::From(maxFallingSpeed));
    properties[_("Acceleration")].SetValue(gd::String::From(acceleration));
    properties[_("Deceleration")].SetValue(gd::String::From(deceleration));
    properties[_("Max. speed")].SetValue(gd::String::From(maxSpeed));
    properties[_("Default controls")].SetValue(ignoreDefaultControls ? "false" : "true").SetType("Boolean");
    properties[_("Slope max. angle")].SetValue(gd::String::From(slopeMaxAngle));

    return properties;
}

bool PlatformerObjectBehavior::UpdateProperty(const gd::String & name, const gd::String & value, gd::Project & project)
{
    if ( name == _("Default controls") ) {
        ignoreDefaultControls = (value == "0");
        return true;
    }

    if ( value.To<double>() < 0 ) return false;

    if ( name == _("Gravity") )
        gravity = value.To<double>();
    else if ( name == _("Max. falling speed") )
        maxFallingSpeed = value.To<double>();
    else if ( name == _("Acceleration") )
        acceleration = value.To<double>();
    else if ( name == _("Deceleration") )
        deceleration = value.To<double>();
    else if ( name == _("Max. speed") )
        maxSpeed = value.To<double>();
    else if ( name == _("Jump speed") )
        jumpSpeed = value.To<double>();
    else if ( name == _("Slope max. angle") )
        return SetSlopeMaxAngle(value.To<double>());
    else
        return false;

    return true;
}

#endif
