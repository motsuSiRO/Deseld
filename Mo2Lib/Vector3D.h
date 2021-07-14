#pragma once
#include <DirectXMath.h>
#include "Mo2Func.h"

namespace Mo2Lib 
{

	class Float2 : public DirectX::XMFLOAT2
	{
	public:
		//float x;
		//float y;

		Float2() = default;

		Float2(const Float2&) = default;
		Float2& operator=(const Float2&) = default;

		Float2(Float2&&) = default;
		Float2& operator=(Float2&&) = default;
		Float2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
		Float2(DirectX::XMVECTOR v) { DirectX::XMStoreFloat2(this, v); }

		const Float2& operator+=(const Float2& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorAdd(v, rh);
			DirectX::XMStoreFloat2(this, v);

			return *this;
		}

		Float2 operator+(const Float2& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorAdd(v, rh);

			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}

		const Float2& operator-=(const Float2& rhs)
		{
					DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);
			DirectX::XMStoreFloat2(this, v);
			return *this;
		}

		Float2 operator-(const Float2& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);

			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}

		const Float2& operator*=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
			v = DirectX::XMVectorMultiply(v, rh);
			DirectX::XMStoreFloat2(this, v);
			return *this;
		}

		Float2 operator*(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
			v = DirectX::XMVectorMultiply(v, rh);
			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}

		Float2 operator*(const Float2& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorMultiply(v, rh);
			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}

		const Float2& operator/=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
			v = DirectX::XMVectorDivide(v, rh);
			DirectX::XMStoreFloat2(this, v);
			return *this;
		}

		Float2 operator/(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
			v = DirectX::XMVectorDivide(v, rh);
			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}

		Float2 operator/(const Float2& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat2(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat2(&rhs);
			v = DirectX::XMVectorDivide(v, rh);
			Float2 re;
			DirectX::XMStoreFloat2(&re, v);

			return re;
		}


		bool operator==(const Float2& rhs)const
		{
			return (isEqual(x, rhs.x) && isEqual(y, rhs.y));
		}
		bool operator==(const float& rhs)const
		{
			return (isEqual(x, rhs) && isEqual(y, rhs));
		}

		bool operator!=(const Float2& rhs)const
		{
			return (x != rhs.x) || (y != rhs.y);
		}

		bool operator!=(const float& rhs)const
		{
			return (x != rhs) || (y != rhs);
		}

		DirectX::XMVECTOR ConvertToXMVECTOR()
		{
			return DirectX::XMLoadFloat2(this);
		}


		inline float Length() const
		{
			return DirectX::XMVector3Length(DirectX::XMLoadFloat2(this)).m128_f32[0];
		}

		inline float LengthSq() const
		{
			return DirectX::XMVector3LengthSq(DirectX::XMLoadFloat2(this)).m128_f32[0];
		}

		inline float Dot(const Float2& f) const
		{
			return DirectX::XMVector3Dot(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&f)).m128_f32[0];
		}

		inline float Cross(const Float2& f) const
		{
			return DirectX::XMVector2Cross(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&f)).m128_f32[0];
		}

		inline void Normalize()
		{
			float vector_length = this->Length();

			if (vector_length > FLT_EPSILON)
			{
				*this /= vector_length;
			}
		}

		inline Float2 GetNormalize() const
		{
			Float2 vec = *this;

			float vector_length = vec.Length();

			if (vector_length > FLT_EPSILON)
			{
				vec /= vector_length;
			}

			return vec;
		}

		inline Float2 Reverse() const
		{
			return Float2(-this->x, -this->y);
		}

		inline void Limit(float max)
		{
			if (this->LengthSq() > powf(max, 2.f))
			{
				this->Normalize();

				*this *= max;
			}
		}


	};



	class Float3 : public DirectX::XMFLOAT3
	{
	public:
		//float x;
		//float y;
		//float z;

		Float3() = default;
	
		Float3(const Float3&) = default;
		Float3& operator=(const Float3&) = default;

		Float3(Float3&&) = default;
		Float3& operator=(Float3&&) = default;

		Float3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z){}
		Float3(DirectX::XMVECTOR v) { DirectX::XMStoreFloat3(this, v); }

		const Float3& operator+=(const Float3& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorAdd(v, rh);
			DirectX::XMStoreFloat3(this, v);

			return *this;
		}

		Float3 operator+(const Float3& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorAdd(v, rh);

			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}

		const Float3& operator-=(const Float3& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);
			DirectX::XMStoreFloat3(this, v);
			return *this;
		}

		Float3 operator-(const Float3& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);

			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}

		const Float3& operator*=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
			v = DirectX::XMVectorMultiply(v, rh);
			DirectX::XMStoreFloat3(this, v);
			return *this;
		}

		Float3 operator*(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
			v = DirectX::XMVectorMultiply(v, rh);
			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}

		Float3 operator*(const Float3& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorMultiply(v, rh);
			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}

		const Float3& operator/=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
			v = DirectX::XMVectorDivide(v, rh);
			DirectX::XMStoreFloat3(this, v);
			return *this;
		}

		Float3 operator/(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
			v = DirectX::XMVectorDivide(v, rh);
			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}

		Float3 operator/(const Float3& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
			v = DirectX::XMVectorDivide(v, rh);
			Float3 re;
			DirectX::XMStoreFloat3(&re, v);

			return re;
		}


		bool operator==(const Float3& rhs)const
		{
			return (isEqual(x, rhs.x) && isEqual(y, rhs.y) && isEqual(z, rhs.z));
		}
		bool operator==(const float& rhs)const
		{
			return (isEqual(x, rhs) && isEqual(y, rhs) && isEqual(z, rhs));
		}

		bool operator!=(const Float3& rhs)const
		{
			return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
		}

		bool operator!=(const float& rhs)const
		{
			return (x != rhs) || (y != rhs) || (z != rhs);
		}
			
		DirectX::XMVECTOR ConvertToXMVECTOR() const
		{
			return DirectX::XMLoadFloat3(this);
		}


		inline float Length() const
		{
			return DirectX::XMVector3Length(DirectX::XMLoadFloat3(this)).m128_f32[0];
		}
			
		inline float LengthSq() const
		{
			return DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(this)).m128_f32[0];
		}

		inline float Dot(const Float3& f) const
		{
			return DirectX::XMVector3Dot(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&f)).m128_f32[0];
		}

		inline Float3 Cross(const Float3& f) const
		{
			return DirectX::XMVector3Cross(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&f));
		}

		inline void Normalize()
		{
			float vector_length = this->Length();

			if (vector_length > FLT_EPSILON)
			{
				*this /= vector_length;
			}
		}

		inline Float3 GetNormalize() const
		{
			Float3 vec = *this;

			float vector_length = vec.Length();

			if (vector_length > FLT_EPSILON)
			{
				vec /= vector_length;
			}

			return vec;
		}

		inline Float3 Reverse() const
		{
			return Float3( -this->x, -this->y, -this->z );
		}

		inline void Limit(float max)
		{
			if (this->LengthSq() > max*max)
			{
				this->Normalize();

				*this *= max;
			}
		}


	};


	class Float4 : public DirectX::XMFLOAT4
	{
	public:
		Float4() = default;

		Float4(const Float4&) = default;
		Float4& operator=(const Float4&) = default;

		Float4(Float4&&) = default;
		Float4& operator=(Float4&&) = default;

		Float4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
		Float4(DirectX::XMVECTOR v) { DirectX::XMStoreFloat4(this, v); }

		const Float4& operator+=(const Float4& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorAdd(v, rh);
			DirectX::XMStoreFloat4(this, v);

			return *this;
		}

		Float4 operator+(const Float4& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorAdd(v, rh);

			Float4 re;
			DirectX::XMStoreFloat4(&re, v);

			return re;
		}

		const Float4& operator-=(const Float4& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);
			DirectX::XMStoreFloat4(this, v);
			return *this;
		}
		Float4 operator-(const Float4& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorSubtract(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);

			return re;
		}

		const Float4& operator*=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, rhs);
			v = DirectX::XMVectorMultiply(v, rh);
			DirectX::XMStoreFloat4(this, v);
			return *this;
		}
		const Float4& operator*=(const Float4& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMQuaternionMultiply(v, rh);
			DirectX::XMStoreFloat4(this, v);
			this->Normalize();
			return *this;
		}

		Float4 operator*(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, rhs);
			v = DirectX::XMVectorMultiply(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);

			return re;
		}
		Float4 operator*(const Float4& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
		
			v = DirectX::XMQuaternionMultiply(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);
			re.Normalize();

			return re;
		}

		const Float4& operator/=(const float& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, rhs);;
			v = DirectX::XMVectorDivide(v, rh);
			DirectX::XMStoreFloat4(this, v);
			return *this;
		}
		const Float4& operator/=(const Float4& rhs)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorDivide(v, rh);
			DirectX::XMStoreFloat4(this, v);
			return *this;
		}
		Float4 operator/(const float& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, rhs);;
			v = DirectX::XMVectorDivide(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);

			return re;
		}
		Float4 operator/(const Float4& rhs)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&rhs);
			v = DirectX::XMVectorDivide(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);

			return re;
		}


		bool operator==(const Float4 & rhs)const
		{
			return (isEqual(x, rhs.x) && isEqual(y, rhs.y) && isEqual(z, rhs.z) && isEqual(w, rhs.w));
		}

		bool operator!=(const Float4 & rhs)const
		{
			return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
		}

		inline Float4 Inverse() const
		{
			return DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(this));
		}

		inline float Length() const
		{
			return DirectX::XMVector4Length(DirectX::XMLoadFloat4(this)).m128_f32[0];
		}

		inline float LengthSq() const
		{
			return DirectX::XMVector4LengthSq(DirectX::XMLoadFloat4(this)).m128_f32[0];
		}


		inline float Dot(const Float4& f) const
		{
			return DirectX::XMVector4Dot(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&f)).m128_f32[0];
		}

		inline Float3 Vec3Cross(const Float4& f) const
		{
			Float3 v1 = Float3(this->x, this->y, this->z);
			Float3 v2 = Float3(f.x, f.y, f.z);

			return DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2));
		}

		inline void Normalize()
		{
			float vector_length = this->Length();

			if (vector_length > FLT_EPSILON)
			{
				*this /= vector_length;
			}
		}

		inline Float4 GetNormalize() const
		{
			Float4 vec = *this;

			float vector_length = vec.Length();

			if (vector_length > FLT_EPSILON)
			{
				vec /= vector_length;
			}

			return vec;
		}



		DirectX::XMVECTOR ConvertToXMVECTOR()
		{
			return DirectX::XMLoadFloat4(this);
		}

		inline Float4 QuaternionMultiply(const Float4& f)const
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR rh = DirectX::XMLoadFloat4(&f);

			v = DirectX::XMQuaternionMultiply(v, rh);
			Float4 re;
			DirectX::XMStoreFloat4(&re, v);
			re.Normalize();

			return re;

		}

		inline Float4& QuaternionSlerp(const Float4& f, float rate)
		{
			Float4 aft = *this * f;

			*this = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&f), rate);
			return *this;
		}

		inline DirectX::XMVECTOR GetQuaternionSlerp(const Float4& f, float rate)
		{
			Float4 aft = *this * f;

			return DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&f), rate);
		}

	};

	class Float4x4 : public DirectX::XMFLOAT4X4
	{
	public:
		Float4x4() = default;

		Float4x4(const Float4x4&) = default;
		Float4x4& operator=(const Float4x4&) = default;

		Float4x4(Float4x4&&) = default;
		Float4x4& operator=(Float4x4&&) = default;


		DirectX::XMMATRIX ConvertToXMMATRIX()
		{
			return DirectX::XMLoadFloat4x4(this);
		}

	};

	class Transform
	{
	public:
		Float3 translate;
		Float4 rotate;
		Float3 scale;

		Transform() : translate(0, 0, 0),
			rotate(DirectX::XMQuaternionIdentity()),
			scale(1, 1, 1){}

		DirectX::XMMATRIX BuildWorld()
		{
			DirectX::XMMATRIX W;
			DirectX::XMMATRIX S, R, T;

			//‰Šú‰»
			W = DirectX::XMMatrixIdentity();

			//Šg‘å
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

			R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate));

			//	•½sˆÚ“®
			T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);

			//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
			W = S * R * T;

			return W;
		}

	};


	// “_
	typedef Float3 Point;

	// 3ŸŒ³ƒxƒNƒgƒ‹
	struct Vec3 : public Float3
	{
		Vec3() = default;
		Vec3(float x, float y, float z) : Float3(x, y, z) {}
		Vec3(const Float3& r) : Float3(r) {}
		~Vec3() {}

		// ‚’¼”»’è
		bool isVertical(const Float3& r) const
		{
			if (isEqual(Dot(r), 0.f))
			{
				return true;
			}
			return false;
		}

		// •½s”»’è
		bool isParallel(const Vec3& vec) const
		{
			Vec3 vec1 = this->GetNormalize();
			Vec3 vec2 = vec.GetNormalize();
			if (vec1 == vec2 || vec1.Reverse() == vec2)
			{
				return true;
			}
			return false;
		}

		// ‰sŠp”»’è@‰sŠp true : “İŠp false ¦‚’¼‚Í“İŠp‚ÉŠÜ‚Ş
		bool isSharpAngle(const Vec3& vec) const
		{
			float dot = Dot(vec);
			return (dot > 0.f);
		}
	};

	// ’¼ü
	struct Line 
	{
		Point p;
		Vec3 v;
		Line() {}
		Line(const Point& p, const Vec3& v) : p(p), v(v) {}

		// “_ã‚ÌÀ•W‚ğæ“¾
		//  t: ƒxƒNƒgƒ‹‚ÉŠ|‚¯Z‚·‚éŒW”
		Float3 GetPoint(float t) const
		{
			return p + v * t;
		}
	};

	// ü•ª
	struct Segment : public Line 
	{
		Segment() {}
		Segment(const Point& p, const Vec3& v) :Line(p, v) {}
		Segment(const Point& p1, const Point& p2) : Line(p1, Vec3(p2-p1)) {}

		// I“_‚ğæ“¾
		Float3 GetEndPoint() const
		{
			return p + v;
		}
	};


	struct Sphere 
	{
		Point p;
		float r; // ”¼Œa
		Sphere() {}
		Sphere(const Point& p, float r) : p(p), r(r) {}
		~Sphere() {}
	};

	// ƒJƒvƒZƒ‹
	struct Capsule 
	{
		Segment s;
		float r; // ”¼Œa
		Capsule() : r(0.5f) {}
		Capsule(const Segment& s, float r) : s(s), r(r) {}
		Capsule(const Point& p1, const Point& p2, float r) : s(p1, p2), r(r) {}
		~Capsule() {}
	};
}

