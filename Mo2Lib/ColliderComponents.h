#pragma once
#include "ComponentObject.h"
#include "GeoPrimitive.h"

class ColliderComponent : public Component
{
protected:
	std::unique_ptr<GeoPrimitive> primitive;
public:
	Mo2Lib::Transform trans;
public:
	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void ImGui() {}

	virtual Mo2Lib::Float3 GetDimension() const{}

protected:
	bool first_hit = true;
public:
	void (*OnHit)();
	void (*OnTrigger)();
};


class BoxComponent : public ColliderComponent
{
public:
	void Start();
	void Update();
	void Draw();
	void ImGui();

	Mo2Lib::Float3 GetDimension() const
	{
		return Mo2Lib::Float3(trans.scale / 2);
	}
};

class SphereComponent : public ColliderComponent
{
public:
	float r;
public:
	void Start();
	void Update();
	void Draw();
	void ImGui();

	Mo2Lib::Float3 GetDimension() const
	{
		return Mo2Lib::Float3(r, r, r);
	}
};