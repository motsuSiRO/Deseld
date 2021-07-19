#pragma once
#include "Mo2Model.h"
#include "ComponentObject.h"
#include "Shader.h"

class Stage01 : public Component
{
public: 
	Transform* trans;
public:
	void Start()override;
	void Update() override;
	void Draw() override;
	void ImGui()override;

private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
};

