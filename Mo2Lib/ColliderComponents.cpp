#include "ColliderComponents.h"
#include "model_renderer.h"
#include "GameSystem.h"
#include "Collision.h"

void ColliderComponent::Start()
{
	primitive = std::make_unique<GeoPrimitive>();
	nolight = std::make_unique<ShaderEx>();
	nolight->Create(L"NoLightObj_VS", L"NoLightObj_PS");
}

void BoxComponent::Start()
{
	Collider::Start();

	trans.scale = { 10.f, 10.f, 10.f };
	primitive->CreateCube();
}

void BoxComponent::Update()
{
	primitive->color = { 1.f,1.f,1.f,1.f };
	
	bool detect = false;
	for (auto obj : Mo2Lib::game.obj_list)
	{
		if (obj.get() == parent)
		{
			detect = true;
			continue;
		}

		if (detect)
		{
			for (auto com : obj->component_list)
			{
				//if (dynamic_cast<BoxComponent*>(com))
				//{
				//	break;
				//}
				SphereComponent* s = dynamic_cast<SphereComponent*>(com);
				if (s)
				{
					if (generate_contact_sphere_box(s, this))
					{
						for (auto it : this->parent->component_list)
						{
							it->OnHit();
						}
						for (auto it : s->parent->component_list)
						{
							it->OnHit();
						}

						break;
					}
				}
			}
		}
	}
}

void BoxComponent::Draw()
{
	primitive->SetTransform(trans);
	Mo2Render().Draw(nolight.get(), *primitive);
}

void BoxComponent::OnHit()
{
	primitive->color = { 1.f,1.f,0.f,1.f };
	parent->Destroy();
}

void BoxComponent::OnTrigger()
{

}

void BoxComponent::ImGui()
{

}

void SphereComponent::Start()
{
	Collider::Start();

	trans.scale = { 10.f, 10.f, 10.f };
	primitive->CreateSphere(16, 8);
}

void SphereComponent::Update()
{
	primitive->color = { 1.f,1.f,1.f,1.f };
	static int dir = 1;
	static float rrrr = 0.f;
	const float rad = 0.000025f;

	trans.translate.x = dir * 100.f * cosf(rrrr);

	rrrr += rad;
	dir *= -1;

	bool detect = false;
	for (auto obj : Mo2Lib::game.obj_list)
	{
		if (obj.get() == parent)
		{
			detect = true;
			continue;
		}

		if (detect)
		{
			for (auto com : obj->component_list)
			{
				if (dynamic_cast<BoxComponent*>(com))
				{
					BoxComponent* b = dynamic_cast<BoxComponent*>(com);
					if (b)
					{
						if (generate_contact_sphere_box(this, b))
						{
							for (auto it : this->parent->component_list)
							{
								it->OnHit();
							}
							for (auto it : b->parent->component_list)
							{
								it->OnHit();
							}
							break;
						}
					}
				}

				if (dynamic_cast<SphereComponent*>(com))
				{
					SphereComponent* s = dynamic_cast<SphereComponent*>(com);
					if (s)
					{
						if (generate_contact_sphere_sphere(s, this))
						{
							for (auto it : this->parent->component_list)
							{
								it->OnHit();
							}
							for (auto it : s->parent->component_list)
							{
								it->OnHit();
							}

							break;
						}
					}
				}
			}
		}
	}

}

void SphereComponent::Draw()
{
	primitive->SetTransform(trans);
	Mo2Render().Draw(nolight.get(), *primitive);
}

void SphereComponent::OnHit()
{
	primitive->color = { 1.f,0.f,0.f,1.f };
	parent->Destroy();
}

void SphereComponent::OnTrigger()
{

}

void SphereComponent::ImGui()
{

}