#pragma once
#include "IAgent.h"
#include <list>
#include "Vector2.h"
#include <cmath>

class RalstonAgent : public IAgent
{
	tankNet::TankBattleStateData current, previous;
	tankNet::TankBattleCommand action;
	tankNet::TankTacticalInfo Target;

	Vector2 moveTarget;
	Vector2 lookTarget;
	Vector2 pastLocation;
	Vector2 target;
	Vector2 LastLocation;
	float currentStandingStillTime;
	float maxStandingStillTime;

	enum Cannon {SCAN, TRACK, FIRE} turretstate = SCAN;
	//std::list<tankNet::TankBattleStateData> states;


	void scan()
	{
		Vector2 Caim = Vector2::fromXZ(current.cannonForward);
		Vector2 Cp = Vector2::fromXZ(current.position);

		action.fireWish = false;

		action.cannonMove = tankNet::CannonMovementOptions::LEFT;
		for (int enemy = 0; enemy < current.playerCount - 1; ++enemy) 
		{
			if (current.tacticoolData[enemy].inSight && current.canFire)
			{
				target = Vector2::fromXZ(current.tacticoolData[enemy].lastKnownPosition);

				if (dot(Caim, normal(target - Cp)) > .899f)
					turretstate = FIRE;
				break;
			}
		}
	}
	void track()
	{
		Vector2 Caim = Vector2::fromXZ(current.cannonForward);
		Vector2 Epos = Vector2::fromXZ(Target.lastKnownPosition);
		Vector2 Cp = Vector2::fromXZ(current.position);

		for (int enemy = 0; enemy < current.playerCount - 1; ++enemy)

		{
			if (current.tacticoolData[enemy].inSight)
			{
				Caim = Epos;
			}
			else
			{
				turretstate = SCAN;
			}

		}
	}
	void fire()
	{
		action.fireWish = current.canFire;
		turretstate = TRACK;
	}
		// auto reset timer 
		
		
	//Vector2 Taim = normal(target - Caim)
	//	if (dot(Caim, perp(Taim)) > 0)
	//		action.cannonMove = tankNet::CannonMovementOptions::RIGHT;	
	//	if (dot(Caim, perp(Taim)) < 0)
	//		action.cannonMove = tankNet::CannonMovementOptions::LEFT;
	//	if (dot(Caim, perp(Taim)) == 0)
	//		return distance(Caim, target);


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
		maxStandingStillTime = 2;
		moveTarget = Vector2::random() * Vector2 { 50, 50 };
		std::cout << "Target Acquired: " << moveTarget.x << " " << moveTarget.y << std::endl;

	
	}

	tankNet::TankBattleCommand update(tankNet::TankBattleStateData *state)
	{
		previous = current;
		current  = *state;
		float deltaTime = sfw::getDeltaTime();
		action.msg = tankNet::TankBattleMessage::GAME;

		switch (turretstate)
		{
		case FIRE:
			fire();
			break;
		case SCAN: 
			scan(); 
			break;
		case TRACK:
			track();
			break;
		}

	/*
		Analyze the state data and determine which actions you want.

	*/

		if (moveToward(moveTarget))
		{
			std::cout << "Target Reached!: " << moveTarget.x << " " << moveTarget.y << std::endl;
			moveTarget = Vector2::random() * Vector2 { 50, 50 };
			std::cout << "Target Acquired: " << moveTarget.x << " " << moveTarget.y << std::endl;
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
		
		/*if (abs(LastLocation.x - pastLocation.x) > (abs(pastLocation.x - cpos.x)) || (abs((LastLocation.y - pastLocation.y) > (abs(pastLocation.y - cpos.y)))))
		{
			

		}*/

		pastLocation = cpos;

		//action.cannonMove = tankNet::CannonMovementOptions::LEFT;
		//action.tankMove   = tankNet::TankMovementOptions::FWRD;
		//action.fireWish   = false;

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