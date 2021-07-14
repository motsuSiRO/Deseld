#pragma once

#include "Physics.h"

class RigidBody2D : public Physics2D
{
private:
	float gravity = 9.8f;
public:
	RigidBody2D() : Physics2D() {};
	virtual ~RigidBody2D() = default;

	void Update(float elapsed_time);

	void SetGravity(float g) { gravity = g; }
};