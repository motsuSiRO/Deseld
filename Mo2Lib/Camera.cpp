#include "Camera.h"

#include "CameraMovement.h"
#include "Mo2GUI.h"

const DirectX::XMMATRIX Camera::GetView()const
{

	// ƒJƒƒ‰‚ÌÝ’è
	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(GetEye().x, GetEye().y, -GetEye().z, 1.0f);
	focus = DirectX::XMVectorSet(GetFocus().x, GetFocus().y, -GetFocus().z, 1.0f);
	up = DirectX::XMVectorSet(GetUp().x, GetUp().y, -GetUp().z, 1.0f);

	return DirectX::XMMatrixLookAtLH(eye, focus, up);
}

Mo2Lib::Float4 Camera::GetFixedQuaternion()
{
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, 0.f, rotate.z);
	DirectX::XMFLOAT4X4 f4x4;
	DirectX::XMStoreFloat4x4(&f4x4, m);
	Mo2Lib::Vec3 up = { f4x4._21, f4x4._22, f4x4._23 };
	Mo2Lib::Float4 orient = DirectX::XMQuaternionRotationMatrix(m);
	orient *= DirectX::XMQuaternionRotationAxis(up.ConvertToXMVECTOR(), rotate.y);
	return orient;

}

void Camera::Update(float dt)
{
	fsm->Update(dt);

	Mo2Lib::Float4x4 m;
	//DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(GetFixedQuaternion().ConvertToXMVECTOR()));

	//m_right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
	//m_up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
	//m_front = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);
}

void Camera::ImGui()
{


	ImGui::DragFloat3("CAM_EYE", &m_eye.x, 0.01f);
	rotate = { DirectX::XMConvertToDegrees(rotate.x), DirectX::XMConvertToDegrees(rotate.y), DirectX::XMConvertToDegrees(rotate.z) };
	ImGui::DragFloat3("CAM_ROTATE", &rotate.x, 0.01f);
	rotate = { DirectX::XMConvertToRadians(rotate.x), DirectX::XMConvertToRadians(rotate.y), DirectX::XMConvertToRadians(rotate.z) };
	ImGui::DragFloat("CAM_DIST", &m_distance, 0.01f);
	ImGui::DragFloat3("CAM_FOCUS", &m_focus.x, 0.01f);

	ImGui::DragFloat("CAM_FOV", &fov_y, 1.f);

	ImGui::DragFloat3("CAM_RIGHT", &m_right.x, 0.01f);
	ImGui::DragFloat3("CAM_UP", &m_up.x, 0.01f);
	ImGui::DragFloat3("CAM_FRONT", &m_front.x, 0.01f);

}

//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------
void MainCamera::Init()
{
	inter_type = -1;
	inter_time = 0.f;
	isInterpolation = false;
	current_cam_index = -1;
	prev_cam_index = -1;


	this->AddCamera(FM_KEY)
		.AddCamera(FM_MOUSE)
		.AddCamera(FM_TPS)
		.AddCamera(AIM_TPS);

	Set(FM_KEY);


}


void MainCamera::Interpolation(float dt)
{
	main_cam->Update(dt);

	if (prev_cam_index == -1)return;

	inter_time += inter_rate * dt;

	if (inter_time > 1.f)
	{
		inter_time = 1.f;
	}
	Camera* prev_cam = cam_list.at(prev_cam_index).get();

	Mo2Lib::Float3 cluc_eye = DirectX::XMVectorLerp(prev_cam->GetEye_VEC(), main_cam->GetEye_VEC(), inter_time);
	Mo2Lib::Float3 cluc_focus = DirectX::XMVectorLerp(prev_cam->GetFocus_VEC(), main_cam->GetFocus_VEC(), inter_time);
	Mo2Lib::Float3 cluc_up = DirectX::XMVectorLerp(prev_cam->GetUp_VEC(), main_cam->GetUp_VEC(), inter_time);
	float culc_FOV = prev_cam->GetFOV() + inter_time * (main_cam->GetFOV() - prev_cam->GetFOV());
	//Mo2Lib::Float3 cluc_right = DirectX::XMVectorLerp(cam_list.at(prev_cam_index)->GetRight_VEC(), main_cam->GetUp_VEC(), inter_time);
	//Mo2Lib::Float3 cluc_front = DirectX::XMVectorLerp(cam_list.at(prev_cam_index)->GetFront_VEC(), main_cam->GetUp_VEC(), inter_time);
	//float culc_FOV = cam_list.at(prev_cam_index)->GetFOV() - main_cam->GetFOV() * inter_time;
	if (inter_time >= 1.f)
	{
		isInterpolation = false;
		current_cam_index = prev_cam_index;
		prev_cam_index = -1;
		inter_time = 0.f;
	}


	switch (inter_type)
	{
	case IPT_LINER:
		if (cluc_eye == cluc_focus)cluc_focus.x += 0.0001f;
		inter_cam.SetEye(cluc_eye);
		inter_cam.SetFocus(cluc_focus);
		inter_cam.SetUp(cluc_up);
		//inter_cam.SetRight(cluc_right);
		//inter_cam.SetFront(cluc_front);
		break;
	default:
		break;
	}


}

bool MainCamera::Set(int index)
{
	if (cam_list.size() >= (size_t)index)
	{
		prev_cam_index = current_cam_index;
		current_cam_index = index;
		main_cam = cam_list.at(index).get();

		return true;
	}
	return false;
}


bool MainCamera::ChangeMainCamera(int index, int interpolation_type, float rate)
{
	if (Set(index))
	{
		inter_rate = rate;
		inter_type = interpolation_type;
		inter_time = 0.f;
		if (interpolation_type >= 0)
		{
			isInterpolation = true;
		}
		return true;
	}


	return false;

}

void MainCamera::SetDirectionFromAngle(float x, float y)
{
	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixRotationRollPitchYaw(x, y, 0.f);

	Mo2Lib::Float3 right = Mo2Lib::Float3(m.r[0].m128_f32[0], m.r[0].m128_f32[1], m.r[0].m128_f32[2]);
	Mo2Lib::Float3 up = Mo2Lib::Float3(m.r[1].m128_f32[0], m.r[1].m128_f32[1], m.r[1].m128_f32[2]);
	Mo2Lib::Float3 front = Mo2Lib::Float3(m.r[2].m128_f32[0], m.r[2].m128_f32[1], m.r[2].m128_f32[2]);

	main_cam->SetFront(front);
	main_cam->SetRight(right);
	main_cam->SetUp(up);
}

const DirectX::XMMATRIX MainCamera::GetView() const
{
	if (isInterpolation)return inter_cam.GetView();
	else return main_cam->GetView();
}


const DirectX::XMMATRIX MainCamera::GetPerspective() const
{
	if (isInterpolation)return inter_cam.GetPerspective();
	else return main_cam->GetPerspective();
}


#define STR(var) #var
void MainCamera::ImGui()
{
	using namespace ImGui;

	Begin("Main_Camera");

	Text("Max_Cam_Index : %d", cam_list.size());

	NewLine();



	//switch (main_cam->GetCamType())
	//{
	//case DEBUGCAM:
	//	Mo2Gui()->str = STR(DEBUGCAM);
	//	Text(Mo2Gui()->str.c_str());
	//	break;
	//case GAMECAM:
	//	Mo2Gui()->str = STR(GAMECAM);
	//	Text(Mo2Gui()->str.c_str());
	//	break;
	//default:
	//	break;
	//}

	//switch (current_cam_index)
	//{
	//case DEBUGCAM:
	//	Text("Debug_CAM");
	//	break;
	//case GAMECAM:
	//	Text("Game_CAM");
	//	break;
	//default:
	//	break;
	//}
	NewLine();

	Mo2Gui()->str = "Current State";
	Text(Mo2Gui()->str.c_str());
	//Mo2Gui()->str = main_cam->GetFSM()->GetName_CurrentState();
	Text("%s", Mo2Gui()->str.c_str());

	//NewLine();

	//Text("Global State");
	//Mo2Gui()->str = main_cam->GetFSM()->GetName_GlobalState();

	//Text("%s", Mo2Gui()->str.c_str());




	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	if (TreeNode("Camera##Parameter"))
	{


		main_cam->ImGui();

		TreePop();
	}

	static int index = 0;
	InputInt("Camera_index##MainCamera", &index);
	if (index < 0)
	{
		index = 0;
	}
	if ((size_t)index > cam_list.size() - 1)
	{
		index = cam_list.size() - 1;
	}

	//Mo2Gui()->str = cam_list.at(index)->GetFSM()->GetName_CurrentState();
	//Text(Mo2Gui()->str.c_str());

	if (Button("ChangeMainCamera"))
	{
		ChangeMainCamera(index, -1);
	}



	if (isInterpolation)ImGui::BulletText("TRUE");
	else ImGui::BulletText("FALSE");
	inter_cam.ImGui();

	End();

}

