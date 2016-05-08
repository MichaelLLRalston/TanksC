#pragma once
#include "IAgent.h"
#include <list>
#include "Vector2.h"

class RalstonAgent : public IAgent
{
	tankNet::TankBattleStateData current, previous;
	tankNet::TankBattleCommand action;

	Vector2 moveTarget;
	Vector2 lookTarget;
	Vector2 pastLocation;
	float currentStandingStillTime;
	float maxStandingStillTime;
	//std::list<tankNet::TankBattleStateData> states;

	// aim the cannon at the target
	bool lookToward(const Vector2 &target)
	{
		Vector2 Caim = Vector2::fromXZ(current.cannonForward);

		Vector2 Taim = normal(target - Caim);

		if (dot(Caim, perp(Taim)) > 0)
			action.cannonMove = tankNet::CannonMovementOptions::RIGHT;
		if (dot(Caim, perp(Taim)) < 0)
			action.cannonMove = tankNet::CannonMovementOptions::LEFT;
		if (dot(Caim, perp(Taim)) == 0)
			return distance(Caim, target);
	}

	// Find a good value for tank Move
	bool moveToward(const Vector2 &target)
	{
		/*
			current Position
			current Forward

			target  Position
			Desired Forward
		*/

		Vector2 cpos = Vector2::fromXZ(current.position);
		Vector2 cfow = Vector2::fromXZ(current.forward);

		// Desired Forward = Normalization of the difference in position
		Vector2 tfow = normal(target-cpos);

		// Line Up with the Target (Turn left or Right)

		if(dot(cfow,perp(tfow)) > 0)
			action.tankMove = tankNet::TankMovementOptions::RIGHT;

		if (dot(cfow, perp(tfow)) < 0)
			action.tankMove = tankNet::TankMovementOptions::LEFT;

		// Move Toward the Target (Move forward or backwards towards target)
		if (dot(cfow, tfow) >  cos(PI / 12.f))
		{
			action.tankMove = tankNet::TankMovementOptions::FWRD;
		}
		if (dot(cfow, tfow) < -cos(PI / 12.f))
			action.tankMove = tankNet::TankMovementOptions::BACK;

		return distance(cpos, target) < 10;
	}

public:
	RalstonAgent()
	{
		currentStandingStillTime = 0;
		maxStandingStillTime = 1;
		moveTarget = Vector2::random() * Vector2 { 50, 50 };
		std::cout << "Target Acquired: " << moveTarget.x << " " << moveTarget.y << std::endl;

		lookTarget = Vector2::random() * Vector2 { 50, 50 };
		std::cout << "Target Spotted: " << lookTarget.x << " " << lookTarget.y << std::endl;
	}

	tankNet::TankBattleCommand update(tankNet::TankBattleStateData *state)
	{
		previous = current;
		current  = *state;
		float deltaTime = sfw::getDeltaTime();

	/*
		Analyze the state data and determine which actions you want.

	*/

		if (moveToward(moveTarget))
		{
			std::cout << "Target Reached!: " << moveTarget.x << " " << moveTarget.y << std::endl;
			moveTarget = Vector2::random() * Vector2 { 50, 50 };
			std::cout << "Target Acquired: " << moveTarget.x << " " << moveTarget.y << std::endl;
		}
		if (lookToward(lookTarget))
		{
			std::cout << "Target Spotted: " << lookTarget.x << " " << lookTarget.y << std::endl;
			lookTarget = Vector2::random() * Vector2 { 50, 50 };
			std::cout << "Target Visualized: " << lookTarget.x << " " << lookTarget.y << std::endl;
		}

		const Vector2 cpos = Vector2::fromXZ(current.position);

		if (pastLocation == cpos)
		{
			currentStandingStillTime += deltaTime;
			if (currentStandingStillTime >= maxStandingStillTime)
			{
				moveTarget = Vector2::random() * Vector2 { 50, 50 };
			}
		}
		else
		{
			currentStandingStillTime = 0;
		}

		pastLocation = cpos;

		//action.cannonMove = tankNet::CannonMovementOptions::LEFT;
		//action.tankMove   = tankNet::TankMovementOptions::FWRD;
		action.fireWish   = false;

		return action;
	}
};

// Percepts -> Agent -> Actuates

/*
	percepts -> Agent
					-> Analysis <- Find where we want to move toward
					-> Behavior <- develop logic to move towards a target
					-> Action

*/