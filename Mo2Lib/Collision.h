#pragma once

//#define COLLSION_OLD
#ifdef COLLSION_OLD
#include "Physics.h"
#include <map>
#include <vector>
#include<memory>
#include "FaceLoader.h"
#include "GeoPrimitive.h"

void Reflect2D(Physics2D* obj, Mo2Lib::Float2 normal, float bounce_rate);

class Collision
{
private:
	Collision() = default;
	~Collision() = default;
public:
	struct Faces
	{
		std::vector<Face> face;
		int current_pos;
		std::vector<Mo2Lib::Transform> transforms;
	};

	std::map<int, Faces> face_map;
	void LoadFaceData(int index, const char* filename);

	std::unique_ptr<GeoPrimitive> sphere;
	std::vector<Mo2Lib::Transform> cd_list;

	static Collision* GetInstance()
	{
		static Collision ins;
		return &ins;
	}
	void Initialize();

	void Render(ID3D11DeviceContext* context, 
		const DirectX::XMFLOAT4X4& view_projection,
		const DirectX::XMFLOAT4& light_dir);


	int RayPick(int face_index, const Mo2Lib::Float3& startPosition,
		const Mo2Lib::Float3& endPosition, Mo2Lib::Float3* outPosition,
		Mo2Lib::Float3* outNormal, float* distance);

	int Reflect(int face_index,
		const Mo2Lib::Float3& startPosition,
		const Mo2Lib::Float3& endPosition,
		float rate,
		Mo2Lib::Float3* outPosition,
		Mo2Lib::Float3* outReflect);

	int MoveCheck(int face_index,
		const Mo2Lib::Float3& startPosition,
		const Mo2Lib::Float3& endPosition,
		Mo2Lib::Float3* outPosition);

	int LocalRayPick(Mo2Lib::Transform transform,
		int index,
		const Mo2Lib::Float3& startPosition,
		const Mo2Lib::Float3& endPosition,
		Mo2Lib::Float3* outPosition,
		Mo2Lib::Float3* outNormal, float* distance);

	int MoveCheck(Mo2Lib::Transform t,
		int index,
		const Mo2Lib::Float3& startPosition,
		const Mo2Lib::Float3& endPosition,
		Mo2Lib::Float3* outPosition);

	bool EachSphere(const Mo2Lib::Sphere& sph1, const Mo2Lib::Sphere& sph2);
	bool EachSphere(const Mo2Lib::Point& p1, float r1, const Mo2Lib::Point& p2, float r2);
	// ÉJÉvÉZÉãìØémÇÃè’ìÀîªíË
	bool EachCapsule(const Mo2Lib::Capsule& c1, const Mo2Lib::Capsule& c2);
	bool EachCapsule(const Mo2Lib::Point& ps1, const Mo2Lib::Point& pe1, float r1, const Mo2Lib::Point& ps2, const Mo2Lib::Point& pe2, float r2);


	bool calcRaySphere(
		Mo2Lib::Segment r,
		Mo2Lib::Sphere sph,
		Mo2Lib::Float3& hitbegin,
		Mo2Lib::Float3& hitend
	);
};

Collision* Mo2CD();//Collision Detection

#else
#include "ColliderComponents.h"

class contact;
class CollisionDetector
{
private:
	std::vector<contact> contacts;
};


class contact
{

};

INT generate_contact_sphere_sphere(SphereComponent* s0, SphereComponent* s1/*, std::vector<Contact>* contacts, FLOAT restitution*/);
//INT generate_contact_sphere_plane(Sphere* sphere, Plane* plane, std::vector<Contact>* contacts, FLOAT restitution, BOOL half_space = TRUE);
INT generate_contact_sphere_box(SphereComponent* sphere, BoxComponent* box/*, std::vector<Contact>* contacts, FLOAT restitution*/);
//INT generate_contact_box_plane(Box* box, Plane* plane, std::vector<Contact>* contacts, FLOAT restitution);

#endif