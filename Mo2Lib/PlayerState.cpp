//#include "Player.h"
//#include "Input.h"
//#include "PlayerConstant.h"
//#include "Camera.h"
//#include "Bullet.h"
//
////---------------------------------------------------------------------------
////
////					State_List
////
////---------------------------------------------------------------------------
//void Moving_State::Begin(Player* p)
//{
//	p->current_state_index = Player::MOVING_STATE;
//	p->SetMAX_VELOCITY(INIT_MAX_DASH_VELOCITY);
//	p->SetG_Scale(INIT_G_SCALE);
//	p->SetMAX_MOVE_SPEED(MAX_WALK_SPEED);
//
//}
//
//void Moving_State::Execute(Player* p)
//{
//	//p->Jump();
//
//	p->MoveXZ();
//
//
//
//	//if (!p->gunParam.cooling)
//	{
//		if (INPUT.ClickKeeping(LEFT_CLICK))
//		{
//			//p->gunParam.Shoot();
//			Mo2Lib::Float3 fire_dir = p->target_pos - p->fire_pos;
//			//fire_dir.x = -fire_dir.x;
//			//fire_dir.y = -fire_dir.y;
//			fire_dir.Normalize();
//			//GunManager()->Fire(0, p->fire_pos, fire_dir);
//			
//			p->target_pos = CAM_LIST()->main_cam->GetEye() + CAM_LIST()->main_cam->GetFront();
//			
//			p->fsm->ChangeState(Player::AIM_STATE);
//		}
//	}
//	//else
//	{
//		//p->anim.CallAnim(Player::FIRE, false, 1.f);
//	}
//	p->CtrlFreecam();
//
//	p->UpdOrinetation(p->GetVelocity());
//
//
//	if (INPUT.KeyPressed(DIK_LSHIFT))
//	{
//		p->fsm->ChangeState(Player::DODGE_STATE);
//	}
//
//}
//
//void Moving_State::End(Player* p)
//{
//
//}
//
//void Aiming_State::Begin(Player* p)
//{
//	p->current_state_index = Player::AIM_STATE;
//
//	p->SetMAX_VELOCITY(INIT_MAX_AIM_VELOCITY);
//
//}
//
//void Aiming_State::Execute(Player* p)
//{
//
//
//	Mo2Lib::Float2 zero = Mo2Lib::Float2(0, 1.f);
//	Mo2Lib::Float2 move_vec;
//	move_vec.x = -p->GetVelocity().GetNormalize().x;
//	move_vec.y = p->GetVelocity().GetNormalize().z;
//
//	//float vertical_facter = cosf(angle);
//	//float horizon_facter = -sinf(angle);
//	p->vec_angle = acosf(move_vec.Dot(zero) /*/ zero.Length()*/);
//	if (move_vec.Cross(zero) < 0.f)
//	{
//		p->vec_angle = DirectX::XM_2PI - p->vec_angle;
//	}
//
//	p->vec_angle -= p->cam_angle.y;
//
//	if (DirectX::XM_2PI < p->vec_angle)
//	{
//		p->vec_angle -= DirectX::XM_2PI;
//	}
//	if (-FLT_EPSILON > p->vec_angle)
//	{
//		p->vec_angle += DirectX::XM_2PI;
//	}
//	p->anim.anim_spd = p->GetMAX_VELOCITY() / INIT_MAX_AIM_VELOCITY;
//
//
//	if (p->input_vec.y > FLT_EPSILON)
//	{
//		p->input_vec.y = 0.f;
//
//		float range = 0.5f;
//
//		if (DirectX::XM_2PI - range <= p->vec_angle || range > p->vec_angle)
//		{//前
//			p->anim.CallRatioDefAnim(Player::SPINE, Player::MOVE_F, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::L_FOOT, Player::MOVE_F, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::R_FOOT, Player::MOVE_F, Player::TPose, 0.f);
//		}
//		else if (DirectX::XM_PIDIV2 - range <= p->vec_angle && DirectX::XM_PIDIV2 + range > p->vec_angle)
//		{//右
//			p->anim.CallRatioDefAnim(Player::SPINE, Player::MOVE_R, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::L_FOOT, Player::MOVE_R, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::R_FOOT, Player::MOVE_R, Player::TPose, 0.f);
//		}
//		else if (DirectX::XM_PI - range <= p->vec_angle && DirectX::XM_PI + range > p->vec_angle)
//		{//後
//			p->anim.CallRatioDefAnim(Player::SPINE, Player::MOVE_B, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::L_FOOT, Player::MOVE_B, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::R_FOOT, Player::MOVE_B, Player::TPose, 0.f);
//		}
//		else if (DirectX::XM_PI + DirectX::XM_PIDIV2 - range <= p->vec_angle && DirectX::XM_PI + DirectX::XM_PIDIV2 + range > p->vec_angle)
//		{//左
//			p->anim.CallRatioDefAnim(Player::SPINE, Player::MOVE_L, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::L_FOOT, Player::MOVE_L, Player::TPose, 0.f);
//			p->anim.CallRatioDefAnim(Player::R_FOOT, Player::MOVE_L, Player::TPose, 0.f);
//		}
//		else
//		{
//			float rate = 0.f;
//			if (0.f <= p->vec_angle && DirectX::XM_PIDIV2 > p->vec_angle)
//			{//前右
//				p->anim.CallRatioAnim(Player::SPINE, Player::MOVE_F, Player::MOVE_R, p->vec_angle / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::L_FOOT, Player::MOVE_F, Player::MOVE_R, p->vec_angle / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::R_FOOT, Player::MOVE_F, Player::MOVE_R, p->vec_angle / DirectX::XM_PIDIV2);
//			}
//			else if (DirectX::XM_PIDIV2 <= p->vec_angle && DirectX::XM_PI > p->vec_angle)
//			{//後右
//				rate = p->vec_angle - DirectX::XM_PIDIV2;
//				p->anim.CallRatioAnim(Player::SPINE, Player::MOVE_R, Player::MOVE_B, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::L_FOOT, Player::MOVE_R, Player::MOVE_B, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::R_FOOT, Player::MOVE_R, Player::MOVE_B, rate / DirectX::XM_PIDIV2);
//
//			}
//			else if (DirectX::XM_PI <= p->vec_angle && DirectX::XM_PI + DirectX::XM_PIDIV2 > p->vec_angle)
//			{//後左
//				rate = p->vec_angle - DirectX::XM_PI;
//				p->anim.CallRatioAnim(Player::SPINE, Player::MOVE_B, Player::MOVE_L, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::L_FOOT, Player::MOVE_B, Player::MOVE_L, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::R_FOOT, Player::MOVE_B, Player::MOVE_L, rate / DirectX::XM_PIDIV2);
//			}
//			else
//			{//前左
//				rate = p->vec_angle - (DirectX::XM_PI + DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::SPINE, Player::MOVE_L, Player::MOVE_F, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::L_FOOT, Player::MOVE_L, Player::MOVE_F, rate / DirectX::XM_PIDIV2);
//				p->anim.CallRatioAnim(Player::R_FOOT, Player::MOVE_L, Player::MOVE_F, rate / DirectX::XM_PIDIV2);
//			}
//
//		}
//
//		float move_speed = p->GetMAX_MOVE_SPEED() * p->mass;
//
//		p->AddForce(p->input_vec * move_speed);
//	}
//	else
//	{
//		for (size_t i = 0; i < p->anim.layers.size(); ++i)
//		{
//			p->anim.CallBlendAnim(i, Player::IDLE, true, true);
//			//anim.CallAnim(IDLE, true, Mo2Lib::Animator::ANIM_BLEND);
//		}
//	}
//
//	//AIM Vertical
//	float cam_rate = min(1.f,fabsf(p->cam_angle.x) / p->HALF_CAM_ANGLE_X);
//	if (p->cam_angle.x > 0.f)
//	{
//		//p->anim.CallRatioDefAnim(Player::SPINE, Player::IDLE, Player::AIMUP, fabsf(p->cam_angle.x) / p->HALF_CAM_ANGLE_X);
//		p->anim.CallRatioDefAnim(Player::HEAD, Player::IDLE, Player::AIMUP, cam_rate);
//		p->anim.CallRatioDefAnim(Player::L_ARM, Player::IDLE, Player::AIMUP, cam_rate);
//		p->anim.CallRatioDefAnim(Player::R_ARM, Player::IDLE, Player::AIMUP, cam_rate);
//	}
//	else
//	{
//		//p->anim.CallRatioDefAnim(Player::SPINE, Player::IDLE, Player::AIMDOWN, p->cam_angle.x / p->HALF_CAM_ANGLE_X);
//		p->anim.CallRatioDefAnim(Player::HEAD, Player::IDLE, Player::AIMDOWN, cam_rate);
//		p->anim.CallRatioDefAnim(Player::L_ARM, Player::IDLE, Player::AIMDOWN, cam_rate);
//		p->anim.CallRatioDefAnim(Player::R_ARM, Player::IDLE, Player::AIMDOWN, cam_rate);
//	}
//
//	
//	//if (!p->gunParam.cooling)
//	//{
//	//	if (INPUT.ClickKeeping(LEFT_CLICK))
//	//	{
//	//		p->gunParam.Shoot();
//	//		Mo2Lib::Float3 fire_dir = p->target_pos - p->fire_pos;
//	//		//fire_dir.x = -fire_dir.x;
//	//		//fire_dir.y = -fire_dir.y;
//	//		fire_dir.Normalize();
//	//		GunManager()->Fire(0, p->fire_pos, fire_dir);
//
//	//		p->target_pos = p->fire_pos + CAM_LIST()->main_cam->GetFront();
//	//	}
//	//}
//
//
//	if (INPUT.KeyPressed(DIK_LSHIFT))
//	{
//		p->fsm->ChangeState(Player::DODGE_STATE);
//	}
//
//
//	p->CtrlFreecam();
//
//
//	Mo2Lib::Float3 forward_vec = CAM_LIST()->main_cam->GetFront();
//	forward_vec.x = -forward_vec.x;
//	forward_vec.y = -forward_vec.y;
//	p->UpdOrinetation(forward_vec);
//
//}
//
//void Aiming_State::End(Player* p)
//{
//
//}
//
//void Dodge_State::Begin(Player* p)
//{
//	p->current_state_index = Player::DODGE_STATE;
//	p->SetAcceleration(Mo2Lib::Float3(0.f, 0.f, 0.f));
//	p->SetVelocity(Mo2Lib::Float3(0.f, 0.f, 0.f));
//
//	p->SetMAX_VELOCITY(INIT_MAX_VELOCITY * 1.7f);
//	p->SetG_Scale(INIT_G_SCALE * 0.1f);
//	p->SetMAX_MOVE_SPEED(MAX_WALK_SPEED * 5.f);
//	p->isAiming = false;
//	p->MoveXZ();
//	p->input_vec.y = 0.f;
//	float dodge = p->GetMAX_MOVE_SPEED() * p->mass;
//	p->SetVelocity(p->input_vec * dodge);
//
//	p->UpdOrinetation(p->GetVelocity());
//
//	p->anim.anim_spd = p->GetMAX_VELOCITY() / INIT_MAX_DASH_VELOCITY;
//	for (size_t i = 0; i < p->anim.layers.size(); ++i)
//	{
//		p->anim.CallBlendAnim(i, Player::DODGEROLL, false, false);
//		//anim.CallAnim(IDLE, true, Mo2Lib::Animator::ANIM_BLEND);
//	}
//
//}
//
//void Dodge_State::Execute(Player* p)
//{
//	if (p->anim.data[Player::DODGEROLL].end_anim)
//	{
//		p->fsm->ChangeState(Player::MOVING_STATE);
//	}
//
//	float dodge = p->GetMAX_MOVE_SPEED() * p->mass;
//
//	p->AddForce(p->forward * dodge);
//
//	p->CtrlFreecam();
//
//	p->UpdOrinetation(p->GetVelocity());
//
//	p->anim.anim_spd = 1.5f;
//	for (size_t i = 0; i < p->anim.layers.size(); ++i)
//	{
//		p->anim.CallBlendAnim(i, Player::DODGEROLL, false, false);
//		//anim.CallAnim(IDLE, true, Mo2Lib::Animator::ANIM_BLEND);
//	}
//
//
//
//}
//
//void Dodge_State::End(Player* p)
//{
//	//p->MoveXZ();
//}
//
//
////------------------------------------------------------------------------
////
////				Function
////
////------------------------------------------------------------------------
//
//void Player::InputDirection()
//{
//	float angle = cam_angle.y + INPUT.LStickAngle();
//	float move_x = sinf(angle);
//	float move_z = cosf(angle);
//	input_vec = { 0.f, 0.f, 0.f };
//
//	if (INPUT.LStickDeadzoneX(10000) || INPUT.LStickDeadzoneY(10000))
//	{
//		input_vec.x -= move_x;
//		input_vec.y = 1.f;
//		input_vec.z += move_z;
//	}
//
//
//	Mo2Lib::Float2 key_vec = {};
//	Mo2Lib::Float2 zero = Mo2Lib::Float2(0, 1.f);
//
//	if (INPUT.KeyKeeping(DIK_W))
//	{
//		key_vec.y = 1;
//		input_vec.y = 1.f;
//	}
//	else if (INPUT.KeyKeeping(DIK_S))
//	{
//		key_vec.y = -1;
//		input_vec.y = 1.f;
//	}
//	else
//	{
//		key_vec.y = 0;
//	}
//
//	if (INPUT.KeyKeeping(DIK_A))
//	{
//		key_vec.x = -1;
//		input_vec.y = 1.f;
//	}
//	else if (INPUT.KeyKeeping(DIK_D))
//	{
//		key_vec.x = 1;
//		input_vec.y = 1.f;
//	}
//	else
//	{
//		key_vec.x = 0;
//	}
//
//	key_vec.Normalize();
//
//	if (input_vec.y > FLT_EPSILON)
//	{
//
//		angle = acosf(key_vec.Dot(zero) /*/ zero.Length()*/);
//		if (key_vec.Cross(zero) < 0.f)
//		{
//			angle = DirectX::XM_2PI - angle;
//		}
//
//		angle += cam_angle.y;
//
//		if (DirectX::XM_2PI < angle)
//		{
//			angle -= DirectX::XM_2PI;
//		}
//
//		move_z = cosf(angle);
//		move_x = sinf(angle);
//
//
//		input_vec.x -= move_x;
//		input_vec.z += move_z;
//	}
//
//}
//
//void Player::MoveXZ()
//{
//	float move_speed = GetMAX_MOVE_SPEED() * mass;
//
//
//	if (input_vec.y > FLT_EPSILON)
//	{
//		input_vec.y = 0.f;
//
//
//
//		anim.anim_spd = GetMAX_VELOCITY() / INIT_MAX_VELOCITY;
//		anim.CallBlendAnim(SPINE, RUN, true, true);
//		anim.CallBlendAnim(HEAD, RUN, true, true);
//		anim.CallBlendAnim(L_ARM, IDLE, true, true);
//		anim.CallBlendAnim(R_ARM, IDLE, true, true);
//		anim.CallBlendAnim(L_FOOT, RUN, true, true);
//		anim.CallBlendAnim(R_FOOT, RUN, true, true);
//
//		AddForce(input_vec * move_speed);
//
//	}
//
//
//}
//
//void Player::CtrlFreecam()
//{
//	//Xpad
//	if (INPUT.RStickDeadzoneX(10000))
//	{
//		cam_angle.y += INPUT.RStickVector().x * 0.1f;
//
//		if (cam_angle.y > DirectX::XM_2PI)
//		{
//			cam_angle.y -= DirectX::XM_2PI;
//		}
//		else if (cam_angle.y < 0.f)
//		{
//			cam_angle.y += DirectX::XM_2PI;
//		}
//
//	}
//
//	if (INPUT.RStickDeadzoneY(10000))
//	{
//		cam_angle.x += INPUT.RStickVector().y * 0.1f;
//
//		if (cam_angle.x > HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = HALF_CAM_ANGLE_X;
//		}
//		else if (cam_angle.x < -HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = -HALF_CAM_ANGLE_X;
//		}
//
//	}
//
//	//Mouse
//	{
//		cam_angle.y += INPUT.mouse.d_x * 0.01f;
//
//		if (cam_angle.y > DirectX::XM_2PI)
//		{
//			cam_angle.y -= DirectX::XM_2PI;
//		}
//		else if (cam_angle.y < 0.f)
//		{
//			cam_angle.y += DirectX::XM_2PI;
//		}
//
//		cam_angle.x -= INPUT.mouse.d_y * 0.01f;
//
//		if (cam_angle.x > HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = HALF_CAM_ANGLE_X;
//		}
//		else if (cam_angle.x < -HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = -HALF_CAM_ANGLE_X;
//		}
//	}
//
//}
//
//void Player::CtrlAimcam()
//{
//
//	if (INPUT.RStickDeadzoneX(10000))
//	{
//		cam_angle.y += INPUT.RStickVector().x * 0.1f;
//
//		if (cam_angle.y > DirectX::XM_2PI)
//		{
//			cam_angle.y -= DirectX::XM_2PI;
//		}
//		else if (cam_angle.y < 0.f)
//		{
//			cam_angle.y += DirectX::XM_2PI;
//		}
//
//	}
//
//	if (INPUT.RStickDeadzoneY(10000))
//	{
//		cam_angle.x += INPUT.RStickVector().y * 0.1f;
//
//		if (cam_angle.x > HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = HALF_CAM_ANGLE_X;
//		}
//		else if (cam_angle.x < -HALF_CAM_ANGLE_X)
//		{
//			cam_angle.x = -HALF_CAM_ANGLE_X;
//		}
//	}
//
//
//	//Mouse
//	//{
//	//	cam_angle.y += INPUT.mouse.d_x * 0.005f;
//
//	//	if (cam_angle.y > DirectX::XM_2PI)
//	//	{
//	//		cam_angle.y -= DirectX::XM_2PI;
//	//	}
//	//	else if (cam_angle.y < 0.f)
//	//	{
//	//		cam_angle.y += DirectX::XM_2PI;
//	//	}
//
//	//	cam_angle.x -= INPUT.mouse.d_y * 0.01f;
//
//	//	if (cam_angle.x > HALF_CAM_ANGLE_X)
//	//	{
//	//		cam_angle.x = HALF_CAM_ANGLE_X;
//	//	}
//	//	else if (cam_angle.x < -HALF_CAM_ANGLE_X)
//	//	{
//	//		cam_angle.x = -HALF_CAM_ANGLE_X;
//	//	}
//	//}
//}
//
//void Player::Jump()
//{
//	float result = GRAVITY * mass * g_scale;
//	jump_time += fsm->dt;
//	if (is_ground)
//	{
//		jump_time = 0.f;
//
//		if (INPUT.KeyPressed(DIK_SPACE))
//		{
//			AddVelocity(Mo2Lib::Float3(0.f, jump, 0.f));
//			//AddAccelertion(Mo2Lib::Float3(0.f, jump, 0.f));
//			//AddForce(Mo2Lib::Float3(0.f, jump * result, 0.f));
//			//anim.CallAnim(JUMP_L, true, GetMAX_VELOCITY() / INIT_MAX_VELOCITY);
//		}
//	}
//	else
//	{
//		if (jump_time <= MAX_JUMP_TIME)
//		{
//			if (INPUT.KeyKeeping(DIK_SPACE))
//			{
//				AddVelocity(Mo2Lib::Float3(0.f, jump, 0.f));
//				//AddAccelertion(Mo2Lib::Float3(0.f, jump, 0.f));
//				//AddForce(Mo2Lib::Float3(0.f, jump * result, 0.f));
//			}
//			else
//			{
//				jump_time = MAX_JUMP_TIME;
//			}
//		}
//		//anim.CallAnim(JUMP_L, true, GetMAX_VELOCITY() / INIT_MAX_VELOCITY);
//	}
//}
//
////void Player::SetCamAngle()
////{
////	DirectX::XMMATRIX m;
////	m = DirectX::XMMatrixRotationRollPitchYaw(cam_angle.x, cam_angle.y, 0.f);
////
////	Mo2Lib::Float3 right = Mo2Lib::Float3(m.r[0].m128_f32[0], m.r[0].m128_f32[1], m.r[0].m128_f32[2]);
////	Mo2Lib::Float3 up = Mo2Lib::Float3(m.r[1].m128_f32[0], m.r[1].m128_f32[1], m.r[1].m128_f32[2]);
////	Mo2Lib::Float3 front = Mo2Lib::Float3(m.r[2].m128_f32[0], m.r[2].m128_f32[1], m.r[2].m128_f32[2]);
////
////	CAM_LIST()->main_cam->SetFront(front);
////	CAM_LIST()->main_cam->SetRight(right);
////	CAM_LIST()->main_cam->SetUp(up);
////
////
////}
