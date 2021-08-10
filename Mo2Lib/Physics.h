#pragma once
#include "Vector3D.h"
#include "cereal/cereal.hpp"
#include "ComponentObject.h"

extern const float EARTH_S_GRAVITY;

class Physics
{
public:
	float mass;
	bool ragidbody;
protected:
	Mo2Lib::Float3 position;
	Mo2Lib::Vec3 velocity;
	Mo2Lib::Vec3 acceleration;
	Mo2Lib::Vec3 resultant;
	Mo2Lib::Vec3 g_vec;	//gravity vector
	static float GRAVITY;	//world's gravity
	float g_scale;			//gravity scale
	float dynamic_friction;

	float MAX_MOVE_SPEED;
	float MAX_VELOCITY;
	float MAX_GRAVITY;

public:
	Physics() : mass(FLT_MAX),
		ragidbody(false),
		position(0.f, 0.f, 0.f),
		velocity(0.f, 0.f, 0.f),
		acceleration(0.f, 0.f, 0.f),
		resultant(0.f, 0.f, 0.f),
		g_vec(0.f, -1.f, 0.f),
		MAX_MOVE_SPEED(1500.f),
		MAX_VELOCITY(500.f),
		dynamic_friction(10.f),
		g_scale(1.f),
		MAX_GRAVITY(1000.f){}

	virtual ~Physics() {}

	void PhysicsInit(float mass);

	void Integrate(float dt);

	void PhysicsImGui(std::string str);

	void AddAccelertion(const Mo2Lib::Vec3& accel);

	void AddVelocity(const Mo2Lib::Vec3& velocity);

	void AddForce(const Mo2Lib::Vec3& force);

	void Deceleration();

	void AddGravity();

	const Mo2Lib::Vec3& GetAcceleration() const
	{
		return acceleration;
	}

	const Mo2Lib::Vec3& GetVelocity() const
	{
		return velocity;
	}

	const Mo2Lib::Float3& GetPos() const
	{
		return position;
	}
	const float GetFriction() const
	{
		return dynamic_friction;
	}

	void SetAcceleration(Mo2Lib::Vec3 a) { acceleration = a; }
	void SetVelocity(Mo2Lib::Vec3 v) { velocity = v; }
	void SetPos(Mo2Lib::Vec3 p) { position = p; }
	void SetMAX_MOVE_SPEED(float mms) { MAX_MOVE_SPEED = mms; }
	void SetMAX_VELOCITY(float v) { MAX_VELOCITY = v; }
	void SetDrag(float d) { dynamic_friction = d; }
	void SetG_Scale(float g) { g_scale = g; }
	void SetMAX_GRAVITY(float g) { MAX_GRAVITY = g; }
	static void SetWORLD_GRAVITY(float g) { GRAVITY = g; }

	const float& GetMAX_MOVE_SPEED() { return MAX_MOVE_SPEED; }
	const float& GetMAX_VELOCITY() { return MAX_VELOCITY; }

};

class Physics2 : public Component
{
public:
	Mo2Lib::Float3* position;
	Mo2Lib::Vec3 velocity;
	Mo2Lib::Vec3 acceleration;
	Mo2Lib::Vec3 resultant;

	bool limit_velocity;
	float mass;
	float dynamic_friction;
	float move_speed;
	float MAX_MOVE_SPEED;
	float MAX_VELOCITY;


	void Start();
	void Update();
	void ImGui();

	void AddAcceleration(const Mo2Lib::Vec3& accel);

	void AddVelocity(const Mo2Lib::Vec3& velocity);

	void AddForce(const Mo2Lib::Vec3& force);

	void AddMoveSpeed(const Mo2Lib::Vec3& force);
	void Deceleration();

};

class Physics2D
{
public:
	float mass;

protected:
	Mo2Lib::Float2 position;
	Mo2Lib::Float2 velocity;
	Mo2Lib::Float2 acceleration;
	Mo2Lib::Float2 resultant;

	float dynamic_friction;
	float MAX_MOVE_SPEED;
	float MAX_VELOCITY;


public:
	Physics2D() : mass(FLT_MAX),
		position(0.f, 0.f),
		velocity(0.f, 0.f),
		acceleration(0.f, 0.f),
		resultant(0.f, 0.f),
		MAX_MOVE_SPEED(1500.f),
		MAX_VELOCITY(500.f),
		dynamic_friction(10.f) {}

	virtual ~Physics2D() {}

	void PhysicsInit(float mass);

	void Integrate(float dt);

	void PhysicsImGui(std::string str);

	void AddForce(const Mo2Lib::Float2& force);

	void Deceleration();

	const Mo2Lib::Float2& GetAcceleration() const
	{
		return acceleration;
	}
	const Mo2Lib::Float2& GetVelocity() const
	{
		return velocity;
	}
	const Mo2Lib::Float2& GetPos() const
	{
		return position;
	}
	const float GetFriction() const
	{
		return dynamic_friction;
	}

	void SetAcceleration(Mo2Lib::Float2 a) { acceleration = a; }
	void SetVelocity(Mo2Lib::Float2 v) { velocity = v; }
	void SetPos(Mo2Lib::Float2 p) { position = p; }
	void SetMAX_MOVE_SPEED(float mms) { MAX_MOVE_SPEED = mms; }
	void SetMAX_VELOCITY(float v) { MAX_VELOCITY = v; }
	void SetDrag(float d) { dynamic_friction = d; }

	const float& GetMAX_MOVE_SPEED() { return MAX_MOVE_SPEED; }
	const float& GetMAX_VELOCITY() { return MAX_VELOCITY; }

};


class Spring
{
	float equilibrium;
	float constant;
};