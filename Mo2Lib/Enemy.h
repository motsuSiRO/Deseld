#pragma once
#include "Vector3D.h"
#include "Physics.h"
#include "model.h"
#include <vector>
#include <memory>
#include <map>
#include "StateMachine.h"

class Enemy : public Physics
{
public:
	int type;
	Mo2Lib::Float4 rotation;
	Mo2Lib::Float3 scale;
	std::vector<Mo2Lib::Sphere> collider;
	bool visiblity = true;
	bool exist;
	Mo2Lib::Float3 target_pos;
	std::unique_ptr<StateMachine<Enemy>> fsm;


public:
	Enemy()
	{
		Init();
	}

	virtual void Init();
	virtual void Update(float dt);
	void UpdOrinetation(const Mo2Lib::Float3& vec);
	void SetAllHitPos(Mo2Lib::Float3 p);
//void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection,
	//	const DirectX::XMFLOAT4& light_dir, bool isSolid);
};


class EnemyManager
{
public:
	//std::vector<Enemy> enms;

	std::list<Enemy*> enm_list;
	Mo2Lib::Float3 target_pos;

	std::map<int, std::vector<Mo2Lib::Sphere>> colider_data;

	void Init();
	void Update(float dt);
	//void Render(ShaderEx* shader, ModelRenderer *renderer);
	void ImGui();
	void Release();
	bool Set(int type, Mo2Lib::Float3 pos);
	void SetTargetPos(Mo2Lib::Float3 o) { target_pos = o; };
	static EnemyManager* Getinstance()
	{
		static EnemyManager ins;
		return &ins;
	}


	enum MODEL_TYPE
	{
		FLOWER = 0,
		GATESQUAD,
		THE_EMBLION,

		MODEL_MAX,
	};

	//enum STATE_TYPE
	//{
	//	QUAD_IDLE = 0,
	//	QUAD_APPROACH,
	//	QUAD_SIDEMOVE,
	//	QUAD_BODYATK,
	//	QUAD_EXPLOSION,

	//	TheE_IDLE,
	//	TheE_APPROACH,
	//	TheE_SIDEMOVE,
	//	TheE_SPINATK,
	//	TheE_RAPID_FIRE,
	//	TheE_ENERGY_FIRE,
	//	TheE_FALL_FIRE,

	//	STATE_MAX,
	//};

	std::map<int, std::shared_ptr<State<Enemy>>> state_list;
private:

	std::map<int, std::shared_ptr<Mo2Lib::Model>> model;

	EnemyManager() {}
	~EnemyManager() { Release(); }

};

EnemyManager* EnmManager();


