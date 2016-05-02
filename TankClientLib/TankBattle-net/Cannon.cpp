#include "TankBattleNet.h"
#include "TankBattleHeaders.h"

#include <cassert>
#include <ostream>
#include <iostream>

#include <vector>


#include "dyad.h"


/*
namespace StupidTank
{

	tankNet::TankTacticalInfo current, previous;
	tankNet::TankBattleCommand cmd;

	tankNet::TankBattleCommand aim(tankNet::TankTacticalInfo state)
	{
		previous = current;
		current = state;
		// cmd;

		
		if (state.inSight = false)
		{
			cmd.cannonMove = tankNet::CannonMovementOptions::LEFT;
		}
		else if (state.inSight = true)
		{
			cmd.cannonMove = 
		}

		return cmd;
	}
}


/*
Agent:


	Percept Data -> Agent -> Actuation Data


	Percept:
		TankBattleStateData
		-> position, forward, cannonForward
		-> If we see enemies, etec.

*/