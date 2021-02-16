#include "CameraMovement.h"
#include "Input.h"
//--------------------------------------------------------------------------------------
//
//		Debug
//
//--------------------------------------------------------------------------------------

class FreeMovebyKey : public State<Camera>
{
	void Begin(Camera* c)
	{

	}

	void Execute(Camera* c)
	{
		static float s_rotate_x = 0.f;
		static float s_rotate_y = 0.f;

		{

			float rota_x = -INPUT.mouse.d_x * 0.01f;
			float rota_y = -INPUT.mouse.d_y * 0.01f;

			//MOUSE CONTROLL
			if (INPUT.mouse.click[RIGHT_CLICK])
			{
				// YŽ²‰ñ“]
				s_rotate_y += rota_x * 0.5f;
				// XŽ²‰ñ“]
				s_rotate_x += rota_y * 0.5f;

				::SetCursorPos(CAM_LIST()->screen_w, CAM_LIST()->screen_h);
				::ShowCursor(false);
			}
			else
			{
				::ShowCursor(true);
			}
		}

		{
			float rota_x = -0.2f;
			float rota_y = -0.2f;

			//KEYBOARD CONTROLL
	// YŽ²‰ñ“]
			const float key_move_speed = 0.1f;
			if (INPUT.keys[DIK_RIGHT])
			{
				s_rotate_y += rota_x * key_move_speed;
			}
			else if (INPUT.keys[DIK_LEFT])
			{
				s_rotate_y -= rota_x * key_move_speed;
			}

			if (s_rotate_y > DirectX::XM_PI)
			{
				s_rotate_y -= DirectX::XM_2PI;
			}
			else if (s_rotate_y < -DirectX::XM_PI)
			{
				s_rotate_y += DirectX::XM_2PI;
			}
			// XŽ²‰ñ“]

			if (INPUT.keys[DIK_DOWN])
			{
				s_rotate_x += rota_y * key_move_speed;
			}
			else if (INPUT.keys[DIK_UP])
			{
				s_rotate_x -= rota_y * key_move_speed;
			}

			if (s_rotate_x > DirectX::XM_PI)
			{
				s_rotate_x -= DirectX::XM_2PI;
			}
			else if (s_rotate_x < -DirectX::XM_PI)
			{
				s_rotate_x += DirectX::XM_2PI;
			}

		}


		//ˆÚ“®ˆ—
		float move_speed = 1.f;
		//float move_y = 1.f;
		float MAX_MOVE_SPEED = 5.f;
		{
			float s = c->GetDistance() * 0.035f;
			float x = 0.f;
			float y = 0.f;
			float z = 0.f;


			if (INPUT.keys[DIK_A])
			{
				x = move_speed * s;

			}
			if (INPUT.keys[DIK_D])
			{
				x = -move_speed * s;
			}

			if (INPUT.keys[DIK_SPACE])
			{
				y = move_speed * s;
			}
			if (INPUT.keys[DIK_LSHIFT])
			{
				y = -move_speed * s;
			}


			if (INPUT.keys[DIK_W])
			{
				z = move_speed * s;
			}
			if (INPUT.keys[DIK_S])
			{
				z = -move_speed * s;
			}


			DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&c->GetEye());
			DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&c->GetRight());
			//DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&c->GetUp());
			DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR front = DirectX::XMLoadFloat3(&c->GetFront());

			right = DirectX::XMVectorMultiply(right, DirectX::XMVectorSet(x, x, x, 0.f));
			up = DirectX::XMVectorMultiply(up, DirectX::XMVectorSet(y, y, y, 0.f));
			front = DirectX::XMVectorMultiply(front, DirectX::XMVectorSet(z, z, z, 0.f));

			right = DirectX::XMVectorAdd(DirectX::XMVectorAdd(right, up), front);//right up front ‚Ìƒf[ƒ^‚ðright•Ï”‚ÉW–ñ

			right = DirectX::XMVector3Normalize(right);
			right = DirectX::XMVectorMultiply(right, DirectX::XMVectorSet(MAX_MOVE_SPEED, MAX_MOVE_SPEED, MAX_MOVE_SPEED, 0.f));

			eye = DirectX::XMVectorAdd(eye, right);



			c->SetEye(eye);


		}



		float x_sin = ::sinf(s_rotate_x);
		float x_cos = ::cosf(s_rotate_x);
		float y_sin = ::sinf(s_rotate_y);
		float y_cos = ::cosf(s_rotate_y);


		Mo2Lib::Float3 front = DirectX::XMVectorSet(-x_cos * y_sin, -x_sin, -x_cos * y_cos, 0.0f);
		Mo2Lib::Float3 right = DirectX::XMVectorSet(y_cos, 0, -y_sin, 0.0f);
		Mo2Lib::Float3 up = right.Cross(front);

		Mo2Lib::Float3 eye = DirectX::XMLoadFloat3(&c->GetEye());
		Mo2Lib::Float3 distance = DirectX::XMVectorSet(c->GetDistance(), c->GetDistance(), c->GetDistance(), 0.0f);
		Mo2Lib::Float3 focus = eye - (front * distance);
		//focus = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye.ConvertToXMVECTOR(), focus.ConvertToXMVECTOR(), up.ConvertToXMVECTOR());
		view = DirectX::XMMatrixTranspose(view);

		right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), view);
		up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), view);
		front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), view);

		c->SetEye(eye);
		c->SetFocus(focus);
		c->SetUp(up);
		c->SetRight(right);
		c->SetFront(front);

	}

	void End(Camera* c)
	{

	}
};

class FreeMovebyMouse : public State<Camera>
{
	void Begin(Camera* c)
	{

	}

	void Execute(Camera* c)
	{
		static float s_rotate_x = 0.f;
		static float s_rotate_y = 0.f;


		float move_x = INPUT.mouse.d_x * 0.015f;
		float move_y = INPUT.mouse.d_y * 0.015f;


		if (INPUT.mouse.click[LEFT_CLICK])
		{
			// YŽ²‰ñ“]
			s_rotate_y += move_x * 0.5f;
			if (s_rotate_y > DirectX::XM_PI)
			{
				s_rotate_y -= DirectX::XM_2PI;
			}
			else if (s_rotate_y < -DirectX::XM_PI)
			{
				s_rotate_y += DirectX::XM_2PI;
			}
			// XŽ²‰ñ“]
			s_rotate_x += move_y * 0.5f;
			if (s_rotate_x > DirectX::XM_PI)
			{
				s_rotate_x -= DirectX::XM_2PI;
			}
			else if (s_rotate_x < -DirectX::XM_PI)
			{
				s_rotate_x += DirectX::XM_2PI;
			}
		}
		else if (INPUT.mouse.click[MID_CLICK])
		{
			// •½sˆÚ“®
			float s = c->GetDistance() * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			Mo2Lib::Float3 focus = c->GetFocus();
			Mo2Lib::Float3 up = c->GetUp();
			Mo2Lib::Float3 right = c->GetRight();

			focus.x += right.x * x;
			focus.y += right.y * x;
			focus.z += right.z * x;

			focus.x += up.x * y;
			focus.y += up.y * y;
			focus.z += up.z * y;

			c->SetFocus(focus);
		}
		else if (INPUT.mouse.click[RIGHT_CLICK])
		{
			// ƒY[ƒ€
			float dist = c->GetDistance();
			dist += (-move_y - move_x) * dist * 0.1f;
			c->SetDistance(dist);
		}


		float x_sin = ::sinf(s_rotate_x);
		float x_cos = ::cosf(s_rotate_x);
		float y_sin = ::sinf(s_rotate_y);
		float y_cos = ::cosf(s_rotate_y);


		DirectX::XMVECTOR front = DirectX::XMVectorSet(-x_cos * y_sin, -x_sin, -x_cos * y_cos, 0.0f);
		DirectX::XMVECTOR right = DirectX::XMVectorSet(y_cos, 0, -y_sin, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);

		DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&c->GetFocus());
		DirectX::XMVECTOR distance = DirectX::XMVectorSet(c->GetDistance(), c->GetDistance(), c->GetDistance(), 0.0f);
		DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));
		//focus = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, focus, up);
		view = DirectX::XMMatrixTranspose(view);

		right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), view);
		up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), view);
		front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), view);

		c->SetEye(eye);
		c->SetFocus(focus);
		c->SetUp(up);
		c->SetRight(right);
		c->SetFront(front);

		if (INPUT.KeyReleased(DIK_LMENU))
		{
			CAM_LIST()->ChangeMainCamera(0, CAM_LIST()->IPT_LINER, 3.f);
			//c->GetFSM()->ChangeState(CAM_LIST->GetState(CAM_LIST->FM_KEY));
		}

	}

	void End(Camera* c)
	{

	}
};

//--------------------------------------------------------------------------------------
//
//		InGame
//
//--------------------------------------------------------------------------------------

class TPSFreeMove : public State<Camera>
{
	void Begin(Camera* c)
	{
		c->SetFOV(80.f);

	}

	void Execute(Camera* c)
	{
		Mo2Lib::Float3 dist = Mo2Lib::Float3(c->GetDistance(), c->GetDistance(), c->GetDistance());
		Mo2Lib::Float3 offset = Mo2Lib::Float3(c->GetDistance(), c->GetDistance(), c->GetDistance());
		Mo2Lib::Float3 eye = c->GetOrientation() - c->GetFront() * offset;
		Mo2Lib::Float3 focus = c->GetOrientation() - c->GetFront() * (dist - offset);

		eye.y += 140.f;

		c->SetEye(eye);
		c->SetFocus(focus);

	}

	void End(Camera* c)
	{

	}

};

class TPSAiming : public State<Camera>
{
	void Begin(Camera* c)
	{
		c->SetFOV(30.f);

	}

	void Execute(Camera* c)
	{
		Mo2Lib::Float3 dist = Mo2Lib::Float3(c->GetDistance(), c->GetDistance(), c->GetDistance());
		Mo2Lib::Float3 offset = Mo2Lib::Float3(c->GetDistance(), c->GetDistance(), c->GetDistance());
		Mo2Lib::Float3 eye = c->GetOrientation() - c->GetFront() * offset;
		Mo2Lib::Float3 focus = c->GetOrientation() + c->GetFront() * (dist - offset);


		c->SetEye(eye);
		c->SetFocus(focus);

	}

	void End(Camera* c)
	{

	}

};


Camera::Camera(int index)
{
	float x = m_focus.x - m_eye.x;
	float y = m_focus.y - m_eye.y;
	float z = m_focus.z - m_eye.z;

	m_distance = ::sqrtf(x * x + y * y + z * z);

	fsm = std::make_unique<StateMachine<Camera>>(this);

	fsm->AddState(FM_KEY, std::make_shared<FreeMovebyKey>())
		.AddState(FM_MOUSE, std::make_shared<FreeMovebyMouse>())
		.AddState(FM_TPS, std::make_shared<TPSFreeMove>())
		.AddState(AIM_TPS, std::make_shared<TPSAiming>());

	fsm->SetCurrentState(index);
}