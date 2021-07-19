#pragma once
#include <list>
#include "Vector3D.h"
#include "GameEntity.h"
#include "Mo2GUI.h"

class Object;

class Component
{
public:
	bool is_visible;
public:
	Component() :is_visible(true) {}
	virtual ~Component() {}
	Object* parent;
	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void ImGui();

	using Super = Component;
};

class Object : public GameEntity
{
public:
	char tag[128] = "GameObject";
	float delta_time;
	bool destroy;

private:
	bool active;

public:
	Object() : GameEntity(), active(true) {}
	Object(char* _tag) : GameEntity(), active(true)
	{
		strcpy_s(tag, 128, _tag);
		destroy = false;
	}
	~Object() {
		for (auto com : component_list)
			delete com;
	}

	std::list<Component*> component_list;

	void Start();
	void Update(float elapsed);
	void Draw();

	bool selected = false;
	void ImGui();

	//オブジェクトが持っているコンポーネントを取得
	template<class T>
	T* GetComponent()
	{
		for (auto com : component_list) {
			T* buff = dynamic_cast<T*>(com);
			if (buff != nullptr)
				return buff;
		}
		return nullptr;
	}

	//オブジェクトが持っているコンポーネントを追加
	template<class T>
	T* AddComponent()
	{
		T* buff = new T();
		buff->parent = this;
		component_list.push_back(buff);
		buff->Start();
		return buff;
	}

	void Destroy();
};

class Transform : public Component
{
public:
	Mo2Lib::Float3 translate;
	Mo2Lib::Float3 rotate;
	Mo2Lib::Float3 scale;

	Mo2Lib::Float4 GetQuaternion()
	{
		Mo2Lib::Float4 orient = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotate.ConvertToXMVECTOR());
		return orient;
	}

	Mo2Lib::Float4 GetFixedQuaternion()
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, 0.f, rotate.z);
		DirectX::XMFLOAT4X4 f4x4;
		DirectX::XMStoreFloat4x4(&f4x4, m);
		Mo2Lib::Vec3 up = { f4x4._21, f4x4._22, f4x4._23 };
		Mo2Lib::Float4 orient = DirectX::XMQuaternionRotationMatrix(m);
		orient *= DirectX::XMQuaternionRotationAxis(up.ConvertToXMVECTOR(), rotate.y);
		return orient;
	}

	void Start()
	{
		translate = { 0.f,0.f,0.f };
		rotate = { 0.f,0.f,0.f };
		scale = { 1.f,1.f,1.f };
	}

	void Update()
	{

	}

	void ImGui()
	{
		std::string str, s;
		s = "##" + parent->GetID();
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
		str = "Transform" + s;
		if (ImGui::CollapsingHeader(str.c_str()))
		{
			str = "Translate" + s;
			ImGui::DragFloat3(str.c_str(), &translate.x, 0.01f);
			str = "Rotate" + s;
			rotate = { DirectX::XMConvertToDegrees(rotate.x), DirectX::XMConvertToDegrees(rotate.y), DirectX::XMConvertToDegrees(rotate.z) };
			ImGui::DragFloat3(str.c_str(), &rotate.x, 0.1f);
			rotate = { DirectX::XMConvertToRadians(rotate.x), DirectX::XMConvertToRadians(rotate.y), DirectX::XMConvertToRadians(rotate.z) };

			str = "Scale" + s;
			ImGui::DragFloat3(str.c_str(), &scale.x, 0.01f);
		}

	}
};