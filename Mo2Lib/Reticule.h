#pragma once
#include "GeoPrimitive.h"
#include "Sprite.h"
#include <memory>

class Reticule
{
public:
	enum DIRECTIONAL_OFFSET
	{
		HORIZONTAL = 0,
		VERTICAL,
		MAX_,//–½–¼Žv‚¢‚Â‚©‚ñ
	};

	Mo2Lib::Float3 center;
	Mo2Lib::Float3 direction[MAX_];
	float offset[MAX_] = {};//0 : horizontal  1 : vertical
	Mo2Lib::Float4 rota[MAX_];

	Reticule()
	{
		center = { 0.f, 0.f, 0.f };
		direction[HORIZONTAL] = { 1.f, 0.f, 0.f };
		direction[VERTICAL] = { 0.f, 1.f, 0.f };
		offset[HORIZONTAL] = 8.f;
		offset[VERTICAL] = 8.f;
	}

	void Initialize();
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context,
		const DirectX::XMFLOAT4X4& view_projection);

	void ImGui();

private:
	std::unique_ptr<GeoLine> line;
	std::unique_ptr<Sprite> line_tex;
	std::unique_ptr<Sprite> center_pt;
};