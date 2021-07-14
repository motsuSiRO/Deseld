#pragma once
#include <list>
#include "Vector3D.h"
#include "GameEntity.h"
#include "Mo2GUI.h"

class Object;

class Component
{
protected:
public:
	Component() {}
	virtual ~Component() {}
	Object* Parent;
	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void ImGui() {}
};

class Object : public GameEntity
{
public:
	char tag[128] = "GameObject";
	float delta_time;
	bool destroy;
	Object() : GameEntity() {}
	Object(char* _tag) : GameEntity()
	{
		strcpy_s(tag, 128, _tag);
		destroy = false;
	}
	~Object() {
		for (auto com : ComponentList)
			delete com;
	}

	std::list<Component*> ComponentList;

	void Update(float elapsed)
	{
		delta_time = elapsed;
		auto buff = ComponentList;
		for (auto com : buff)
		{
			com->Update();
		}
	}

	void Draw()
	{
		for (auto com : ComponentList)
		{
			com->Draw();
		}
	}

	bool selected = false;
	void ImGui()
	{
		if (selected)
		{
			ImGui::SetNextWindowPos(ImVec2(1420.f, 0.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(500.f, 1000.f), ImGuiCond_Once);
			char _tag[128];
			strcpy_s(_tag, 128, tag);
			std::string str = "##";
			str += tag;
			if (ImGui::Begin(tag))
			{
				if (ImGui::InputText(str.c_str(), _tag, size_t(128), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					strcpy_s(tag, 128, _tag);
				}

				for (auto& com : ComponentList)
				{
					com->ImGui();
				}
			}
			ImGui::End();
		}
	}

	//オブジェクトが持っているコンポーネントを取得
	template<class T>
	T* GetComponent()
	{
		for (auto com : ComponentList) {
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
		buff->Parent = this;
		ComponentList.push_back(buff);
		buff->Start();
		return buff;
	}
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
		s = "##" + Parent->GetID();
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