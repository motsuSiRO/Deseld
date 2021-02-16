#include "Physics.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

void Physics2D::PhysicsInit(float m)
{
	mass = m;
	position = { 0.f,0.f };
	velocity = { 0.f,0.f };
	acceleration = { 0.f,0.f };
	resultant = { 0.f,0.f };
	MAX_MOVE_SPEED = 1500.f;
	MAX_VELOCITY = 200.f;
	dynamic_friction = 10.f;

}

void Physics2D::Integrate(float dt)
{
	assert(mass > 0);


	acceleration = (resultant / mass);

	velocity += acceleration * dt;

	velocity.Limit(MAX_VELOCITY);

	position += velocity * dt;


	//if(resultant == 0.f)velocity += velocity.Reverse() * drag;

	resultant = Mo2Lib::Float2(0.f, 0.f);

}


void Physics2D::AddForce(const Mo2Lib::Float2& force)
{
	resultant += force;
}

void Physics2D::Deceleration()
{
	AddForce(velocity.Reverse() * dynamic_friction);
}


void Physics2D::PhysicsImGui(std::string s)
{
	//std::string str = "Pysics" + s;
	//ImGui::Begin(str.c_str());

	ImGui::Separator();
	ImGui::LabelText("Pysics##", s.c_str());

	std::string str = "Position##" + s;
	ImGui::Text(str.c_str());
	ImGui::DragFloat3(str.c_str(), &position.x, 0.01f);
	//ImGui::Text("x : %.3f y : %.3f z : %.3f", position.x, position.y, position.z);

	str = "Velocity##" + s;
	ImGui::Text(str.c_str());
	ImGui::Text("x : %.3f y : %.3f ", velocity.x, velocity.y);

	str = "Acceleration##" + s;
	ImGui::Text(str.c_str());
	ImGui::Text("x : %.3f y : %.3f ", acceleration.x, acceleration.y);

	str = "Drag##" + s;
	ImGui::DragFloat(str.c_str(), &dynamic_friction, 0.01f);
	str = "MAX_MOVE_SPEED##" + s;
	ImGui::DragFloat(str.c_str(), &MAX_MOVE_SPEED, 1.f);
	str = "MAX_VELOCITY##" + s;
	ImGui::DragFloat(str.c_str(), &MAX_VELOCITY, 1.f);

}
