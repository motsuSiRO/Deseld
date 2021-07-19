

#ifdef COLLSION_OLD
#include "Collision.h"
#include "BinaryConversion.h"
#include "framework.h"


Collision* Mo2CD()//Collision Detection
{
	return Collision::GetInstance();
}

void Collision::Initialize()
{
#ifdef _DEBUG
	ID3D11Device* device = Mo2System->DX11device.Get();

	sphere = std::make_unique<GeoPrimitive>();
	sphere->CreateSphere(16, 16);
	sphere->color = { 0.8f, 0.8f, 0.3f, 0.5f };


#endif


}


#ifdef _DEBUG
void Collision::Render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& view_projection,
	const DirectX::XMFLOAT4& light_dir)
{
	for (auto it : cd_list)
	{
		sphere->color = it.rotate;
		
		it.rotate = DirectX::XMQuaternionIdentity();
		sphere->SetTransform(it);

		sphere->render(context, view_projection, light_dir, true);

	}

	cd_list.clear();
}

#endif

void Collision::LoadFaceData(int index, const char* filename)
{
	Face_List face_l;

	face_l.LoadFromData(filename);

	for (auto& face : face_l.faces)
	{
		face_map[index].face.emplace_back(face);
	}
}

int Collision::RayPick(int index, const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition, Mo2Lib::Float3* outPosition,
	Mo2Lib::Float3* outNormal, float* d)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
	float neart;//��ԋ߂�������ێ�
	DirectX::XMStoreFloat(&neart, length);
	if (neart < FLT_EPSILON && neart > -FLT_EPSILON)
	{
		return -1;
	}

	if ((size_t)index > face_map.size())
	{
		return -1;
	}

	DirectX::XMVECTOR position, normal;
	for (const auto& it : face_map[index].face)
	{
		// �ʒ��_�擾  
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);
		//  3 �ӎZ�o   
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
		// �O�ςɂ��@���Z�o 
		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
		// ���ς̌��ʂ��v���X�Ȃ�Η����� 
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot > 0)continue;
		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);


		// ��_�Z�o 
		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(v0, n), DirectX::XMVectorAdd(DirectX::XMVector3Dot(dir, n), DirectX::g_XMEpsilon.v));
		float ft;
		DirectX::XMStoreFloat(&ft, t);
		if (ft < FLT_EPSILON || ft > neart) continue;
		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(t, dir), start);

		//  ���_���� 
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
		DirectX::XMVECTOR temp = DirectX::XMVector3Cross(v1, ab);
		DirectX::XMVECTOR work = DirectX::XMVector3Dot(temp, n);
		float fwork;
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < FLT_EPSILON) continue;

		v1 = DirectX::XMVectorSubtract(b, cp);
		temp = DirectX::XMVector3Cross(v1, bc);
		work = DirectX::XMVector3Dot(temp, n);
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < FLT_EPSILON) continue;

		v1 = DirectX::XMVectorSubtract(c, cp);
		temp = DirectX::XMVector3Cross(v1, ca);
		work = DirectX::XMVector3Dot(temp, n);
		DirectX::XMStoreFloat(&fwork, work);
		if (fwork < FLT_EPSILON) continue;

		if (neart < ft)continue;

		// ���ۑ�  
		position = cp; //���������ꏊ
		normal = n;    //�@��
		neart = ft;   //���� 
		ret = it.materialIndex; //�C���f�b�N�X�i�Ԃ�l�j 


		*d = neart;
		//�ł��߂��q�b�g�ʒu�܂ł̋���  
		if (ret != -1)//�ǂ��ɂ�������Ȃ�������B 
		{
			DirectX::XMStoreFloat3(outPosition, position);
			DirectX::XMStoreFloat3(outNormal, normal);
		}

		return ret;
	}

	//if (ret != -1)//�ǂ��ɂ�������Ȃ�������B 
	//{
	//	DirectX::XMStoreFloat3(outPosition, position);
	//	DirectX::XMStoreFloat3(outNormal, normal);
	//}

	return ret;
}


int Collision::Reflect(int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	float rate,
	Mo2Lib::Float3* outPosition,
	Mo2Lib::Float3* outReflect)
{
	Mo2Lib::Float3 hitPosition, hitNormal;
	float dist;
	int ret = RayPick(index, startPosition, endPosition, &hitPosition, &hitNormal, &dist);
	if (ret < 0)
	{
		return ret;
	}
	// �ǂ̖@��
	DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal)));

	// ���˃x�N�g����@���Ɏˉe
	Mo2Lib::Float3 in_vec, hit_vec;
	DirectX::XMStoreFloat3(&in_vec, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&endPosition), DirectX::XMLoadFloat3(&startPosition)));

	DirectX::XMStoreFloat3(&hit_vec,
		DirectX::XMVectorNegate(
			DirectX::XMVectorMultiply(
				DirectX::XMVector3Dot(
					DirectX::XMLoadFloat3(&in_vec),
					DirectX::XMLoadFloat3(&hitNormal)),
				DirectX::XMLoadFloat3(&hitNormal))));

	hit_vec.x *= 2;
	hit_vec.y *= 2;
	hit_vec.z *= 2;
	// ���ˑ��x�x�N�g���̌v�Z
	Mo2Lib::Float3 reflect_vec;

	reflect_vec.x = (in_vec.x + hit_vec.x) * rate;
	reflect_vec.y = in_vec.y + hit_vec.y * rate;
	reflect_vec.z = in_vec.z + hit_vec.z * rate;

	// ���ˑ��x�Ɍ�����rate��������outReflect���v�Z
	DirectX::XMStoreFloat3(outReflect, DirectX::XMLoadFloat3(&reflect_vec));

	// hitPosition��outReflect�𑫂��Ĕ��ˈʒu���v�Z�i�ȈՌv�Z�j
	DirectX::XMStoreFloat3(outPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&hitPosition), DirectX::XMLoadFloat3(outReflect)));

	//DirectX::XMStoreFloat3(outPosition, position);//���ˈʒu
	//DirectX::XMStoreFloat3(outReflect, reflect);//���ˑ��x�x�N�g��
	return ret;

}


int Collision::MoveCheck(int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	Mo2Lib::Float3* outPosition)
{
	float dist;
	// ���C�s�b�N
	Mo2Lib::Float3 hitPosition, hitNormal;
	int materialIndex = RayPick(index, startPosition, endPosition, &hitPosition, &hitNormal, &dist);
	if (materialIndex == -1)
	{	// �q�b�g���Ȃ�������ړ���̈ʒu�͏I�_
		*outPosition = endPosition;
		return materialIndex;
	}

	// �ǂ����ʂ����x�N�g��
	DirectX::XMVECTOR hitStart = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, hitStart);

	// �ǂ̖@���x�N�g����P�ʉ�
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// ���˃x�N�g����@���Ɏˉe
	DirectX::XMVECTOR dot = DirectX::XMVectorNegate(DirectX::XMVector3Dot(vec, normal));
	float hosei = 1.1f;
	DirectX::XMVectorMultiply(dot, DirectX::XMVectorSet(hosei, hosei, hosei, 0.f));
	DirectX::XMVECTOR push_back = DirectX::XMVectorMultiply(dot, normal);
	// �␳�ʒu�̌v�Z
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, push_back);


	DirectX::XMStoreFloat3(outPosition, position);
	// �␳��̈ʒu���ǂɂ߂荞��ł����ꍇ�͈ړ����Ȃ��悤�ɂ���


	int Index = RayPick(index, startPosition, *outPosition, &hitPosition, &hitNormal, &dist);

	if (Index != -1)
	{
		*outPosition = startPosition;
		//MoveCheck(startPosition, *outPosition, outPosition);
	}

	return materialIndex;
}

int Collision::LocalRayPick(Mo2Lib::Transform t,
	int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	Mo2Lib::Float3* outPosition,
	Mo2Lib::Float3* outNormal, float* distance)
{
	// �O��̃��[���h�s��Ƌt�s������߂�
	DirectX::XMMATRIX worldTransform = t.BuildWorld();
	DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);
	// �O��̃��[�J����Ԃł̃��C�ɕϊ�
	DirectX::XMVECTOR worldStart = startPosition.ConvertToXMVECTOR();
	DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform);
	DirectX::XMVECTOR worldEnd = endPosition.ConvertToXMVECTOR();
	DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldEnd, inverseTransform);

	// ���[�J����Ԃł̃��C�Ƃ̌�_�����߂�
	float outDistance;
	Mo2Lib::Float3 start, end, position, normal;

	//DirectX::XMStoreFloat3(&start, localStart);
	start = localStart;
	DirectX::XMStoreFloat3(&end, localEnd);
	end = localEnd;

	int ret = RayPick(index, start, end, &position, &normal, &outDistance);
	//���j��StaticObject�N���X�Ɂuprotected:	StaticMesh* GetMesh() const { return mesh.get(); }�v��ǉ����Ă��������B
	if (ret != -1)
	{
		// �O��̃��[�J����Ԃ��獡��̃��[���h��Ԃ֕ϊ�
		// �O�񂩂獡��ɂ����ĕύX���ꂽ���e������Ă���I�u�W�F�N�g�ɔ��f�����B
		//worldTransform = GetWorldMatrix();

		DirectX::XMVECTOR localPosition = position.ConvertToXMVECTOR();
		DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);

		DirectX::XMStoreFloat3(outPosition, worldPosition);
		*outPosition = worldPosition;
		*outNormal = normal;
		*distance = outDistance;
	}

	return ret;
}


//��蒼��
int Collision::MoveCheck(Mo2Lib::Transform t,
	int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	Mo2Lib::Float3* outPosition)
{
	float dist;
	// ���C�s�b�N
	Mo2Lib::Float3 hitPosition, hitNormal;
	int materialIndex = LocalRayPick(t, index, startPosition, endPosition, &hitPosition, &hitNormal, &dist);
	if (materialIndex == -1)
	{	// �q�b�g���Ȃ�������ړ���̈ʒu�͏I�_
		*outPosition = endPosition;
		return materialIndex;
	}

	// �ǂ����ʂ����x�N�g��
	DirectX::XMVECTOR hitStart = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, hitStart);

	// �ǂ̖@���x�N�g����P�ʉ�
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// ���˃x�N�g����@���Ɏˉe
	DirectX::XMVECTOR dot = DirectX::XMVectorNegate(DirectX::XMVector3Dot(vec, normal));
	float hosei = 1.1f;
	DirectX::XMVectorMultiply(dot, DirectX::XMVectorSet(hosei, hosei, hosei, 0.0f));
	DirectX::XMVECTOR push_back = DirectX::XMVectorMultiply(dot, normal);
	// �␳�ʒu�̌v�Z
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, push_back);


	DirectX::XMStoreFloat3(outPosition, position);
	// �␳��̈ʒu���ǂɂ߂荞��ł����ꍇ�͈ړ����Ȃ��悤�ɂ���


	//int Index = LocalRayPick(t, index, startPosition, *outPosition, &hitPosition, &hitNormal, &dist);

	//if (Index != -1)
	//{
	//	*outPosition = hitPosition;
	//	//MoveCheck(t, index, startPosition, endPosition, outPosition, outNormal, distance);
	//}

	return materialIndex;
}


bool Collision::EachSphere(
	const Mo2Lib::Sphere& sph1,
	const Mo2Lib::Sphere& sph2)
{
	Mo2Lib::Vec3 vec = sph2.p - sph1.p;
	float r_Sq = (sph1.r + sph2.r) * (sph1.r + sph2.r);
	return (vec.LengthSq() <= r_Sq);
}

bool Collision::EachSphere(
	const Mo2Lib::Point& p1, float r1,
	const Mo2Lib::Point& p2, float r2)
{
	Mo2Lib::Vec3 vec = p2 - p1;
	float r_Sq = (r1 + r2) * (r1 + r2);
	return (vec.LengthSq() <= r_Sq);
}


// �_�ƒ����̍ŒZ����
// p : �_
// l : ����
// h : �_���牺�낵�������̑��i�߂�l�j
// t :�x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float calcPointLineDist(const Mo2Lib::Point& p, const Mo2Lib::Line& l, Mo2Lib::Point& h, float& t)
{

	float lenSqV = l.v.LengthSq();
	t = 0.0f;
	if (lenSqV > 0.0f)
		t = l.v.Dot(p - l.p) / lenSqV;

	h = l.p + l.v * t;
	return (h - p).Length();
}

// ��p1p2p3�͉s�p�H
bool isSharpAngle(const Mo2Lib::Point& p1, const Mo2Lib::Point& p2, const Mo2Lib::Point& p3)
{
	return static_cast<Mo2Lib::Vec3>(p1 - p2).isSharpAngle(p3 - p2);
}

// �_�Ɛ����̍ŒZ����
// p : �_
// seg : ����
// h : �ŒZ�����ƂȂ�[�_�i�߂�l�j
// t : �[�_�ʒu�i t < 0: �n�_�̊O, 0 <= t <= 1: ������, t > 1: �I�_�̊O �j
// �߂�l: �ŒZ����
float calcPointSegmentDist(const Mo2Lib::Point& p, const Mo2Lib::Segment& seg, Mo2Lib::Point& h, float& t)
{

	const Mo2Lib::Point e = seg.GetEndPoint();

	// �����̒����A�����̑��̍��W�y��t���Z�o
	float len = calcPointLineDist(p, Mo2Lib::Line(seg.p, e - seg.p), h, t);

	if (isSharpAngle(p, seg.p, e) == false) {
		// �n�_���̊O��
		h = seg.p;
		return (seg.p - p).Length();
	}
	else if (isSharpAngle(p, e, seg.p) == false) {
		// �I�_���̊O��
		h = e;
		return (e - p).Length();
	}

	return len;
}

// 2�����̍ŒZ����
// l1 : L1
// l2 : L2
// p1 : L1���̐����̑��i�߂�l�j
// p2 : L2���̐����̑��i�߂�l�j
// t1 : L1���̃x�N�g���W���i�߂�l�j
// t2 : L2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float calcLineLineDist(const Mo2Lib::Line& l1, const Mo2Lib::Line& l2, Mo2Lib::Point& p1, Mo2Lib::Point& p2, float& t1, float& t2)
{

	// 2���������s�H
	if (l1.v.isParallel(l2.v) == true) {

		// �_P11�ƒ���L2�̍ŒZ�����̖��ɋA��
		float len = calcPointLineDist(l1.p, l2, p2, t2);
		p1 = l1.p;
		t1 = 0.0f;

		return len;
	}

	// 2�����͂˂���֌W
	float DV1V2 = l1.v.Dot(l2.v);
	float DV1V1 = l1.v.LengthSq();
	float DV2V2 = l2.v.LengthSq();
	Mo2Lib::Vec3 P21P11 = l1.p - l2.p;
	t1 = (DV1V2 * l2.v.Dot(P21P11) - DV2V2 * l1.v.Dot(P21P11)) / (DV1V1 * DV2V2 - DV1V2 * DV1V2);
	p1 = l1.GetPoint(t1);
	t2 = l2.v.Dot(p1 - l2.p) / DV2V2;
	p2 = l2.GetPoint(t2);

	return (p2 - p1).Length();
}


// 0�`1�̊ԂɃN�����v
void clamp01(float& v) {
	if (v < 0.0f)
		v = 0.0f;
	else if (v > 1.0f)
		v = 1.0f;
}

// 2�����̍ŒZ����
// s1 : S1(����1)
// s2 : S2(����2)
// p1 : S1���̐����̑��i�߂�l�j
// p2 : S2���̐����̑��i�߂�l�j
// t1 : S1���̃x�N�g���W���i�߂�l�j
// t2 : S2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float calcSegmentSegmentDist(const Mo2Lib::Segment& s1, const Mo2Lib::Segment& s2, Mo2Lib::Point& p1, Mo2Lib::Point& p2, float& t1, float& t2)
{

	// S1���k�ނ��Ă���H
	if (s1.v.LengthSq() < FLT_EPSILON) {
		// S2���k�ށH
		if (s2.v.LengthSq() < FLT_EPSILON) {
			// �_�Ɠ_�̋����̖��ɋA��
			float len = (s2.p - s1.p).Length();
			p1 = s1.p;
			p2 = s2.p;
			t1 = t2 = 0.0f;
			return len;
		}
		else {
			// S1�̎n�_��S2�̍ŒZ���ɋA��
			float len = calcPointSegmentDist(s1.p, s2, p2, t2);
			p1 = s1.p;
			t1 = 0.0f;
			clamp01(t2);
			return len;
		}
	}

	// S2���k�ނ��Ă���H
	else if (s2.v.LengthSq() < FLT_EPSILON) {
		// S2�̎n�_��S1�̍ŒZ���ɋA��
		float len = calcPointSegmentDist(s2.p, s1, p1, t1);
		p2 = s2.p;
		clamp01(t1);
		t2 = 0.0f;
		return len;
	}

	/* �������m */

	// 2���������s�������琂���̒[�_�̈��P1�ɉ�����
	if (s1.v.isParallel(s2.v) == true) {
		t1 = 0.0f;
		p1 = s1.p;
		float len = calcPointSegmentDist(s1.p, s2, p2, t2);
		if (0.0f <= t2 && t2 <= 1.0f)
			return len;
	}
	else {
		// �����͂˂���̊֌W
		// 2�����Ԃ̍ŒZ���������߂ĉ���t1,t2�����߂�
		float len = calcLineLineDist(s1, s2, p1, p2, t1, t2);
		if (
			0.0f <= t1 && t1 <= 1.0f &&
			0.0f <= t2 && t2 <= 1.0f
			) {
			return len;
		}
	}

	// �����̑����O�ɂ��鎖������
	// S1����t1��0�`1�̊ԂɃN�����v���Đ������~�낷
	clamp01(t1);
	p1 = s1.GetPoint(t1);
	float len = calcPointSegmentDist(p1, s2, p2, t2);
	if (0.0f <= t2 && t2 <= 1.0f)
		return len;

	// S2�����O�������̂�S2�����N�����v�AS1�ɐ������~�낷
	clamp01(t2);
	p2 = s2.GetPoint(t2);
	len = calcPointSegmentDist(p2, s1, p1, t1);
	if (0.0f <= t1 && t1 <= 1.0f)
		return len;

	// �o���̒[�_���ŒZ�Ɣ���
	clamp01(t1);
	p1 = s1.GetPoint(t1);
	return (p2 - p1).Length();
}


bool Collision::EachCapsule(
	const Mo2Lib::Capsule& c1,
	const Mo2Lib::Capsule& c2)
{
	Mo2Lib::Point p1, p2;
	float t1, t2;
	float d = calcSegmentSegmentDist(c1.s, c2.s, p1, p2, t1, t2);
	return (d <= c1.r + c2.r);
}


bool Collision::EachCapsule(
	const Mo2Lib::Point& ps1, const Mo2Lib::Point& pe1, float r1,
	const Mo2Lib::Point& ps2, const Mo2Lib::Point& pe2, float r2)
{
	Mo2Lib::Segment s1 = { ps1, pe1 };
	Mo2Lib::Segment s2 = { ps2, pe2 };
	Mo2Lib::Point p1, p2;
	float t1, t2;
	float d = calcSegmentSegmentDist(s1, s2, p1, p2, t1, t2);
	return (d <= r1 + r2);
}


bool Collision::calcRaySphere(
	Mo2Lib::Segment ray,
	Mo2Lib::Sphere sph,
	Mo2Lib::Float3& hit_begin,
	Mo2Lib::Float3& hit_end)
{
	Mo2Lib::Vec3 target_vec;
	target_vec = sph.p - ray.p;

	//float A = vx * vx + vy * vy + vz * vz;
	float A = ray.v.LengthSq();
	float B = ray.v.x * target_vec.x + ray.v.y * target_vec.y + ray.v.z * target_vec.z;
	float C = target_vec.LengthSq() - sph.r * sph.r;

	if (A < FLT_EPSILON)
		return false; // ���C�̒�����0

	float s = B * B - A * C;
	if (s < FLT_EPSILON)
		return false; // �Փ˂��Ă��Ȃ�

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	if (a1 < 0.0f || a2 < 0.0f)
		return false; // ���C�̔��΂ŏՓ�

	hit_begin.x = ray.p.x + a1 * ray.v.x;
	hit_begin.y = ray.p.y + a1 * ray.v.y;
	hit_begin.z = ray.p.z + a1 * ray.v.z;
	hit_end.x = ray.p.x + a2 * ray.v.x;
	hit_end.y = ray.p.y + a2 * ray.v.y;
	hit_end.z = ray.p.z + a2 * ray.v.z;

	return true;
}

#else
#include "Collision.h"


INT generate_contact_sphere_sphere(SphereComponent* s0, SphereComponent* s1)
{
	//2�̋��̂̏Փ˔�����s��
	//�Փ˂��Ă���ꍇ��Contact�I�u�W�F�N�g�𐶐�����
	//Contact�̑S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
	assert(s0 != s1);

	Mo2Lib::Float3 p0 = s0->trans.translate;
	Mo2Lib::Float3 p1 = s1->trans.translate;

	Mo2Lib::Float3 n = p0 - p1;
	FLOAT l = n.Length();
	//D3DXVec3Normalize(&n, &n);
	if (l < s0->r + s1->r)
	{
		//Contact contact;
		//contact.normal = n;
		//contact.penetration = s0->r + s1->r - l;
		////contact.point = p1 + 0.5f * l * n;
		//contact.point = p1 + (s1->r - (0.5f * contact.penetration)) * n;
		//contact.body[0] = s0;
		//contact.body[1] = s1;
		//contact.restitution = restitution;
		//contacts->push_back(contact);
		return 1;
	}
	return 0;
}
//INT generate_contact_sphere_plane(Sphere* sphere, Plane* plane, std::vector<Contact>* contacts, FLOAT restitution, BOOL half_space)
//{
//	//���̂ƕ��ʂ̏Փ˔�����s��
//	//�Փ˂��Ă���ꍇ��Contact�I�u�W�F�N�g�𐶐�����
//	//Contact�̑S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
//	//��half_space���^�̏ꍇ�͕ЖʁA�U�̏ꍇ�͗��ʂ̏Փ˔�����s��
//	D3DXMATRIX matrix, inverse_matrix;
//	D3DXMatrixRotationQuaternion(&matrix, &plane->orientation);
//	matrix._41 = plane->position.x;
//	matrix._42 = plane->position.y;
//	matrix._43 = plane->position.z;
//	D3DXVECTOR3 n(matrix._21, matrix._22, matrix._23);
//	D3DXMatrixInverse(&inverse_matrix, 0, &matrix);
//
//	D3DXVECTOR3 p;
//	D3DXVec3TransformCoord(&p, &sphere->position, &inverse_matrix);
//
//	//Half-space
//	if (half_space && p.y < 0) return 0;
//
//	if (fabsf(p.y) < sphere->r)
//	{
//		Contact contact;
//		contact.normal = p.y > 0 ? n : -n;
//		contact.point = sphere->position + p.y * -n;
//		contact.penetration = sphere->r - fabsf(p.y);
//		contact.body[0] = sphere;
//		contact.body[1] = plane;
//		contact.restitution = restitution;
//		contacts->push_back(contact);
//		return 1;
//	}
//	return 0;
//}

INT generate_contact_sphere_box(SphereComponent* sphere, BoxComponent* box)
{
	//�@���̂ƒ����̂̏Փ˔�����s��
	//�Փ˂��Ă���ꍇ��Contact�I�u�W�F�N�g�𐶐�����
	//Contact�̑S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
	DirectX::XMMATRIX boxM_space, R, T;
	T = DirectX::XMMatrixTranslation(box->trans.translate.x, box->trans.translate.y, box->trans.translate.z);
	DirectX::XMMatrixRotationQuaternion(box->trans.rotate.ConvertToXMVECTOR());
	Mo2Lib::Float3 dimension = box->GetDimension();
	boxM_space = R * T;
	DirectX::XMMATRIX Inverse_boxM_space;
	Inverse_boxM_space = DirectX::XMMatrixInverse(NULL, boxM_space);

	Mo2Lib::Float3 sp;
	sp = DirectX::XMVector3TransformCoord(sphere->trans.translate.ConvertToXMVECTOR(), Inverse_boxM_space);


	Mo2Lib::Float3 closest_pt = sp;
	if (closest_pt.x > box->trans.scale.x/2)closest_pt.x = box->trans.scale.x/2;
	if (closest_pt.x < -box->trans.scale.x/2)closest_pt.x = -box->trans.scale.x/2;

	if (closest_pt.y > box->trans.scale.y/2)closest_pt.y = box->trans.scale.y/2;
	if (closest_pt.y < -box->trans.scale.y/2)closest_pt.y = -box->trans.scale.y/2;

	if (closest_pt.z > box->trans.scale.z/2)closest_pt.z = box->trans.scale.z/2;
	if (closest_pt.z < -box->trans.scale.z/2)closest_pt.z = -box->trans.scale.z/2;

	Mo2Lib::Float3 dist = closest_pt - sp;
	FLOAT len = dist.Length();

	if (len < sphere->r && len > FLT_EPSILON)
	{
		closest_pt = DirectX::XMVector3TransformCoord(closest_pt.ConvertToXMVECTOR(), boxM_space);

		//Contact contact;
		//D3DXVec3Normalize(&contact.normal, &(sphere->position - closest_pt));
		//contact.point = closest_pt;
		//contact.penetration = sphere->r - dist;
		//contact.body[0] = sphere;
		//contact.body[1] = box;
		//contact.restitution = restitution;
		//contacts->push_back(contact);
		return 1;
	}

	return 0;
}
//INT generate_contact_box_plane(Box* box, Plane* plane, std::vector<Contact>* contacts, FLOAT restitution)
//{
//	//�A�����̂ƕ��ʂ̏Փ˔�����s��
//	//�Փ˂��Ă���ꍇ��Contact�I�u�W�F�N�g�𐶐�����
//	//Contact�̑S�Ẵ����o�ϐ��ɒl���Z�b�g���A�R���e�i(contacts)�ɒǉ�����
//	INT contacts_used = 0;	// ��������Contact�I�u�W�F�N�g�̐��i���t���[���ɏՓ˂����|�C���g�̐��j
//
//	D3DXVECTOR3 vertices[8] =
//	{
//		D3DXVECTOR3(-box->half_size.x, -box->half_size.y, -box->half_size.z),
//		D3DXVECTOR3(-box->half_size.x, -box->half_size.y, +box->half_size.z),
//		D3DXVECTOR3(-box->half_size.x, +box->half_size.y, -box->half_size.z),
//		D3DXVECTOR3(-box->half_size.x, +box->half_size.y, +box->half_size.z),
//		D3DXVECTOR3(+box->half_size.x, -box->half_size.y, -box->half_size.z),
//		D3DXVECTOR3(+box->half_size.x, -box->half_size.y, +box->half_size.z),
//		D3DXVECTOR3(+box->half_size.x, +box->half_size.y, -box->half_size.z),
//		D3DXVECTOR3(+box->half_size.x, +box->half_size.y, +box->half_size.z)
//	};
//
//	D3DXVECTOR3 n;
//	rotate_vector_by_quaternion(&n, plane->orientation, D3DXVECTOR3(0, 1, 0));
//	FLOAT d = D3DXVec3Dot(&n, &plane->position);
//
//	for (int i = 0; i < 8; i++)
//	{
//		rotate_vector_by_quaternion(&vertices[i], box->orientation, vertices[i]);
//		vertices[i] += box->position;
//
//
//		FLOAT distance = D3DXVec3Dot(&vertices[i], &n);
//
//		if (distance < d)
//		{
//			Contact contact;
//			contact.normal = n;
//			contact.point = vertices[i];
//			contact.penetration = d - distance;
//			contact.body[0] = box;
//			contact.body[1] = plane;
//			contact.restitution = restitution;
//
//			// �����Փ˂��N���邽�߁A�ő�߂荞�ݗʂ����|�C���g�ł̂݁A�߂荞�ݗʂ̏������s��
//			float maxPenetration = 0.0f;
//			for (int j = 0; j < contacts->size(); j++)
//			{
//				if ((*contacts)[j].penetration > maxPenetration)
//				{
//					maxPenetration = (*contacts)[j].penetration;
//				}
//			}
//			if (maxPenetration < contact.penetration)
//			{
//				contact.penetFlg = true;
//
//				// ����܂ł̂߂荞�݃t���O��false��
//				for (int j = 0; j < contacts->size(); j++)
//				{
//					(*contacts)[j].penetFlg = false;
//				}
//			}
//			else
//			{
//				contact.penetFlg = false;
//			}
//
//			contacts->push_back(contact);
//
//			contacts_used++;
//		}
//	}
//
//	return contacts_used;
//}



#endif