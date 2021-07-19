#include "Physics.h"
#include "Mo2GUI.h"
#include "framework.h"

const float EARTH_S_GRAVITY = 9.8f;
float Physics::GRAVITY = EARTH_S_GRAVITY;


void Physics::PhysicsInit(float m)
{
	mass = m;
	ragidbody = false;
	position = { 0.f, 0.f, 0.f };
	velocity = { 0.f, 0.f, 0.f };
	acceleration = { 0.f, 0.f, 0.f };
	resultant = { 0.f, 0.f, 0.f };
	g_vec = { 0.f, -1.f, 0.f };
	MAX_MOVE_SPEED = 1500.f;
	MAX_VELOCITY = 500.f;
	dynamic_friction = 10.f;
	g_scale = 1.f;
	MAX_GRAVITY = 1000.f;

}

void Physics::Integrate(float dt)
{
	assert(mass > 0);

	acceleration = (resultant / mass);

	velocity += acceleration * dt;

	if (ragidbody)
	{
		Mo2Lib::Float2 flat = { velocity.x, velocity.z };
		flat.Limit(MAX_VELOCITY);
		velocity.x = flat.x;
		velocity.z = flat.y;
		if (velocity.y < -MAX_GRAVITY)
		{
			velocity.y = -MAX_GRAVITY;
		}
	}
	else
	{
		velocity.Limit(MAX_VELOCITY);
	}

	position += velocity * dt;


	//if(resultant == 0.f)velocity += velocity.Reverse() * drag;

	resultant = { 0.f, 0.f, 0.f };
}

void Physics::AddAccelertion(const Mo2Lib::Vec3& a)
{
	acceleration += a;
}

void Physics::AddVelocity(const Mo2Lib::Vec3& v)
{
	velocity += v;
}

void Physics::AddForce(const Mo2Lib::Vec3& force)
{
	resultant += force;
}

void Physics::Deceleration()
{
	Mo2Lib::Vec3 v = { velocity.Reverse().x , 0.f, velocity.Reverse().z };
	AddForce(v * dynamic_friction * mass);
}

void Physics::AddGravity()
{
	float result = Physics::GRAVITY * mass * g_scale;
	AddForce(g_vec * result);
}

void Physics::PhysicsImGui(std::string s)
{

	//ImGui::Begin(str.c_str());

	std::string str = "Pysics##" + s;
	if (ImGui::CollapsingHeader(str.c_str()))
	{
		//ImGui::LabelText("Pysics##", s.c_str());

		str = "Position##" + s;
		ImGui::Text(str.c_str());
		ImGui::DragFloat3(str.c_str(), &position.x, 0.01f);
		//ImGui::Text("x : %.3f y : %.3f z : %.3f", position.x, position.y, position.z);

		str = "Acceleration##" + s;
		ImGui::Text(str.c_str());
		ImGui::Text("x : %.3f y : %.3f z : %.3f", acceleration.x, acceleration.y, acceleration.z);

		str = "Acceleration_Scale##" + s;
		ImGui::Text(str.c_str());
		ImGui::Text("%.3f", acceleration.Length());

		str = "Velocity##" + s;
		ImGui::Text(str.c_str());
		ImGui::Text("x : %.3f y : %.3f z : %.3f", velocity.x, velocity.y, velocity.z);

		str = "Velocity_Scale##" + s;
		ImGui::Text(str.c_str());
		ImGui::Text("%.3f", velocity.Length());


		str = "Mass##" + s;
		ImGui::DragFloat(str.c_str(), &mass, 0.1f);
		str = "Drag##" + s;
		ImGui::DragFloat(str.c_str(), &dynamic_friction, 0.01f);
		str = "MAX_MOVE_SPEED##" + s;
		ImGui::DragFloat(str.c_str(), &MAX_MOVE_SPEED, 1.f);
		str = "MAX_VELOCITY##" + s;
		ImGui::DragFloat(str.c_str(), &MAX_VELOCITY, 1.f);

		str = "WORLD_GRAVITY##" + s;
		ImGui::DragFloat(str.c_str(), &GRAVITY, 0.01f);
		str = "MAX_GRAVITY##" + s;
		ImGui::DragFloat(str.c_str(), &MAX_GRAVITY, 1.f);

		str = "RagidBody##" + s;
		ImGui::Checkbox(str.c_str(), &ragidbody);

	}
}


void Physics2::Start()
{
	position = &parent->GetComponent<Transform>()->translate;

	mass = 1.f;
	limit_velocity = false;

	velocity = { 0.f, 0.f, 0.f };
	acceleration = { 0.f, 0.f, 0.f };
	resultant = { 0.f, 0.f, 0.f };
}

void Physics2::Update()
{
	acceleration = (resultant / mass);

	velocity += acceleration * Mo2System->delta_time;

	if (limit_velocity)
	{
		velocity.Limit(MAX_VELOCITY);
	}

	*position += velocity * Mo2System->delta_time;

	resultant = { 0.f, 0.f, 0.f };
}

void Physics2::AddForce(const Mo2Lib::Vec3& vec)
{
	resultant += vec;
}

void Physics2::AddVelocity(const Mo2Lib::Vec3& vec)
{
	velocity += vec;
}

void Physics2::AddAcceleration(const Mo2Lib::Vec3& vec)
{
	acceleration += vec;
}

void Physics2::Deceleration()
{
	Mo2Lib::Vec3 v = { velocity.Reverse().x , 0.f, velocity.Reverse().z };
	resultant += v * dynamic_friction * mass;
}

void Physics2::ImGui()
{
	std::string str;
	std::string physics = "Physics" + parent->GetID();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader(physics.c_str()))
	{
		str = "Acceleration##" + physics;
		ImGui::Text(str.c_str());
		ImGui::Text("x : %.3f y : %.3f z : %.3f", acceleration.x, acceleration.y, acceleration.z);

		str = "Acceleration_Scale##" + physics;
		ImGui::Text(str.c_str());
		ImGui::Text("%.3f", acceleration.Length());

		str = "Velocity##" + physics;
		ImGui::Text(str.c_str());
		ImGui::Text("x : %.3f y : %.3f z : %.3f", velocity.x, velocity.y, velocity.z);

		str = "Velocity_Scale##" + physics;
		ImGui::Text(str.c_str());
		ImGui::Text("%.3f", velocity.Length());


		str = "Mass##" + physics;
		ImGui::DragFloat(str.c_str(), &mass, 0.1f);
		str = "Drag##" + physics;
		ImGui::DragFloat(str.c_str(), &dynamic_friction, 0.01f);
		str = "MAX_MOVE_SPEED##" + physics;
		ImGui::DragFloat(str.c_str(), &MAX_MOVE_SPEED, 1.f);
		str = "MAX_VELOCITY##" + physics;
		ImGui::DragFloat(str.c_str(), &MAX_VELOCITY, 1.f);


	}
}

