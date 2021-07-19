#pragma once
#include "ComponentObject.h"
#include "Mo2Cerealize.h"
#include "Mo2Model.h"
#include "Shader.h"

struct Magagine
{
	int remaining_bullets;
	int MAX_SIZE;

	//Magagine(int size) : remaining_bullets(size),
	//	MAX_SIZE(size) {}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(MAX_SIZE));
	}
};


class Firearm : public Component
{
public:
	bool cooling;
	float recast_time;

	float vertical_recoil;
	float v_recoil_control;
	float horizontal_recoil;
	float h_recoil_control;

	Magagine mag;
	Transform* trans;

	void Start();
	void Update();
	void Draw();
	void ImGui();

	bool Shoot();

private:
	float MAX_RECAST_TIME;

	float MAX_V_RECOIL;
	float ONCE_V_RECOIL;
	float RC_V_POWER;
	float MAX_H_RECOIL;
	float ONCE_H_RECOIL;
	float RC_H_POWER;

	int MAX_MAGAGINE;

	void Cooling();

	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(MAX_RECAST_TIME),
			CEREAL_NVP(MAX_V_RECOIL),
			CEREAL_NVP(ONCE_V_RECOIL),
			CEREAL_NVP(RC_V_POWER),
			CEREAL_NVP(MAX_H_RECOIL),
			CEREAL_NVP(ONCE_H_RECOIL),
			CEREAL_NVP(RC_H_POWER),
			CEREAL_NVP(mag),
			CEREAL_NVP(MAX_MAGAGINE)
		);
	}

};