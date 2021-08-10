#include "ColliderComponents.h"

#include "Scene.h"
#include "model_renderer.h"
#include "Collision.h"

void ColliderComponent::Start()
{
	is_visible = false;
	primitive = std::make_unique<GeoPrimitive>();
	nolight = std::make_unique<ShaderEx>();
	nolight->Create(L"NoLightObj_VS", L"NoLightObj_PS");
}

void ColliderComponent::Draw()
{
	primitive->SetTransform(trans);
	primitive->color = { 1.f, 1.f, 1.f, 0.3f };
	primitive->is_solid = true;
	Mo2Render().Draw(nolight.get(), *primitive);

	primitive->color = { 1.f, 1.f, 1.f, 1.f };
	primitive->is_solid = false;
	Mo2Render().Draw(nolight.get(), *primitive);

}

void ColliderComponent::ImGui()
{
	
	
}

void BoxComponent::Start()
{
	Collider::Start();

	trans.scale = { 10.f, 10.f, 10.f };
	primitive->CreateCube();
}

void BoxComponent::Update()
{
	
	bool detect = false;
	for (auto obj : Mo2Scene.GetObjects())
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
	Collider::Draw();
}

void BoxComponent::OnHit()
{

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

	bool detect = false;
	for (auto obj : Mo2Scene.GetObjects())
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
	Collider::Draw();
}

void SphereComponent::OnHit()
{
	Transform* trans = parent->GetComponent<Transform>();
	Mo2Lib::Float3 vec;
	vec = trans->translate - contact.point;
	vec.Normalize();
	trans->translate += vec * contact.penetration;
}

void SphereComponent::OnTrigger()
{

}

void SphereComponent::ImGui()
{

}