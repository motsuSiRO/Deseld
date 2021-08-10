#pragma once
#include "ComponentObject.h"
#include "GeoPrimitive.h"
#include "Shader.h"

struct Contact
{

	//Contact() : point(0, 0, 0), normal(0, 0, 0), penetration(0), restitution(0)
	//{
	//	body[0] = body[1] = 0;
	//}

	//RigidBody* body[2];

	Mo2Lib::Float3 point;	//接触点
	//D3DXVECTOR3 normal; //剛体0(body[0])から見た接触面の法線
	FLOAT penetration;	//めり込み量
	//FLOAT restitution;	//反発係数
	//bool penetFlg;

	//void resolve();	//接触の解消

};


class ColliderComponent : public Component
{
protected:
	std::unique_ptr<GeoPrimitive> primitive;
	std::unique_ptr<ShaderEx> nolight;
public:
	Mo2Lib::Transform trans;
public:
	Contact contact;
	
	virtual void Start();
	virtual void Update() {}
	virtual void Draw();
	virtual void ImGui();
	virtual void OnHit() {}
	virtual void OnTrigger() {}

	virtual Mo2Lib::Float3 GetDimension() const { return Mo2Lib::Float3(0.f, 0.f, 0.f); }
	using Collider = ColliderComponent;

protected:
	bool first_hit = true;
};


class BoxComponent : public ColliderComponent
{
public:
	void Start();
	void Update();
	void Draw();
	void ImGui();
	void OnHit();
	void OnTrigger();

	Mo2Lib::Float3 GetDimension() const
	{
		return Mo2Lib::Float3(trans.scale/2);
	}
};

class SphereComponent : public ColliderComponent
{
private:
	float r;
public:

	void Start();
	void Update();
	void Draw();
	void ImGui();
	void OnHit();
	void OnTrigger();

	Mo2Lib::Float3 GetDimension() const
	{
		return Mo2Lib::Float3(r, r, r);
	}

	float GetRadius()
	{
		return trans.scale.Length() / 2;
	}
};