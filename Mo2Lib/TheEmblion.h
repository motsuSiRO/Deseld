#pragma once
#include "Enemy.h"

enum THE_EMBLION_STATE
{
	TheE_IDLE = 0,
	TheE_APPROACH,
	TheE_SIDEMOVE,

	TheE_SPINATK,
	TheE_RAPID_FIRE,
	TheE_ENERGY_FIRE,
	TheE_FALL_FIRE,
};

//enum THE_EMBLION_LOWSTATE
//{
//	TheE_APPROACH = 0,
//	TheE_SIDEMOVE,
//
//	TheE_SPINATK,
//	TheE_RAPID_FIRE,
//	TheE_ENERGY_FIRE,
//	TheE_FALL_FIRE,
//
//};

class TheEmblion : public Enemy
{
public:
	void Init();
	void Update(float dt);
};