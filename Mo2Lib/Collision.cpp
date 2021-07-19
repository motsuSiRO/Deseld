#include "Collision.h"
#include "BinaryConversion.h"
#include "framework.h"


#ifdef COLLSION_OLD
Collision* Mo2CD()//Collision Detection
{
	return Collision::GetInstance();
}

void Collision::Initialize()
{
#ifdef _DEBUG
	ID3D11Device* device = Mo2System->DX11device.Get();

	sphere = std::make_unique<GeoPrimitive>(device);
	sphere->CreateSphere(device, 16, 16);
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
	float neart;//一番近い距離を保持
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
		// 面頂点取得  
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);
		//  3 辺算出   
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
		// 外積による法線算出 
		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
		// 内積の結果がプラスならば裏向き 
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot > 0)continue;
		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);


		// 交点算出 
		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(v0, n), DirectX::XMVectorAdd(DirectX::XMVector3Dot(dir, n), DirectX::g_XMEpsilon.v));
		float ft;
		DirectX::XMStoreFloat(&ft, t);
		if (ft < FLT_EPSILON || ft > neart) continue;
		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(t, dir), start);

		//  内点判定 
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

		// 情報保存  
		position = cp; //当たった場所
		normal = n;    //法線
		neart = ft;   //距離 
		ret = it.materialIndex; //インデックス（返り値） 


		*d = neart;
		//最も近いヒット位置までの距離  
		if (ret != -1)//どこにも当たらなかったら。 
		{
			DirectX::XMStoreFloat3(outPosition, position);
			DirectX::XMStoreFloat3(outNormal, normal);
		}

		return ret;
	}

	//if (ret != -1)//どこにも当たらなかったら。 
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
	// 壁の法線
	DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal)));

	// 入射ベクトルを法線に射影
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
	// 反射速度ベクトルの計算
	Mo2Lib::Float3 reflect_vec;

	reflect_vec.x = (in_vec.x + hit_vec.x) * rate;
	reflect_vec.y = in_vec.y + hit_vec.y * rate;
	reflect_vec.z = in_vec.z + hit_vec.z * rate;

	// 反射速度に減衰率rateをかけてoutReflectを計算
	DirectX::XMStoreFloat3(outReflect, DirectX::XMLoadFloat3(&reflect_vec));

	// hitPositionにoutReflectを足して反射位置を計算（簡易計算）
	DirectX::XMStoreFloat3(outPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&hitPosition), DirectX::XMLoadFloat3(outReflect)));

	//DirectX::XMStoreFloat3(outPosition, position);//反射位置
	//DirectX::XMStoreFloat3(outReflect, reflect);//反射速度ベクトル
	return ret;

}


int Collision::MoveCheck(int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	Mo2Lib::Float3* outPosition)
{
	float dist;
	// レイピック
	Mo2Lib::Float3 hitPosition, hitNormal;
	int materialIndex = RayPick(index, startPosition, endPosition, &hitPosition, &hitNormal, &dist);
	if (materialIndex == -1)
	{	// ヒットしなかったら移動後の位置は終点
		*outPosition = endPosition;
		return materialIndex;
	}

	// 壁をつきぬけたベクトル
	DirectX::XMVECTOR hitStart = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, hitStart);

	// 壁の法線ベクトルを単位化
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影
	DirectX::XMVECTOR dot = DirectX::XMVectorNegate(DirectX::XMVector3Dot(vec, normal));
	float hosei = 1.1f;
	DirectX::XMVectorMultiply(dot, DirectX::XMVectorSet(hosei, hosei, hosei, 0.f));
	DirectX::XMVECTOR push_back = DirectX::XMVectorMultiply(dot, normal);
	// 補正位置の計算
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, push_back);


	DirectX::XMStoreFloat3(outPosition, position);
	// 補正後の位置が壁にめり込んでいた場合は移動しないようにする


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
	// 前回のワールド行列と逆行列を求める
	DirectX::XMMATRIX worldTransform = t.BuildWorld();
	DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);
	// 前回のローカル空間でのレイに変換
	DirectX::XMVECTOR worldStart = startPosition.ConvertToXMVECTOR();
	DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform);
	DirectX::XMVECTOR worldEnd = endPosition.ConvertToXMVECTOR();
	DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldEnd, inverseTransform);

	// ローカル空間でのレイとの交点を求める
	float outDistance;
	Mo2Lib::Float3 start, end, position, normal;

	//DirectX::XMStoreFloat3(&start, localStart);
	start = localStart;
	DirectX::XMStoreFloat3(&end, localEnd);
	end = localEnd;

	int ret = RayPick(index, start, end, &position, &normal, &outDistance);
	//注）↑StaticObjectクラスに「protected:	StaticMesh* GetMesh() const { return mesh.get(); }」を追加してください。
	if (ret != -1)
	{
		// 前回のローカル空間から今回のワールド空間へ変換
		// 前回から今回にかけて変更された内容が乗っているオブジェクトに反映される。
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


//作り直し
int Collision::MoveCheck(Mo2Lib::Transform t,
	int index,
	const Mo2Lib::Float3& startPosition,
	const Mo2Lib::Float3& endPosition,
	Mo2Lib::Float3* outPosition)
{
	float dist;
	// レイピック
	Mo2Lib::Float3 hitPosition, hitNormal;
	int materialIndex = LocalRayPick(t, index, startPosition, endPosition, &hitPosition, &hitNormal, &dist);
	if (materialIndex == -1)
	{	// ヒットしなかったら移動後の位置は終点
		*outPosition = endPosition;
		return materialIndex;
	}

	// 壁をつきぬけたベクトル
	DirectX::XMVECTOR hitStart = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, hitStart);

	// 壁の法線ベクトルを単位化
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影
	DirectX::XMVECTOR dot = DirectX::XMVectorNegate(DirectX::XMVector3Dot(vec, normal));
	float hosei = 1.1f;
	DirectX::XMVectorMultiply(dot, DirectX::XMVectorSet(hosei, hosei, hosei, 0.0f));
	DirectX::XMVECTOR push_back = DirectX::XMVectorMultiply(dot, normal);
	// 補正位置の計算
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, push_back);


	DirectX::XMStoreFloat3(outPosition, position);
	// 補正後の位置が壁にめり込んでいた場合は移動しないようにする


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


// 点と直線の最短距離
// p : 点
// l : 直線
// h : 点から下ろした垂線の足（戻り値）
// t :ベクトル係数（戻り値）
// 戻り値: 最短距離
float calcPointLineDist(const Mo2Lib::Point& p, const Mo2Lib::Line& l, Mo2Lib::Point& h, float& t)
{

	float lenSqV = l.v.LengthSq();
	t = 0.0f;
	if (lenSqV > 0.0f)
		t = l.v.Dot(p - l.p) / lenSqV;

	h = l.p + l.v * t;
	return (h - p).Length();
}

// ∠p1p2p3は鋭角？
bool isSharpAngle(const Mo2Lib::Point& p1, const Mo2Lib::Point& p2, const Mo2Lib::Point& p3)
{
	return static_cast<Mo2Lib::Vec3>(p1 - p2).isSharpAngle(p3 - p2);
}

// 点と線分の最短距離
// p : 点
// seg : 線分
// h : 最短距離となる端点（戻り値）
// t : 端点位置（ t < 0: 始点の外, 0 <= t <= 1: 線分内, t > 1: 終点の外 ）
// 戻り値: 最短距離
float calcPointSegmentDist(const Mo2Lib::Point& p, const Mo2Lib::Segment& seg, Mo2Lib::Point& h, float& t)
{

	const Mo2Lib::Point e = seg.GetEndPoint();

	// 垂線の長さ、垂線の足の座標及びtを算出
	float len = calcPointLineDist(p, Mo2Lib::Line(seg.p, e - seg.p), h, t);

	if (isSharpAngle(p, seg.p, e) == false) {
		// 始点側の外側
		h = seg.p;
		return (seg.p - p).Length();
	}
	else if (isSharpAngle(p, e, seg.p) == false) {
		// 終点側の外側
		h = e;
		return (e - p).Length();
	}

	return len;
}

// 2直線の最短距離
// l1 : L1
// l2 : L2
// p1 : L1側の垂線の足（戻り値）
// p2 : L2側の垂線の足（戻り値）
// t1 : L1側のベクトル係数（戻り値）
// t2 : L2側のベクトル係数（戻り値）
// 戻り値: 最短距離
float calcLineLineDist(const Mo2Lib::Line& l1, const Mo2Lib::Line& l2, Mo2Lib::Point& p1, Mo2Lib::Point& p2, float& t1, float& t2)
{

	// 2直線が平行？
	if (l1.v.isParallel(l2.v) == true) {

		// 点P11と直線L2の最短距離の問題に帰着
		float len = calcPointLineDist(l1.p, l2, p2, t2);
		p1 = l1.p;
		t1 = 0.0f;

		return len;
	}

	// 2直線はねじれ関係
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


// 0～1の間にクランプ
void clamp01(float& v) {
	if (v < 0.0f)
		v = 0.0f;
	else if (v > 1.0f)
		v = 1.0f;
}

// 2線分の最短距離
// s1 : S1(線分1)
// s2 : S2(線分2)
// p1 : S1側の垂線の足（戻り値）
// p2 : S2側の垂線の足（戻り値）
// t1 : S1側のベクトル係数（戻り値）
// t2 : S2側のベクトル係数（戻り値）
// 戻り値: 最短距離
float calcSegmentSegmentDist(const Mo2Lib::Segment& s1, const Mo2Lib::Segment& s2, Mo2Lib::Point& p1, Mo2Lib::Point& p2, float& t1, float& t2)
{

	// S1が縮退している？
	if (s1.v.LengthSq() < FLT_EPSILON) {
		// S2も縮退？
		if (s2.v.LengthSq() < FLT_EPSILON) {
			// 点と点の距離の問題に帰着
			float len = (s2.p - s1.p).Length();
			p1 = s1.p;
			p2 = s2.p;
			t1 = t2 = 0.0f;
			return len;
		}
		else {
			// S1の始点とS2の最短問題に帰着
			float len = calcPointSegmentDist(s1.p, s2, p2, t2);
			p1 = s1.p;
			t1 = 0.0f;
			clamp01(t2);
			return len;
		}
	}

	// S2が縮退している？
	else if (s2.v.LengthSq() < FLT_EPSILON) {
		// S2の始点とS1の最短問題に帰着
		float len = calcPointSegmentDist(s2.p, s1, p1, t1);
		p2 = s2.p;
		clamp01(t1);
		t2 = 0.0f;
		return len;
	}

	/* 線分同士 */

	// 2線分が平行だったら垂線の端点の一つをP1に仮決定
	if (s1.v.isParallel(s2.v) == true) {
		t1 = 0.0f;
		p1 = s1.p;
		float len = calcPointSegmentDist(s1.p, s2, p2, t2);
		if (0.0f <= t2 && t2 <= 1.0f)
			return len;
	}
	else {
		// 線分はねじれの関係
		// 2直線間の最短距離を求めて仮のt1,t2を求める
		float len = calcLineLineDist(s1, s2, p1, p2, t1, t2);
		if (
			0.0f <= t1 && t1 <= 1.0f &&
			0.0f <= t2 && t2 <= 1.0f
			) {
			return len;
		}
	}

	// 垂線の足が外にある事が判明
	// S1側のt1を0～1の間にクランプして垂線を降ろす
	clamp01(t1);
	p1 = s1.GetPoint(t1);
	float len = calcPointSegmentDist(p1, s2, p2, t2);
	if (0.0f <= t2 && t2 <= 1.0f)
		return len;

	// S2側が外だったのでS2側をクランプ、S1に垂線を降ろす
	clamp01(t2);
	p2 = s2.GetPoint(t2);
	len = calcPointSegmentDist(p2, s1, p1, t1);
	if (0.0f <= t1 && t1 <= 1.0f)
		return len;

	// 双方の端点が最短と判明
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
		return false; // レイの長さが0

	float s = B * B - A * C;
	if (s < FLT_EPSILON)
		return false; // 衝突していない

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	if (a1 < 0.0f || a2 < 0.0f)
		return false; // レイの反対で衝突

	hit_begin.x = ray.p.x + a1 * ray.v.x;
	hit_begin.y = ray.p.y + a1 * ray.v.y;
	hit_begin.z = ray.p.z + a1 * ray.v.z;
	hit_end.x = ray.p.x + a2 * ray.v.x;
	hit_end.y = ray.p.y + a2 * ray.v.y;
	hit_end.z = ray.p.z + a2 * ray.v.z;

	return true;
}

#else





#endif