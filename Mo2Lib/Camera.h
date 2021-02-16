#pragma once
#include "Vector3D.h"
#include "StateMachine.h"
#include <vector>
#include <memory>
#include <map>




class Camera
{

public:
	Camera() = default;
	Camera(int index);
	~Camera(){}

	void Update(float dt);
	void ImGui();

	Mo2Lib::Float3 rotate;

	const Mo2Lib::Float3& GetEye() const { return m_eye; }
	DirectX::XMVECTOR GetEye_VEC() const { return m_eye.ConvertToXMVECTOR(); }
	const Mo2Lib::Float3& GetFocus() const { return m_focus; }
	DirectX::XMVECTOR GetFocus_VEC() const { return m_focus.ConvertToXMVECTOR(); }
	const Mo2Lib::Float3& GetUp() const { return m_up; }
	DirectX::XMVECTOR GetUp_VEC() const { return m_up.ConvertToXMVECTOR(); }
	const Mo2Lib::Float3& GetRight() const { return m_right; }
	DirectX::XMVECTOR GetRight_VEC() const { return m_right.ConvertToXMVECTOR(); }
	const Mo2Lib::Float3& GetFront() const { return m_front; }
	DirectX::XMVECTOR GetFront_VEC() const { return m_front.ConvertToXMVECTOR(); }
	const float& GetDistance() const { return m_distance; }
	const Mo2Lib::Float3& GetOrientation() const { return m_orientation; }

	const DirectX::XMMATRIX GetView() const;
	const DirectX::XMMATRIX GetPerspective() const { return DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z); }


	const float& GetFOV() const { return fov_y; }
	const float& GetAspect() const { return aspect; }
	const float& GetNearZ() const { return near_z; }
	const float& GetFarZ() const { return far_z; }
	Mo2Lib::Float4 GetFixedQuaternion();


	void SetEye(Mo2Lib::Float3 eye) { m_eye = eye; }
	void SetEye(DirectX::XMVECTOR eye) { m_eye = eye; }
	void SetFocus(Mo2Lib::Float3 focus) { m_focus = focus; }
	void SetFocus(DirectX::XMVECTOR focus) { m_focus = focus; }
	void SetUp(Mo2Lib::Float3 up) { m_up = up; }
	void SetUp(DirectX::XMVECTOR up) { m_up = up; }
	void SetRight(Mo2Lib::Float3 right) { m_right = right; }
	void SetRight(DirectX::XMVECTOR right) { m_right = right; }
	void SetFront(Mo2Lib::Float3 front) { m_front = front; }
	void SetFront(DirectX::XMVECTOR front) { m_front = front; }
	void SetDistance(float dist) { m_distance = dist; }
	void SetOrientation(Mo2Lib::Float3 orient) { m_orientation = orient; }
	void SetOrientation(DirectX::XMVECTOR orient)  { m_orientation = orient; }
	void SetFOV(float fov) {fov_y = DirectX::XMConvertToRadians(fov); }
	void SetAspect(float as) { aspect = as; }
	void SetNearZ(float _near)  {  near_z = _near; }
	void SetFarZ(float _far)  { far_z = _far; }

	std::unique_ptr<StateMachine<Camera>> fsm;

protected:
	Mo2Lib::Float3 m_orientation = { 0.f, 0.f, 0.f };
	Mo2Lib::Float3	m_eye = { 0.f, 0.f, 200.f };
	Mo2Lib::Float3	m_focus = { 0.f, 0.f, 0.f };
	Mo2Lib::Float3	m_up = { 0.f, 1.f, 0.f };
	Mo2Lib::Float3	m_right = { 1.f, 0.f, 0.f };
	Mo2Lib::Float3	m_front = { 0.f, 0.f, 1.f };
	float				m_distance = 100.0f;



	// 角度をラジアン(θ)に変換
	float fov_y = DirectX::XMConvertToRadians(80.0f);	// 画角
	float aspect = 1920.f / 1080.f;	// 画面比率
	float near_z = 10.f;	// 表示最近面までの距離
	float far_z = 5000.0f;	// 表示最遠面までの距離



};



class MainCamera
{
private:
	MainCamera(){}
	~MainCamera(){}
public:

	//enum CAMERA_STATE
	//{
	//	FM_KEY = 0,		//FREEMOVE
	//	FM_MOUSE,		//FREEMOVE
	//	FM_TPS,
	//	AIM_TPS,
	//};

	enum INTERPOLATION_TYPE
	{
		IPT_LINER = 0,

	};

	int screen_w;
	int screen_h;
	int current_cam_index = -1;
	int prev_cam_index = -1;
	float inter_rate;
	float inter_time;
	int inter_type;
	bool isInterpolation;
	Camera inter_cam;
	Camera* main_cam;
	Camera* prev_cam;
	//Camera* prev_cam;
	std::map<int, std::shared_ptr<Camera>> cam_list;
	//std::map<int, std::shared_ptr<State<Camera>>> state_list;

	void Init();
	void Interpolation(float dt);
	bool Set(int index);
	bool ChangeMainCamera(int index, int interpolation_type, float rate = 1.f);
	void ImGui();

	const DirectX::XMMATRIX GetView() const;
	const DirectX::XMMATRIX GetPerspective() const;

	void SetScreensize(int w, int h) { screen_w = w; screen_h = h; }
	void SetDirectionFromAngle(float x, float y);

	MainCamera& AddCamera(int state_index)
	{
		static int index = 0;

		cam_list[index] = std::make_shared<Camera>(state_index);

		index++;

		return *this;
	}

	static MainCamera* GetInstance()
	{
		static MainCamera ins;
		return &ins;
	}
};

static MainCamera* CAM_LIST()
{
	return MainCamera::GetInstance();
}





