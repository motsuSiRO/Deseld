#pragma once
#include "Enemy.h"

enum GATESQUAD_STATE
{
	QUAD_IDLE = 0,
	QUAD_APPROACH,
	QUAD_SIDEMOVE,

	QUAD_BODYATK,
	QUAD_EXPLOSION,
};

class GatesQuad : public Enemy
{
public:
	void Init();
	void Update(float dt);

};


