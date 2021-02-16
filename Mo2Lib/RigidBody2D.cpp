#include "RigidBody2D.h"


void RigidBody2D::Update(float dt)
{
	AddForce(Mo2Lib::Float2(0.f, -gravity*mass));

	Integrate(dt);
}