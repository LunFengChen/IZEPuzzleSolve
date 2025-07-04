﻿#include "PlantState.h"

std::string PlantState::ToString(PlantState state)
{
	switch (state)
	{
	case IDLE:
		return "IDLE";
	case WAIT:
		return "WAIT";
	case WORK:
		return "WORK";
	case SQUASH_LOOK:
		return "SQUASH_LOOK";
	case SQUASH_JUMP_UP:
		return "SQUASH_JUMP_UP";
	case SQUASH_STOP_IN_THE_AIR:
		return "SQUASH_STOP_IN_THE_AIR";
	case SQUASH_JUMP_DOWN:
		return "SQUASH_JUMP_DOWN";
	case SQUASH_CRUSHED:
		return "SQUASH_CRUSHED";
	case GRAVE_BUSTER_LAND:
		return "GRAVE_BUSTER_LAND";
	case GRAVE_BUSTER_IDLE:
		return "GRAVE_BUSTER_IDLE";
	case CHOMPER_BITE_BEGIN:
		return "CHOMPER_BITE_BEGIN";
	case CHOMPER_BITE_SUCCESS:
		return "CHOMPER_BITE_SUCCESS";
	case CHOMPER_BITE_FAIL:
		return "CHOMPER_BITE_FAIL";
	case CHOMPER_CHEW:
		return "CHOMPER_CHEW";
	case CHOMPER_SWALLOW:
		return "CHOMPER_SWALLOW";
	case POTATO_SPROUT_OUT:
		return "POTATO_SPROUT_OUT";
	case POTATO_ARMED:
		return "POTATO_ARMED";
	case SPIKE_ATTACK:
		return "SPIKE_ATTACK";
	case SCAREDYSHROOM_SCARED:
		return "SCAREDYSHROOM_SCARED";
	case SCAREDYSHROOM_SCARED_IDLE:
		return "SCAREDYSHROOM_SCARED_IDLE";
	case SCAREDYSHROOM_GROW:
		return "SCAREDYSHROOM_GROW";
	case SUNSHROOM_SMALL:
		return "SUNSHROOM_SMALL";
	case SUNSHROOM_GROW:
		return "SUNSHROOM_GROW";
	case SUNSHROOM_BIG:
		return "SUNSHROOM_BIG";
	case MAGNETSHROOM_WORKING:
		return "MAGNETSHROOM_WORKING";
	case MAGNETSHROOM_INACTIVE_IDLE:
		return "MAGNETSHROOM_INACTIVE_IDLE";
	case BOWLING_RISE:
		return "BOWLING_RISE";
	case BOWLING_FALL:
		return "BOWLING_FALL";
	case CACTUS_SHORT_IDLE:
		return "CACTUS_SHORT_IDLE";
	case CACTUS_GROW_TALL:
		return "CACTUS_GROW_TALL";
	case CACTUS_TALL_IDLE:
		return "CACTUS_TALL_IDLE";
	case CACTUS_GET_SHORT:
		return "CACTUS_GET_SHORT";
	case TANGLE_KELP_GRAB:
		return "TANGLE_KELP_GRAB";
	case COB_CANNON_UNARAMED_IDLE:
		return "COB_CANNON_UNARAMED_IDLE";
	case COB_CANNON_CHARGE:
		return "COB_CANNON_CHARGE";
	case COB_CANNON_LAUNCH:
		return "COB_CANNON_LAUNCH";
	case COB_CANNON_ARAMED_IDLE:
		return "COB_CANNON_ARAMED_IDLE";
	case KERNELPULT_LAUNCH_BUTTER:
		return "KERNELPULT_LAUNCH_BUTTER";
	case UMBRELLA_LEAF_BLOCK:
		return "UMBRELLA_LEAF_BLOCK";
	case UMBRELLA_LEAF_SHRINK:
		return "UMBRELLA_LEAF_SHRINK";
	case IMITATER_EXPLODE:
		return "IMITATER_EXPLODE";
	case FLOWER_POT_PLACED:
		return "FLOWER_POT_PLACED";
	case LILY_PAD_PLACED:
		return "LILY_PAD_PLACED";
	default:
		return"UNKNOW";
	}
}