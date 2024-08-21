#pragma once
#include <d3d11_1.h> 
//#include <d3dx11.h> 
#if !defined(__d3d11_h__) && !defined(__d3d11_x_h__) && !defined(__d3d12_h__) && !defined(__d3d12_x_h__)
#error include d3d11.h or d3d12.h before including TMath.h
#endif

#if !defined(_XBOX_ONE) || !defined(_TITLE)
//#include <dxgi1_2.h>
#endif

#include <functional>
#include <assert.h>
#include <memory.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

#ifndef XM_CONSTEXPR
#define XM_CONSTEXPR
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace BASIS_EX
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix;
	struct Quaternion;
	struct Plane;

	//------------------------------------------------------------------------------
	// 2D rectangle
	struct Rectangle
	{
		long x;
		long y;
		long width;
		long height;

		// Creators
		Rectangle() noexcept : x(0), y(0), width(0), height(0) {}
		XM_CONSTEXPR Rectangle(long ix, long iy, long iw, long ih) : x(ix), y(iy), width(iw), height(ih) {}
		explicit Rectangle(const RECT& rct) : x(rct.left), y(rct.top), width(rct.right - rct.left), height(rct.bottom - rct.top) {}

		Rectangle(const Rectangle&) = default;
		Rectangle& operator=(const Rectangle&) = default;

		Rectangle(Rectangle&&) = default;
		Rectangle& operator=(Rectangle&&) = default;

		operator RECT() { RECT rct; rct.left = x; rct.top = y; rct.right = (x + width); rct.bottom = (y + height); return rct; }
#ifdef __cplusplus_winrt
		operator Windows::Foundation::Rect() { return Windows::Foundation::Rect(float(x), float(y), float(width), float(height)); }
#endif

		// Comparison operators
		bool operator == (const Rectangle& r) const { return (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height); }
		bool operator == (const RECT& rct) const { return (x == rct.left) && (y == rct.top) && (width == (rct.right - rct.left)) && (height == (rct.bottom - rct.top)); }

		bool operator != (const Rectangle& r) const { return (x != r.x) || (y != r.y) || (width != r.width) || (height != r.height); }
		bool operator != (const RECT& rct) const { return (x != rct.left) || (y != rct.top) || (width != (rct.right - rct.left)) || (height != (rct.bottom - rct.top)); }

		// Assignment operators
		Rectangle& operator=(_In_ const RECT& rct) { x = rct.left; y = rct.top; width = (rct.right - rct.left); height = (rct.bottom - rct.top); return *this; }

		// TRectangle operations
		Vector2 Location() const;
		Vector2 Center() const;

		bool IsEmpty() const { return (width == 0 && height == 0 && x == 0 && y == 0); }

		bool Contains(long ix, long iy) const { return (x <= ix) && (ix < (x + width)) && (y <= iy) && (iy < (y + height)); }
		bool Contains(const Vector2& point) const;
		bool Contains(const Rectangle& r) const { return (x <= r.x) && ((r.x + r.width) <= (x + width)) && (y <= r.y) && ((r.y + r.height) <= (y + height)); }
		bool Contains(const RECT& rct) const { return (x <= rct.left) && (rct.right <= (x + width)) && (y <= rct.top) && (rct.bottom <= (y + height)); }

		void Inflate(long horizAmount, long vertAmount);

		bool Intersects(const Rectangle& r) const { return (r.x < (x + width)) && (x < (r.x + r.width)) && (r.y < (y + height)) && (y < (r.y + r.height)); }
		bool Intersects(const RECT& rct) const { return (rct.left < (x + width)) && (x < rct.right) && (rct.top < (y + height)) && (y < rct.bottom); }

		void Offset(long ox, long oy) { x += ox; y += oy; }

		// Static functions
		static Rectangle Intersect(const Rectangle& ra, const Rectangle& rb);
		static RECT Intersect(const RECT& rcta, const RECT& rctb);

		static Rectangle Union(const Rectangle& ra, const Rectangle& rb);
		static RECT Union(const RECT& rcta, const RECT& rctb);
	};

	//------------------------------------------------------------------------------
	// 2D vector
	struct Vector2 : DirectX::XMFLOAT2
	{
		Vector2() noexcept : XMFLOAT2(0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector2(float x) : XMFLOAT2(x, x) {}
		XM_CONSTEXPR Vector2(float _x, float _y) : XMFLOAT2(_x, _y) {}
		explicit Vector2(_In_reads_(2) const float* pArray) : XMFLOAT2(pArray) {}
		Vector2(XMFLOAT2 V) { x = V.x; y = V.y; }
		Vector2(const XMFLOAT2& V) { this->x = V.x; this->y = V.y; }
		explicit Vector2(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; }

		Vector2(const Vector2&) = default;
		Vector2& operator=(const Vector2&) = default;

		Vector2(Vector2&&) = default;
		Vector2& operator=(Vector2&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat2(this); }

		// Comparison operators
		bool operator == (const Vector2& V) const;
		bool operator != (const Vector2& V) const;

		// Assignment operators
		Vector2& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; return *this; }
		Vector2& operator+= (const Vector2& V);
		Vector2& operator-= (const Vector2& V);
		Vector2& operator*= (const Vector2& V);
		Vector2& operator*= (float S);
		Vector2& operator/= (float S);

		// Unary operators
		Vector2 operator+ () const { return *this; }
		Vector2 operator- () const { return Vector2(-x, -y); }

		// Vector operations
		bool InBounds(const Vector2& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector2& V) const;
		void Cross(const Vector2& V, Vector2& result) const;
		Vector2 Cross(const Vector2& V) const;

		void Normalize();
		void Normalize(Vector2& result) const;

		void Clamp(const Vector2& vmin, const Vector2& vmax);
		void Clamp(const Vector2& vmin, const Vector2& vmax, Vector2& result) const;

		// Static functions
		static float Distance(const Vector2& v1, const Vector2& v2);
		static float DistanceSquared(const Vector2& v1, const Vector2& v2);

		static void Min(const Vector2& v1, const Vector2& v2, Vector2& result);
		static Vector2 Min(const Vector2& v1, const Vector2& v2);

		static void Max(const Vector2& v1, const Vector2& v2, Vector2& result);
		static Vector2 Max(const Vector2& v1, const Vector2& v2);

		static void Lerp(const Vector2& v1, const Vector2& v2, float t, Vector2& result);
		static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

		static void SmoothStep(const Vector2& v1, const Vector2& v2, float t, Vector2& result);
		static Vector2 SmoothStep(const Vector2& v1, const Vector2& v2, float t);

		static void Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g, Vector2& result);
		static Vector2 Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g);

		static void CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t, Vector2& result);
		static Vector2 CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t);

		static void Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t, Vector2& result);
		static Vector2 Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t);

		static void Reflect(const Vector2& ivec, const Vector2& nvec, Vector2& result);
		static Vector2 Reflect(const Vector2& ivec, const Vector2& nvec);

		static void Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex, Vector2& result);
		static Vector2 Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex);

		static void Transform(const Vector2& v, const Quaternion& quat, Vector2& result);
		static Vector2 Transform(const Vector2& v, const Quaternion& quat);

		static void Transform(const Vector2& v, const Matrix& m, Vector2& result);
		static Vector2 Transform(const Vector2& v, const Matrix& m);
		static void Transform(_In_reads_(count) const Vector2* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector2* resultArray);

		static void Transform(const Vector2& v, const Matrix& m, Vector4& result);
		static void Transform(_In_reads_(count) const Vector2* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector4* resultArray);

		static void TransformNormal(const Vector2& v, const Matrix& m, Vector2& result);
		static Vector2 TransformNormal(const Vector2& v, const Matrix& m);
		static void TransformNormal(_In_reads_(count) const Vector2* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector2* resultArray);

		// Constants
		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 UnitX;
		static const Vector2 UnitY;
	};

	// Binary operators
	Vector2 operator+ (const Vector2& V1, const Vector2& V2);
	Vector2 operator- (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (const Vector2& V, float S);
	Vector2 operator/ (const Vector2& V1, const Vector2& V2);
	Vector2 operator* (float S, const Vector2& V);

	//------------------------------------------------------------------------------
	// 3D vector
	struct Vector3 : DirectX::XMFLOAT3
	{
		Vector3() noexcept : DirectX::XMFLOAT3(0.f, 0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector3(float x) : DirectX::XMFLOAT3(x, x, x) {}
		XM_CONSTEXPR Vector3(float _x, float _y, float _z) : DirectX::XMFLOAT3(_x, _y, _z) {}
		Vector3(const float* pArray) : DirectX::XMFLOAT3(pArray) {}
		Vector3(DirectX::XMFLOAT3 V) { x = V.x; y = V.y; z = V.z; }
		Vector3(const DirectX::XMFLOAT3& V) { this->x = V.x; this->y = V.y; this->z = V.z; }
		explicit Vector3(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; }

		Vector3(const Vector3&) = default;
		Vector3& operator=(const Vector3&) = default;

		Vector3(Vector3&&) = default;
		Vector3& operator=(Vector3&&) = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat3(this); }

		float operator [](int i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return 0.0f;
		}

		// Comparison operators
		bool operator == (const Vector3& V) const;
		bool operator != (const Vector3& V) const;

		// Assignment operators
		Vector3& operator= (const DirectX::XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; return *this; }
		Vector3& operator+= (const Vector3& V);
		Vector3& operator-= (const Vector3& V);
		Vector3& operator*= (const Vector3& V);
		Vector3& operator*= (float S);
		Vector3& operator/= (float S);

		// Unary operators
		Vector3 operator+ () const { return *this; }
		Vector3 operator- () const;

		// Vector operations
		bool InBounds(const Vector3& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector3& V) const;
		void Cross(const Vector3& V, Vector3& result) const;
		Vector3 Cross(const Vector3& V) const;

		void Normalize();
		void Normalize(Vector3& result) const;

		void Clamp(const Vector3& vmin, const Vector3& vmax);
		void Clamp(const Vector3& vmin, const Vector3& vmax, Vector3& result) const;

		// Static functions
		static float Distance(const Vector3& v1, const Vector3& v2);
		static float DistanceSquared(const Vector3& v1, const Vector3& v2);

		static void Min(const Vector3& v1, const Vector3& v2, Vector3& result);
		static Vector3 Min(const Vector3& v1, const Vector3& v2);

		static void Max(const Vector3& v1, const Vector3& v2, Vector3& result);
		static Vector3 Max(const Vector3& v1, const Vector3& v2);

		static void Lerp(const Vector3& v1, const Vector3& v2, float t, Vector3& result);
		static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

		static void SmoothStep(const Vector3& v1, const Vector3& v2, float t, Vector3& result);
		static Vector3 SmoothStep(const Vector3& v1, const Vector3& v2, float t);

		static void Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g, Vector3& result);
		static Vector3 Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g);

		static void CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t, Vector3& result);
		static Vector3 CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t);

		static void Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t, Vector3& result);
		static Vector3 Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t);

		static void Reflect(const Vector3& ivec, const Vector3& nvec, Vector3& result);
		static Vector3 Reflect(const Vector3& ivec, const Vector3& nvec);

		static void Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex, Vector3& result);
		static Vector3 Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex);

		static void Transform(const Vector3& v, const Quaternion& quat, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Quaternion& quat);

		static void Transform(const Vector3& v, const Matrix& m, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Matrix& m);
		static void Transform(_In_reads_(count) const Vector3* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector3* resultArray);

		static void Transform(const Vector3& v, const Matrix& m, Vector4& result);
		static void Transform(_In_reads_(count) const Vector3* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector4* resultArray);

		static void TransformNormal(const Vector3& v, const Matrix& m, Vector3& result);
		static Vector3 TransformNormal(const Vector3& v, const Matrix& m);
		static void TransformNormal(_In_reads_(count) const Vector3* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector3* resultArray);

		// Constants
		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Right;
		static const Vector3 Left;
		static const Vector3 Forward;
		static const Vector3 Backward;
	};

	// Binary operators
	Vector3 operator+ (const Vector3& V1, const Vector3& V2);
	Vector3 operator- (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (const Vector3& V, float S);
	Vector3 operator/ (const Vector3& V1, const Vector3& V2);
	Vector3 operator* (float S, const Vector3& V);

	//------------------------------------------------------------------------------
	// 4D vector
	struct Vector4 : public XMFLOAT4
	{
		Vector4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
		XM_CONSTEXPR explicit Vector4(float x) : XMFLOAT4(x, x, x, x) {}
		XM_CONSTEXPR Vector4(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		explicit Vector4(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		Vector4(XMFLOAT4 V) { x = V.x; y = V.y; z = V.z; w = V.w; }
		Vector4(const XMFLOAT4& V) { this->x = V.x; this->y = V.y; this->z = V.z; this->w = V.w; }
		explicit Vector4(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		Vector4(const Vector4&) = default;
		Vector4& operator=(const Vector4&) = default;

		Vector4(Vector4&&) = default;
		Vector4& operator=(Vector4&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const Vector4& V) const;
		bool operator != (const Vector4& V) const;

		// Assignment operators
		Vector4& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		Vector4& operator+= (const Vector4& V);
		Vector4& operator-= (const Vector4& V);
		Vector4& operator*= (const Vector4& V);
		Vector4& operator*= (float S);
		Vector4& operator/= (float S);

		// Unary operators
		Vector4 operator+ () const { return *this; }
		Vector4 operator- () const;

		// Vector operations
		bool InBounds(const Vector4& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float Dot(const Vector4& V) const;
		void Cross(const Vector4& v1, const Vector4& v2, Vector4& result) const;
		Vector4 Cross(const Vector4& v1, const Vector4& v2) const;

		void Normalize();
		void Normalize(Vector4& result) const;

		void Clamp(const Vector4& vmin, const Vector4& vmax);
		void Clamp(const Vector4& vmin, const Vector4& vmax, Vector4& result) const;

		// Static functions
		static float Distance(const Vector4& v1, const Vector4& v2);
		static float DistanceSquared(const Vector4& v1, const Vector4& v2);

		static void Min(const Vector4& v1, const Vector4& v2, Vector4& result);
		static Vector4 Min(const Vector4& v1, const Vector4& v2);

		static void Max(const Vector4& v1, const Vector4& v2, Vector4& result);
		static Vector4 Max(const Vector4& v1, const Vector4& v2);

		static void Lerp(const Vector4& v1, const Vector4& v2, float t, Vector4& result);
		static Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

		static void SmoothStep(const Vector4& v1, const Vector4& v2, float t, Vector4& result);
		static Vector4 SmoothStep(const Vector4& v1, const Vector4& v2, float t);

		static void Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g, Vector4& result);
		static Vector4 Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g);

		static void CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t, Vector4& result);
		static Vector4 CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t);

		static void Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t, Vector4& result);
		static Vector4 Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t);

		static void Reflect(const Vector4& ivec, const Vector4& nvec, Vector4& result);
		static Vector4 Reflect(const Vector4& ivec, const Vector4& nvec);

		static void Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex, Vector4& result);
		static Vector4 Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex);

		static void Transform(const Vector2& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector2& v, const Quaternion& quat);

		static void Transform(const Vector3& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector3& v, const Quaternion& quat);

		static void Transform(const Vector4& v, const Quaternion& quat, Vector4& result);
		static Vector4 Transform(const Vector4& v, const Quaternion& quat);

		static void Transform(const Vector4& v, const Matrix& m, Vector4& result);
		static Vector4 Transform(const Vector4& v, const Matrix& m);
		static void Transform(_In_reads_(count) const Vector4* varray, size_t count, const Matrix& m, _Out_writes_(count) Vector4* resultArray);

		// Constants
		static const Vector4 Zero;
		static const Vector4 One;
		static const Vector4 UnitX;
		static const Vector4 UnitY;
		static const Vector4 UnitZ;
		static const Vector4 UnitW;
	};

	// Binary operators
	Vector4 operator+ (const Vector4& V1, const Vector4& V2);
	Vector4 operator- (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (const Vector4& V, float S);
	Vector4 operator/ (const Vector4& V1, const Vector4& V2);
	Vector4 operator* (float S, const Vector4& V);

	//------------------------------------------------------------------------------
	// 4x4 TMatrix (assumes right-handed cooordinates)
	struct Matrix : public XMFLOAT4X4
	{
		Matrix() noexcept
			: XMFLOAT4X4(1.f, 0, 0, 0,
				0, 1.f, 0, 0,
				0, 0, 1.f, 0,
				0, 0, 0, 1.f) {}
		XM_CONSTEXPR Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
			: XMFLOAT4X4(m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33) {}
		explicit Matrix(const Vector3& r0, const Vector3& r1, const Vector3& r2)
			: XMFLOAT4X4(r0.x, r0.y, r0.z, 0,
				r1.x, r1.y, r1.z, 0,
				r2.x, r2.y, r2.z, 0,
				0, 0, 0, 1.f) {}
		explicit Matrix(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3)
			: XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
				r1.x, r1.y, r1.z, r1.w,
				r2.x, r2.y, r2.z, r2.w,
				r3.x, r3.y, r3.z, r3.w) {}
		Matrix(const XMFLOAT4X4& M) { memcpy_s(this, sizeof(float) * 16, &M, sizeof(XMFLOAT4X4)); }
		Matrix(const XMFLOAT3X3& M);
		Matrix(const XMFLOAT4X3& M);

		explicit Matrix(_In_reads_(16) const float* pArray) : XMFLOAT4X4(pArray) {}
		Matrix(CXMMATRIX M) { XMStoreFloat4x4(this, M); }

		Matrix(const Matrix&) = default;
		Matrix& operator=(const Matrix&) = default;

		Matrix(Matrix&&) = default;
		Matrix& operator=(Matrix&&) = default;

		operator XMFLOAT4X4() const { return *this; }

		// Comparison operators
		bool operator == (const Matrix& M) const;
		bool operator != (const Matrix& M) const;

		// Assignment operators
		Matrix& operator= (const XMFLOAT3X3& M);
		Matrix& operator= (const XMFLOAT4X3& M);
		Matrix& operator+= (const Matrix& M);
		Matrix& operator-= (const Matrix& M);
		Matrix& operator*= (const Matrix& M);
		Matrix& operator*= (float S);
		Matrix& operator/= (float S);

		Matrix& operator/= (const Matrix& M);
		// Element-wise divide

		// Unary operators
		Matrix operator+ () const { return *this; }
		Matrix operator- () const;

		// Properties
		Vector3 Up() const { return Vector3(_21, _22, _23); }
		void Up(const Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

		Vector3 Down() const { return Vector3(-_21, -_22, -_23); }
		void Down(const Vector3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

		Vector3 Right() const { return Vector3(_11, _12, _13); }
		void Right(const Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

		Vector3 Left() const { return Vector3(-_11, -_12, -_13); }
		void Left(const Vector3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

		Vector3 Forward() const { return Vector3(-_31, -_32, -_33); }
		void Forward(const Vector3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

		Vector3 Backward() const { return Vector3(_31, _32, _33); }
		void Backward(const Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

		Vector3 Translation() const { return Vector3(_41, _42, _43); }
		void Translation(const Vector3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

		// TMatrix operations
		bool Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation);

		Matrix Transpose() const;
		void Transpose(Matrix& result) const;

		Matrix Invert() const;
		void Invert(Matrix& result) const;

		float Determinant() const;

		// Static functions
		static Matrix CreateBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& cameraUp, _In_opt_ const Vector3* cameraForward = nullptr);

		static Matrix CreateConstrainedBillboard(const Vector3& object, const Vector3& cameraPosition, const Vector3& rotateAxis,
			_In_opt_ const Vector3* cameraForward = nullptr, _In_opt_ const Vector3* objectForward = nullptr);

		static Matrix CreateTranslation(const Vector3& position);
		static Matrix CreateTranslation(float x, float y, float z);

		static Matrix CreateScale(const Vector3& scales);
		static Matrix CreateScale(float xs, float ys, float zs);
		static Matrix CreateScale(float scale);

		static Matrix CreateRotationX(float radians);
		static Matrix CreateRotationY(float radians);
		static Matrix CreateRotationZ(float radians);

		static Matrix CreateFromAxisAngle(const Vector3& axis, float angle);

		static Matrix CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane);
		static Matrix CreatePerspective(float width, float height, float nearPlane, float farPlane);
		static Matrix CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		static Matrix CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
		static Matrix CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

		static Matrix CreateLookAt(const Vector3& position, const Vector3& target, const Vector3& up);
		static Matrix CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up);

		static Matrix CreateFromQuaternion(const Quaternion& quat);

		static Matrix CreateFromYawPitchRoll(float yaw, float pitch, float roll);

		static Matrix CreateShadow(const Vector3& lightDir, const Plane& plane);

		static Matrix CreateReflection(const Plane& plane);

		static void Lerp(const Matrix& M1, const Matrix& M2, float t, Matrix& result);
		static Matrix Lerp(const Matrix& M1, const Matrix& M2, float t);

		static void Transform(const Matrix& M, const Quaternion& rotation, Matrix& result);
		static Matrix Transform(const Matrix& M, const Quaternion& rotation);

		// Constants
		static const Matrix Identity;
	};

	// Binary operators
	Matrix operator+ (const Matrix& M1, const Matrix& M2);
	Matrix operator- (const Matrix& M1, const Matrix& M2);
	Matrix operator* (const Matrix& M1, const Matrix& M2);
	Matrix operator* (const Matrix& M, float S);
	Matrix operator/ (const Matrix& M, float S);
	Matrix operator/ (const Matrix& M1, const Matrix& M2);
	// Element-wise divide
	Matrix operator* (float S, const Matrix& M);


	//-----------------------------------------------------------------------------
	// TPlane
	struct Plane : public XMFLOAT4
	{
		Plane() noexcept : XMFLOAT4(0.f, 1.f, 0.f, 0.f) {}
		XM_CONSTEXPR Plane(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		Plane(const Vector3& normal, float d) : XMFLOAT4(normal.x, normal.y, normal.z, d) {}
		Plane(const Vector3& point1, const Vector3& point2, const Vector3& point3);
		Plane(const Vector3& point, const Vector3& normal);
		explicit Plane(const Vector4& v) : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit Plane(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		Plane(XMFLOAT4 V) { x = V.x; y = V.y; z = V.z; w = V.w; }
		Plane(const XMFLOAT4& p) { this->x = p.x; this->y = p.y; this->z = p.z; this->w = p.w; }
		explicit Plane(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		Plane(const Plane&) = default;
		Plane& operator=(const Plane&) = default;

		Plane(Plane&&) = default;
		Plane& operator=(Plane&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const Plane& p) const;
		bool operator != (const Plane& p) const;

		// Assignment operators
		Plane& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }

		// Properties
		Vector3 Normal() const { return Vector3(x, y, z); }
		void Normal(const Vector3& normal) { x = normal.x; y = normal.y; z = normal.z; }

		float D() const { return w; }
		void D(float d) { w = d; }

		// TPlane operations
		void Normalize();
		void Normalize(Plane& result) const;

		float Dot(const Vector4& v) const;
		float DotCoordinate(const Vector3& position) const;
		float DotNormal(const Vector3& normal) const;

		// Static functions
		static void Transform(const Plane& plane, const Matrix& M, Plane& result);
		static Plane Transform(const Plane& plane, const Matrix& M);

		static void Transform(const Plane& plane, const Quaternion& rotation, Plane& result);
		static Plane Transform(const Plane& plane, const Quaternion& rotation);
		// Input quaternion must be the inverse transpose of the transformation
	};

	//------------------------------------------------------------------------------
	// TQuaternion
	struct Quaternion : public XMFLOAT4
	{
		Quaternion() noexcept : XMFLOAT4(0, 0, 0, 1.f) {}
		XM_CONSTEXPR Quaternion(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		Quaternion(const Vector3& v, float scalar) : XMFLOAT4(v.x, v.y, v.z, scalar) {}
		explicit Quaternion(const Vector4& v) : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit Quaternion(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		Quaternion(XMFLOAT4 V) { x = V.x; y = V.y; z = V.z; w = V.w; }
		Quaternion(const XMFLOAT4& q) { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; }
		explicit Quaternion(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		Quaternion(const Quaternion&) = default;
		Quaternion& operator=(const Quaternion&) = default;

		Quaternion(Quaternion&&) = default;
		Quaternion& operator=(Quaternion&&) = default;

		//operator int () const { return 0; }
		//SampleClass f;
		//int i = f; //  f.operator int () 를 호출하고 초기화 및 반한됨.
		operator XMVECTOR() const { return XMLoadFloat4(this); }

		// Comparison operators
		bool operator == (const Quaternion& q) const;
		bool operator != (const Quaternion& q) const;

		// Assignment operators
		Quaternion& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		Quaternion& operator+= (const Quaternion& q);
		Quaternion& operator-= (const Quaternion& q);
		Quaternion& operator*= (const Quaternion& q);
		Quaternion& operator*= (float S);
		Quaternion& operator/= (const Quaternion& q);

		// Unary operators
		Quaternion operator+ () const { return *this; }
		Quaternion operator- () const;

		// TQuaternion operations
		float Length() const;
		float LengthSquared() const;

		void Normalize();
		void Normalize(Quaternion& result) const;

		void Conjugate();
		void Conjugate(Quaternion& result) const;

		void Inverse(Quaternion& result) const;

		float Dot(const Quaternion& Q) const;

		// Static functions
		static Quaternion CreateFromAxisAngle(const Vector3& axis, float angle);
		static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
		static Quaternion CreateFromRotationMatrix(const Matrix& M);

		static void Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

		static void Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);

		static void Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result);
		static Quaternion Concatenate(const Quaternion& q1, const Quaternion& q2);

		// Constants
		static const Quaternion Identity;
	};

	// Binary operators
	Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (const Quaternion& Q, float S);
	Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2);
	Quaternion operator* (float S, const Quaternion& Q);

	//------------------------------------------------------------------------------
	// TColor
	struct Color : public XMFLOAT4
	{
		Color() noexcept : XMFLOAT4(0, 0, 0, 1.f) {}
		XM_CONSTEXPR Color(float _r, float _g, float _b) : XMFLOAT4(_r, _g, _b, 1.f) {}
		XM_CONSTEXPR Color(float _r, float _g, float _b, float _a) : XMFLOAT4(_r, _g, _b, _a) {}
		explicit Color(const Vector3& clr) : XMFLOAT4(clr.x, clr.y, clr.z, 1.f) {}
		explicit Color(const Vector4& clr) : XMFLOAT4(clr.x, clr.y, clr.z, clr.w) {}
		explicit Color(_In_reads_(4) const float* pArray) : XMFLOAT4(pArray) {}
		Color(XMFLOAT3 V) { x = V.x; y = V.y; z = V.z; }
		Color(const XMFLOAT4& c) { this->x = c.x; this->y = c.y; this->z = c.z; this->w = c.w; }
		explicit Color(const XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		explicit Color(const DirectX::PackedVector::XMCOLOR& Packed);
		// BGRA Direct3D 9 D3DCOLOR packed color

		explicit Color(const DirectX::PackedVector::XMUBYTEN4& Packed);
		// RGBA XNA Game Studio packed color

		Color(const Color&) = default;
		Color& operator=(const Color&) = default;

		Color(Color&&) = default;
		Color& operator=(Color&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }
		operator const float* () const { return reinterpret_cast<const float*>(this); }

		// Comparison operators
		bool operator == (const Color& c) const;
		bool operator != (const Color& c) const;

		// Assignment operators
		Color& operator= (const XMVECTORF32& F) { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		Color& operator= (const DirectX::PackedVector::XMCOLOR& Packed);
		Color& operator= (const DirectX::PackedVector::XMUBYTEN4& Packed);
		Color& operator+= (const Color& c);
		Color& operator-= (const Color& c);
		Color& operator*= (const Color& c);
		Color& operator*= (float S);
		Color& operator/= (const Color& c);

		// Unary operators
		Color operator+ () const { return *this; }
		Color operator- () const;

		// Properties
		float R() const { return x; }
		void R(float r) { x = r; }

		float G() const { return y; }
		void G(float g) { y = g; }

		float B() const { return z; }
		void B(float b) { z = b; }

		float A() const { return w; }
		void A(float a) { w = a; }

		// TColor operations
		DirectX::PackedVector::XMCOLOR BGRA() const;
		DirectX::PackedVector::XMUBYTEN4 RGBA() const;

		Vector3 ToVector3() const;
		Vector4 ToVector4() const;

		void Negate();
		void Negate(Color& result) const;

		void Saturate();
		void Saturate(Color& result) const;

		void Premultiply();
		void Premultiply(Color& result) const;

		void AdjustSaturation(float sat);
		void AdjustSaturation(float sat, Color& result) const;

		void AdjustContrast(float contrast);
		void AdjustContrast(float contrast, Color& result) const;

		// Static functions
		static void Modulate(const Color& c1, const Color& c2, Color& result);
		static Color Modulate(const Color& c1, const Color& c2);

		static void Lerp(const Color& c1, const Color& c2, float t, Color& result);
		static Color Lerp(const Color& c1, const Color& c2, float t);
	};

	// Binary operators
	Color operator+ (const Color& C1, const Color& C2);
	Color operator- (const Color& C1, const Color& C2);
	Color operator* (const Color& C1, const Color& C2);
	Color operator* (const Color& C, float S);
	Color operator/ (const Color& C1, const Color& C2);
	Color operator* (float S, const Color& C);

	//------------------------------------------------------------------------------
	// TRay
	class Ray
	{
	public:
		Vector3 position;
		Vector3 direction;

		Ray() noexcept : position(0, 0, 0), direction(0, 0, 1) {}
		Ray(const Vector3& pos, const Vector3& dir) : position(pos), direction(dir) {}

		Ray(const Ray&) = default;
		Ray& operator=(const Ray&) = default;

		Ray(Ray&&) = default;
		Ray& operator=(Ray&&) = default;

		// Comparison operators
		bool operator == (const Ray& r) const;
		bool operator != (const Ray& r) const;

		// TRay operations
		bool Intersects(const BoundingSphere& sphere, _Out_ float& Dist) const;
		bool Intersects(const BoundingBox& box, _Out_ float& Dist) const;
		bool Intersects(const Vector3& tri0, const Vector3& tri1, const Vector3& tri2, _Out_ float& Dist) const;
		bool Intersects(const Plane& plane, _Out_ float& Dist) const;
	};

	//------------------------------------------------------------------------------
	// TViewport
	class Viewport
	{
	public:
		float x;
		float y;
		float width;
		float height;
		float minDepth;
		float maxDepth;

		Viewport() noexcept :
			x(0.f), y(0.f), width(0.f), height(0.f), minDepth(0.f), maxDepth(1.f) {}
		XM_CONSTEXPR Viewport(float ix, float iy, float iw, float ih, float iminz = 0.f, float imaxz = 1.f) :
			x(ix), y(iy), width(iw), height(ih), minDepth(iminz), maxDepth(imaxz) {}
		explicit Viewport(const RECT& rct) :
			x(float(rct.left)), y(float(rct.top)),
			width(float(rct.right - rct.left)),
			height(float(rct.bottom - rct.top)),
			minDepth(0.f), maxDepth(1.f) {}

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
		// Direct3D 11 interop
		explicit Viewport(const D3D11_VIEWPORT& vp) :
			x(vp.TopLeftX), y(vp.TopLeftY),
			width(vp.Width), height(vp.Height),
			minDepth(vp.MinDepth), maxDepth(vp.MaxDepth) {}

		operator D3D11_VIEWPORT() { return *reinterpret_cast<const D3D11_VIEWPORT*>(this); }
		const D3D11_VIEWPORT* Get11() const { return reinterpret_cast<const D3D11_VIEWPORT*>(this); }
		Viewport& operator= (const D3D11_VIEWPORT& vp);
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__)
		// Direct3D 12 interop
		explicit TViewport(const D3D12_VIEWPORT& vp) :
			x(vp.TopLeftX), y(vp.TopLeftY),
			width(vp.Width), height(vp.Height),
			minDepth(vp.MinDepth), maxDepth(vp.MaxDepth) {}

		operator D3D12_VIEWPORT() { return *reinterpret_cast<const D3D12_VIEWPORT*>(this); }
		const D3D12_VIEWPORT* Get12() const { return reinterpret_cast<const D3D12_VIEWPORT*>(this); }
		TViewport& operator= (const D3D12_VIEWPORT& vp);
#endif

		Viewport(const Viewport&) = default;
		Viewport& operator=(const Viewport&) = default;

		Viewport(Viewport&&) = default;
		Viewport& operator=(Viewport&&) = default;

		// Comparison operators
		bool operator == (const Viewport& vp) const;
		bool operator != (const Viewport& vp) const;

		// Assignment operators
		Viewport& operator= (const RECT& rct);

		// TViewport operations
		float AspectRatio() const;

		Vector3 Project(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world) const;
		void Project(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world, Vector3& result) const;

		Vector3 Unproject(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world) const;
		void Unproject(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world, Vector3& result) const;

		// Static methods
		static RECT __cdecl ComputeDisplayArea(DXGI_SCALING scaling, UINT backBufferWidth, UINT backBufferHeight, int outputWidth, int outputHeight);
		static RECT __cdecl ComputeTitleSafeArea(UINT backBufferWidth, UINT backBufferHeight);
	};


	///////////////////////////////////////// static ///////////////////////////////////////////
	//https://docs.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-migration-d3dx
	//--------------------------
	// 2D Vector
	//--------------------------

	static float D3DXVec2Length(CONST Vector2* pV)
	{
		return pV->Length();
	}

	static float D3DXVec2LengthSq(CONST Vector2* pV)
	{
		return 0.0f;
	}

	static float D3DXVec2Dot(CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return 0.0f;
	}

	// Z component of ((x1,y1,0) cross (x2,y2,0))
	static float D3DXVec2CCW(CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return 0.0f;
	}

	static Vector2* D3DXVec2Add(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return pOut;
	}

	static Vector2* D3DXVec2Subtract(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return pOut;
	}

	// Minimize each component.  x = min(x1, x2), y = min(y1, y2)
	static Vector2* D3DXVec2Minimize(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return pOut;
	}

	// Maximize each component.  x = max(x1, x2), y = max(y1, y2)
	static Vector2* D3DXVec2Maximize(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2)
	{
		return pOut;
	}

	static Vector2* D3DXVec2Scale(Vector2* pOut, CONST Vector2* pV, float s)
	{
		return pOut;
	}

	// Linear interpolation. V1 + s(V2-V1)
	static Vector2* D3DXVec2Lerp(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2,
		float s)
	{
		*pOut = Vector2::Lerp(*pV1, *pV2, s);
		return pOut;
	}
	static Vector2* D3DXVec2Normalize(Vector2* pOut, CONST Vector2* pV)
	{
		return pOut;
	}

	// Hermite interpolation between position V1, tangent T1 (when s == 0)
	// and position V2, tangent T2 (when s == 1).
	static Vector2* D3DXVec2Hermite(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pT1,
		CONST Vector2* pV2, CONST Vector2* pT2, float s)
	{
		return pOut;
	}

	// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	static Vector2* D3DXVec2CatmullRom(Vector2* pOut, CONST Vector2* pV0, CONST Vector2* pV1,
		CONST Vector2* pV2, CONST Vector2* pV3, float s)
	{
		return pOut;
	}

	// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	static Vector2* D3DXVec2BaryCentric(Vector2* pOut, CONST Vector2* pV1, CONST Vector2* pV2,
		CONST Vector2* pV3, float f, float g)
	{
		return pOut;
	}

	// Transform (x, y, 0, 1) by matrix.
	static Vector4* D3DXVec2Transform(Vector4* pOut, CONST Vector2* pV, CONST Matrix* pM)
	{
		return pOut;
	}

	// Transform (x, y, 0, 1) by matrix, project result back into w=1.
	static Vector2* D3DXVec2TransformCoord(Vector2* pOut, CONST Vector2* pV, CONST Matrix* pM)
	{
		return pOut;
	}

	// Transform (x, y, 0, 0) by matrix.
	static Vector2* D3DXVec2TransformNormal(Vector2* pOut, CONST Vector2* pV, CONST Matrix* pM)
	{
		return pOut;
	}

	// Transform Array (x, y, 0, 1) by matrix.
	static Vector4* D3DXVec2TransformArray(Vector4* pOut, UINT OutStride, CONST Vector2* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}

	// Transform Array (x, y, 0, 1) by matrix, project result back into w=1.
	static Vector2* D3DXVec2TransformCoordArray(Vector2* pOut, UINT OutStride, CONST Vector2* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}

	// Transform Array (x, y, 0, 0) by matrix.
	static Vector2* D3DXVec2TransformNormalArray(Vector2* pOut, UINT OutStride, CONST Vector2* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}
	//--------------------------
	// 3D Vector
	//--------------------------

	static  float D3DXVec3Dot(CONST Vector3* pV1, CONST Vector3* pV2)
	{
		return pV1->Dot(*pV2);
	}
	static Vector3* D3DXVec3Cross(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2)
	{
		*pOut = pV1->Cross(*pV2);
		return pOut;
	}
	static Vector3* D3DXVec3Normalize(Vector3* pOut, CONST Vector3* pV)
	{
		pV->Normalize(*pOut);
		return pOut;
	}
	static Vector3* D3DXVec3TransformCoord(Vector3* pOut, CONST Vector3* pV, CONST Matrix* pM)
	{
		*pOut = Vector3::Transform(*pV, *pM);
		return pOut;
	}
	static float D3DXVec3Length(CONST Vector3* pV)
	{
		return pV->Length();
	}



	static float D3DXVec3LengthSq(CONST Vector3* pV)
	{
		return pV->LengthSquared();
	}


	static Vector3* D3DXVec3Add(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2)
	{
		*pOut = *pV1 + *pV2;
		return pOut;
	}

	static Vector3* D3DXVec3Subtract(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2)
	{
		*pOut = *pV1 - *pV2;
		return pOut;
	}

	// Minimize each component.  x = min(x1, x2), y = min(y1, y2), ...
	static Vector3* D3DXVec3Minimize(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2) {
	}

	// Maximize each component.  x = max(x1, x2), y = max(y1, y2), ...
	static Vector3* D3DXVec3Maximize(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2)
	{
		return pOut;
	}

	static Vector3* D3DXVec3Scale(Vector3* pOut, CONST Vector3* pV, float s)
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat3(pV);
		XMVECTOR X = XMVectorScale(v1, s);
		Vector3 R;
		XMStoreFloat3(&R, X);
		*pOut = R;
		return pOut;
	}

	// Linear interpolation. V1 + s(V2-V1)
	static Vector3* D3DXVec3Lerp(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2, float s)
	{
		*pOut = Vector3::Lerp(*pV1, *pV2, s);
		return pOut;
	}


	// Hermite interpolation between position V1, tangent T1 (when s == 0)
	// and position V2, tangent T2 (when s == 1).
	static Vector3* D3DXVec3Hermite(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pT1,
		CONST Vector3* pV2, CONST Vector3* pT2, float s)
	{
		*pOut = Vector3::Hermite(*pV1, *pT1, *pV2, *pT2, s);
		return pOut;
	}

	// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	static Vector3* D3DXVec3CatmullRom(Vector3* pOut, CONST Vector3* pV0, CONST Vector3* pV1,
		CONST Vector3* pV2, CONST Vector3* pV3, float s)
	{
		*pOut = Vector3::CatmullRom(*pV0, *pV1, *pV2, *pV3, s);
		return pOut;
	}

	// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	static Vector3* D3DXVec3BaryCentric(Vector3* pOut, CONST Vector3* pV1, CONST Vector3* pV2,
		CONST Vector3* pV3, float f, float g)
	{
		*pOut = Vector3::Barycentric(*pV1, *pV2, *pV3, f, g);
		return pOut;
	}

	// Transform (x, y, z, 1) by matrix.
	static Vector4* D3DXVec3Transform(Vector4* pOut, CONST Vector3* pV, CONST Matrix* pM)
	{
		return pOut;
	}

	// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a 
	// non-affine matrix, the matrix you pass to this function should be the 
	// transpose of the inverse of the matrix you would use to transform a coord.
	static Vector3* D3DXVec3TransformNormal(Vector3* pOut, CONST Vector3* pV, CONST Matrix* pM)
	{
		*pOut = Vector3::TransformNormal(*pV, *pM);
		return pOut;
	}


	// Transform Array (x, y, z, 1) by matrix. 
	static Vector4* D3DXVec3TransformArray(Vector4* pOut, UINT OutStride, CONST Vector3* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}

	// Transform Array (x, y, z, 1) by matrix, project result back into w=1.
	static Vector3* D3DXVec3TransformCoordArray(Vector3* pOut, UINT OutStride, CONST Vector3* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}

	// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a 
	// non-affine matrix, the matrix you pass to this function should be the 
	// transpose of the inverse of the matrix you would use to transform a coord.
	static Vector3* D3DXVec3TransformNormalArray(Vector3* pOut, UINT OutStride, CONST Vector3* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}

	// Project vector from object space into screen space
	static Vector3* D3DXVec3Project(Vector3* pOut, CONST Vector3* pV, CONST D3D10_VIEWPORT* pViewport,
		CONST Matrix* pProjection, CONST Matrix* pView, CONST Matrix* pWorld)
	{
		//*pOut = TViewport::Project();
		return pOut;
	}

	// Project vector from screen space into object space
	static Vector3* D3DXVec3Unproject(Vector3* pOut, CONST Vector3* pV, CONST D3D10_VIEWPORT* pViewport,
		CONST Matrix* pProjection, CONST Matrix* pView, CONST Matrix* pWorld)
	{
		//*pOut = TViewport::Unproject();
		return pOut;
	}

	// Project vector Array from object space into screen space
	static Vector3* D3DXVec3ProjectArray(Vector3* pOut, UINT OutStride, CONST Vector3* pV, UINT VStride, CONST D3D10_VIEWPORT* pViewport,
		CONST Matrix* pProjection, CONST Matrix* pView, CONST Matrix* pWorld, UINT n)
	{
		return pOut;
	}

	// Project vector Array from screen space into object space
	static Vector3* D3DXVec3UnprojectArray(Vector3* pOut, UINT OutStride, CONST Vector3* pV, UINT VStride, CONST D3D10_VIEWPORT* pViewport,
		CONST Matrix* pProjection, CONST Matrix* pView, CONST Matrix* pWorld, UINT n)
	{
		return pOut;
	}

	//--------------------------
	// 4D Vector
	//--------------------------

	static float D3DXVec4Length(CONST Vector4* pV)
	{
		return pV->Length();
	}


	static float D3DXVec4LengthSq(CONST Vector4* pV)
	{
		return 0.0f;
	}

	static float D3DXVec4Dot(CONST Vector4* pV1, CONST Vector4* pV2)
	{
		return 0.0f;
	}
	static Vector4* D3DXVec4Add(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2)
	{
		return pOut;
	}

	static Vector4* D3DXVec4Subtract(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2)
	{
		return pOut;
	}

	// Minimize each component.  x = min(x1, x2), y = min(y1, y2), ...
	static Vector4* D3DXVec4Minimize(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2)
	{
		return pOut;
	}

	// Maximize each component.  x = max(x1, x2), y = max(y1, y2), ...
	static Vector4* D3DXVec4Maximize(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2)
	{
		return pOut;
	}
	static Vector4* D3DXVec4Scale(Vector4* pOut, CONST Vector4* pV, float s) {
		return pOut;
	}

	// Linear interpolation. V1 + s(V2-V1)
	static Vector4* D3DXVec4Lerp(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2, float s)
	{
		*pOut = Vector4::Lerp(*pV1, *pV2, s);
		return pOut;
	}

	// Cross-product in 4 dimensions.
	static Vector4* D3DXVec4Cross(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2,
		CONST Vector4* pV3)
	{
		return pOut;
	}

	static Vector4* D3DXVec4Normalize(Vector4* pOut, CONST Vector4* pV)
	{
		return pOut;
	}

	// Hermite interpolation between position V1, tangent T1 (when s == 0)
	// and position V2, tangent T2 (when s == 1).
	static Vector4* D3DXVec4Hermite(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pT1,
		CONST Vector4* pV2, CONST Vector4* pT2, float s)
	{
		return pOut;
	}

	// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	static Vector4* D3DXVec4CatmullRom(Vector4* pOut, CONST Vector4* pV0, CONST Vector4* pV1,
		CONST Vector4* pV2, CONST Vector4* pV3, float s)
	{
		return pOut;
	}

	// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	static Vector4* D3DXVec4BaryCentric(Vector4* pOut, CONST Vector4* pV1, CONST Vector4* pV2,
		CONST Vector4* pV3, float f, float g)
	{
		return pOut;
	}

	// Transform vector by matrix.
	static Vector4* D3DXVec4Transform(Vector4* pOut, CONST Vector4* pV, CONST Matrix* pM)
	{
		return pOut;
	}

	// Transform vector array by matrix.
	static Vector4* D3DXVec4TransformArray(Vector4* pOut, UINT OutStride, CONST Vector4* pV, UINT VStride, CONST Matrix* pM, UINT n)
	{
		return pOut;
	}


	static Matrix* D3DXMatrixIdentity(Matrix* pOut)
	{
		XMStoreFloat4x4(pOut, XMMatrixIdentity());
		return pOut;
	}
	static Matrix* D3DXMatrixRotationQuaternion(Matrix* pOut, CONST Quaternion* pQ)
	{
		*pOut = Matrix::CreateFromQuaternion(*pQ);
		return pOut;
	}
	static Quaternion* D3DXQuaternionRotationMatrix(Quaternion* pOut, CONST Matrix* pM)
	{
		*pOut = Quaternion::CreateFromRotationMatrix(*pM);
		return pOut;
	}

	static float D3DXMatrixDeterminant(CONST Matrix* pM) {
		return pM->Determinant();
	}

	static HRESULT D3DXMatrixDecompose(Vector3* pOutScale, Quaternion* pOutRotation,
		Vector3* pOutTranslation, Matrix* pM)
	{
		if (pM->Decompose(*pOutScale, *pOutRotation, *pOutTranslation) == false)
		{
			return S_FALSE;
		}
		return S_OK;
	}

	static Matrix* D3DXMatrixTranspose(Matrix* pOut, CONST Matrix* pM) {

		*pOut = pM->Transpose();
		return pOut;
	}

	// Matrix multiplication.  The result represents the transformation M2
	// followed by the transformation M1.  (Out = M1 * M2)
	static Matrix* D3DXMatrixMultiply(Matrix* pOut, CONST Matrix* pM1, CONST Matrix* pM2) {
		*pOut = (*pM1) * (*pM2);
		return pOut;
	}

	// Matrix multiplication, followed by a transpose. (Out = T(M1 * M2))
	static Matrix* D3DXMatrixMultiplyTranspose(Matrix* pOut, CONST Matrix* pM1, CONST Matrix* pM2) {

		*pOut = (*pM1) * (*pM2);
		pOut->Transpose();
		return pOut;
	}

	// Calculate inverse of matrix.  Inversion my fail, in which case NULL will
	// be returned.  The determinant of pM is also returned it pfDeterminant
	// is non-NULL.
	static Matrix* D3DXMatrixInverse(Matrix* pOut, float* pDeterminant, CONST Matrix* pM) {
		pM->Invert(*pOut);
		if (pDeterminant != nullptr)
		{
			*pDeterminant = pM->Determinant();
		}
		return pOut;
	}

	// Build a matrix which scales by (sx, sy, sz)
	static Matrix* D3DXMatrixScaling(Matrix* pOut, float sx, float sy, float sz) {
		*pOut = Matrix::CreateScale(sx, sy, sz);
		return pOut;
	}

	// Build a matrix which translates by (x, y, z)
	static Matrix* D3DXMatrixTranslation(Matrix* pOut, float x, float y, float z) {
		*pOut = Matrix::CreateTranslation(x, y, z);
		return pOut;
	}

	// Build a matrix which rotates around the X axis
	static Matrix* D3DXMatrixRotationX(Matrix* pOut, float Angle) {
		*pOut = Matrix::CreateRotationX(Angle);
		return pOut;
	}

	// Build a matrix which rotates around the Y axis
	static Matrix* D3DXMatrixRotationY(Matrix* pOut, float Angle) {
		*pOut = Matrix::CreateRotationY(Angle);
		return pOut;
	}

	// Build a matrix which rotates around the Z axis
	static Matrix* D3DXMatrixRotationZ(Matrix* pOut, float Angle) {
		*pOut = Matrix::CreateRotationZ(Angle);
		return pOut;
	}

	// Build a matrix which rotates around an arbitrary axis
	static Matrix* D3DXMatrixRotationAxis(Matrix* pOut, CONST Vector3* pV, float Angle) {
		*pOut = Matrix::CreateFromAxisAngle(*pV, Angle);
		return pOut;
	}

	// Yaw around the Y axis, a pitch around the X axis,
	// and a roll around the Z axis.
	static Matrix* D3DXMatrixRotationYawPitchRoll(Matrix* pOut, float Yaw, float Pitch, float Roll) {
		*pOut = Matrix::CreateFromYawPitchRoll(Yaw, Pitch, Roll);
		return pOut;
	}

	// Build transformation matrix.  NULL arguments are treated as identity.
	// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
	static Matrix* D3DXMatrixTransformation(Matrix* pOut, CONST Vector3* pScalingCenter,
		CONST Quaternion* pScalingRotation, CONST Vector3* pScaling,
		CONST Vector3* pRotationCenter, CONST Quaternion* pRotation,
		CONST Vector3* pTranslation)
	{
		return pOut;
	}

	// Build 2D transformation matrix in XY plane.  NULL arguments are treated as identity.
	// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
	static Matrix* D3DXMatrixTransformation2D(Matrix* pOut, CONST Vector2* pScalingCenter,
		float ScalingRotation, CONST Vector2* pScaling,
		CONST Vector2* pRotationCenter, float Rotation,
		CONST Vector2* pTranslation) {
		return pOut;
	}

	// Build affine transformation matrix.  NULL arguments are treated as identity.
	// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
	static Matrix* D3DXMatrixAffineTransformation(Matrix* pOut, float Scaling, CONST Vector3* pRotationCenter,
		CONST Quaternion* pRotation, CONST Vector3* pTranslation)
	{
		XMVECTOR S = XMVectorReplicate(Scaling);//XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR O = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		if (pRotationCenter != NULL)
		{
			O = DirectX::XMLoadFloat3(pRotationCenter);
		}
		XMVECTOR P = DirectX::XMLoadFloat3(pTranslation);
		XMVECTOR Q = DirectX::XMLoadFloat4(pRotation);
		*pOut = DirectX::XMMatrixAffineTransformation(S, O, Q, P);
		return pOut;
	}

	// Build 2D affine transformation matrix in XY plane.  NULL arguments are treated as identity.
	// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
	static Matrix* D3DXMatrixAffineTransformation2D(Matrix* pOut, float Scaling, CONST Vector2* pRotationCenter,
		float Rotation, CONST Vector2* pTranslation) {
		return pOut;
	}

	// Build a lookat matrix. (right-handed)
	static Matrix* D3DXMatrixLookAtRH(Matrix* pOut, CONST Vector3* pEye, CONST Vector3* pAt, CONST Vector3* pUp) {
		return pOut;
	}

	// Build a lookat matrix. (left-handed)
	static Matrix* D3DXMatrixLookAtLH(Matrix* pOut, CONST Vector3* pEye, CONST Vector3* pAt, CONST Vector3* pUp) {
		*pOut = Matrix::CreateLookAt(*pEye, *pAt, *pUp);
		return pOut;
	}

	// Build a perspective projection matrix. (right-handed)
	static Matrix* D3DXMatrixPerspectiveRH(Matrix* pOut, float w, float h, float zn, float zf) {
		return pOut;
	}

	// Build a perspective projection matrix. (left-handed)
	static Matrix* D3DXMatrixPerspectiveLH(Matrix* pOut, float w, float h, float zn, float zf) {
		*pOut = Matrix::CreatePerspective(w, h, zn, zf);
		return pOut;
	}

	// Build a perspective projection matrix. (right-handed)
	static Matrix* D3DXMatrixPerspectiveFovRH(Matrix* pOut, float fovy, float Aspect, float zn, float zf)
	{
		using namespace DirectX;
		XMStoreFloat4x4(pOut, XMMatrixPerspectiveFovRH(fovy, Aspect, zn, zf));
		return pOut;
	}

	// Build a perspective projection matrix. (left-handed)
	static Matrix* D3DXMatrixPerspectiveFovLH(Matrix* pOut, float fovy, float Aspect, float zn, float zf)
	{
		using namespace DirectX;
		*pOut = Matrix::CreatePerspectiveFieldOfView(fovy, Aspect, zn, zf);
		return pOut;
	}

	// Build a perspective projection matrix. (right-handed)
	static Matrix* D3DXMatrixPerspectiveOffCenterRH(Matrix* pOut, float l, float r, float b, float t, float zn, float zf) {
		return pOut;
	}

	// Build a perspective projection matrix. (left-handed)
	static Matrix* D3DXMatrixPerspectiveOffCenterLH(Matrix* pOut, float l, float r, float b, float t, float zn, float zf) {
		*pOut = Matrix::CreatePerspectiveOffCenter(l, r, b, t, zn, zf);
		return pOut;
	}

	// Build an ortho projection matrix. (right-handed)
	static Matrix* D3DXMatrixOrthoRH(Matrix* pOut, float w, float h, float zn, float zf) {
		return pOut;
	}

	// Build an ortho projection matrix. (left-handed)
	static Matrix* D3DXMatrixOrthoLH(Matrix* pOut, float w, float h, float zn, float zf) {
		*pOut = Matrix::CreateOrthographic(w, h, zn, zf);
		return pOut;
	}

	// Build an ortho projection matrix. (right-handed)
	static Matrix* D3DXMatrixOrthoOffCenterRH(Matrix* pOut, float l, float r, float b, float t, float zn, float zf) {
		return pOut;
	}

	// Build an ortho projection matrix. (left-handed)
	static Matrix* D3DXMatrixOrthoOffCenterLH(Matrix* pOut, float l, float r, float b, float t, float zn, float zf) {
		*pOut = Matrix::CreateOrthographicOffCenter(l, r, b, t, zn, zf);
		return pOut;
	}

	// Build a matrix which flattens geometry into a plane, as if casting
	// a shadow from a light.
	static Matrix* D3DXMatrixShadow(Matrix* pOut, CONST Vector4* pLight, CONST Plane* pPlane)
	{
		CONST Vector3 pLightLight = Vector3(pLight->x, pLight->y, pLight->z);
		*pOut = Matrix::CreateShadow(pLightLight, *pPlane);
		return pOut;
	}

	// Build a matrix which reflects the coordinate system about a plane
	static Matrix* D3DXMatrixReflect(Matrix* pOut, CONST Plane* pPlane) {
		*pOut = Matrix::CreateReflection(*pPlane);
		return pOut;
	}

	//--------------------------
	// Quaternion
	//--------------------------

	// inline

	static float D3DXQuaternionLength(CONST Quaternion* pQ)
	{
		return pQ->Length();
	};

	// Length squared, or "norm"
	static float D3DXQuaternionLengthSq(CONST Quaternion* pQ)
	{
		return pQ->LengthSquared();
	};

	static float D3DXQuaternionDot(CONST Quaternion* pQ1, CONST Quaternion* pQ2)
	{
		return pQ1->Dot(*pQ2);;
	};

	// (0, 0, 0, 1)
	static Quaternion* D3DXQuaternionIdentity(Quaternion* pOut) {
		//*pOut = TQuaternion::Identity;
		XMStoreFloat4(pOut, XMQuaternionIdentity());
		return pOut;
	};

	static BOOL D3DXQuaternionIsIdentity(CONST Quaternion* pQ) {
		return TRUE;
	};

	// (-x, -y, -z, w)
	static Quaternion* D3DXQuaternionConjugate(Quaternion* pOut, CONST Quaternion* pQ) {
		return pOut;
	};


	// Compute a quaternin's axis and angle of rotation. Expects unit quaternions.
	static void D3DXQuaternionToAxisAngle(CONST Quaternion* pQ, Vector3* pAxis, float* pAngle)
	{

	};

	static Quaternion* D3DXQuaternionRotationAxis(Quaternion* pOut, CONST Vector3* pV, FLOAT Angle)
	{
		*pOut = Quaternion::CreateFromAxisAngle(*pV, Angle);
		return pOut;
	}

	// Yaw around the Y axis, a pitch around the X axis,
	// and a roll around the Z axis.
	static Quaternion* D3DXQuaternionRotationYawPitchRoll(Quaternion* pOut, float Yaw, float Pitch, float Roll)
	{
		*pOut = Quaternion::CreateFromYawPitchRoll(Yaw, Pitch, Roll);
		return pOut;
	};

	// Quaternion multiplication.  The result represents the rotation Q2
	// followed by the rotation Q1.  (Out = Q2 * Q1)
	static Quaternion* D3DXQuaternionMultiply(Quaternion* pOut, CONST Quaternion* pQ1,
		CONST Quaternion* pQ2)
	{
		*pOut = *pQ1 * *pQ2;
		return pOut;
	};

	static Quaternion* D3DXQuaternionNormalize(Quaternion* pOut, CONST Quaternion* pQ)
	{
		*pOut = *pQ;
		pOut->Normalize();
		return pOut;
	};

	// Conjugate and re-norm
	static Quaternion* D3DXQuaternionInverse(Quaternion* pOut, CONST Quaternion* pQ)
	{
		Quaternion qRet = *pQ;
		pQ->Inverse(qRet);
		*pOut = qRet;
		return pOut;
	};

	// Expects unit quaternions.
	// if q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)
	static Quaternion* D3DXQuaternionLn(Quaternion* pOut, CONST Quaternion* pQ) {
		return pOut;
	};

	// Expects pure quaternions. (w == 0)  w is ignored in calculation.
	// if q = (0, theta * v); exp(q) = (cos(theta), sin(theta) * v)
	static Quaternion* D3DXQuaternionExp(Quaternion* pOut, CONST Quaternion* pQ) {
		return pOut;
	};

	// Spherical linear interpolation between Q1 (t == 0) and Q2 (t == 1).
	// Expects unit quaternions.
	static Quaternion* D3DXQuaternionSlerp(Quaternion* pOut, CONST Quaternion* pQ1, CONST Quaternion* pQ2, float t)
	{
		*pOut = Quaternion::Slerp(*pQ1, *pQ2, t);
		return pOut;
	};

	// Spherical quadrangle interpolation.
	// Slerp(Slerp(Q1, C, t), Slerp(A, B, t), 2t(1-t))
	static Quaternion* D3DXQuaternionSquad(Quaternion* pOut, CONST Quaternion* pQ1,
		CONST Quaternion* pA, CONST Quaternion* pB,
		CONST Quaternion* pC, float t) {
		return pOut;
	};

	// Setup control points for spherical quadrangle interpolation
	// from Q1 to Q2.  The control points are chosen in such a way 
	// to ensure the continuity of tangents with adjacent segments.
	static void D3DXQuaternionSquadSetup(Quaternion* pAOut, Quaternion* pBOut, Quaternion* pCOut,
		CONST Quaternion* pQ0, CONST Quaternion* pQ1,
		CONST Quaternion* pQ2, CONST Quaternion* pQ3) {
	};

	// Barycentric interpolation.
	// Slerp(Slerp(Q1, Q2, f+g), Slerp(Q1, Q3, f+g), g/(f+g))
	static Quaternion* D3DXQuaternionBaryCentric(Quaternion* pOut, CONST Quaternion* pQ1,
		CONST Quaternion* pQ2, CONST Quaternion* pQ3,
		float f, float g) {
		return pOut;
	};


	//--------------------------
	// Plane
	//--------------------------

	// inline

	// ax + by + cz + dw
	static float D3DXPlaneDot(CONST Plane* pP, CONST Vector4* pV) {
		return 0.0f;
	};
	// ax + by + cz + d
	static float D3DXPlaneDotCoord(CONST Plane* pP, CONST Vector3* pV) {
		return 0.0f;
	};
	// ax + by + cz
	static float D3DXPlaneDotNormal(CONST Plane* pP, CONST Vector3* pV) {
		return 0.0f;
	};
	static Plane* D3DXPlaneScale(Plane* pOut, CONST Plane* pP, float s) {
		return pOut;
	};


	// Normalize plane (so that |a,b,c| == 1)
	static Plane* D3DXPlaneNormalize(Plane* pOut, CONST Plane* pP) {
		return pOut;
	};

	// Find the intersection between a plane and a line.  If the line is
	// parallel to the plane, NULL is returned.
	static Vector3* D3DXPlaneIntersectLine(Vector3* pOut, CONST Plane* pP, CONST Vector3* pV1,
		CONST Vector3* pV2) {
		return pOut;
	};

	// Construct a plane from a point and a normal
	static Plane* D3DXPlaneFromPointNormal(Plane* pOut, CONST Vector3* pPoint, CONST Vector3* pNormal) {
		return pOut;
	};

	// Construct a plane from 3 points
	static Plane* D3DXPlaneFromPoints(Plane* pOut, CONST Vector3* pV1, CONST Vector3* pV2,
		CONST Vector3* pV3) {
		return pOut;
	};

	// Transform a plane by a matrix.  The vector (a,b,c) must be normal.
	// M should be the inverse transpose of the transformation desired.
	static Plane* D3DXPlaneTransform(Plane* pOut, CONST Plane* pP, CONST Matrix* pM) {
		return pOut;
	};
	// Transform an array of planes by a matrix.  The vectors (a,b,c) must be normal.
// M should be the inverse transpose of the transformation desired.
	static Plane* D3DXPlaneTransformArray(Plane* pOut, UINT OutStride, CONST Plane* pP, UINT PStride, CONST Matrix* pM, UINT n) {
		return pOut;
	};



	//--------------------------
	// Color
	//--------------------------

	// inline

	// (1-r, 1-g, 1-b, a)
	static Color* D3DXColorNegative(Color* pOut, CONST Color* pC) {
		return pOut;
	};

	static Color* D3DXColorAdd(Color* pOut, CONST Color* pC1, CONST Color* pC2) {
		return pOut;
	};

	static Color* D3DXColorSubtract(Color* pOut, CONST Color* pC1, CONST Color* pC2) {
		return pOut;
	};

	static Color* D3DXColorScale(Color* pOut, CONST Color* pC, float s) {
		return pOut;
	};

	// (r1*r2, g1*g2, b1*b2, a1*a2)
	static Color* D3DXColorModulate(Color* pOut, CONST Color* pC1, CONST Color* pC2) {
		return pOut;
	};

	// Linear interpolation of r,g,b, and a. C1 + s(C2-C1)
	static Color* D3DXColorLerp(Color* pOut, CONST Color* pC1, CONST Color* pC2, float s) {
		return pOut;
	};


	// Interpolate r,g,b between desaturated color and color.
	// DesaturatedColor + s(Color - DesaturatedColor)
	static Color* D3DXColorAdjustSaturation(Color* pOut, CONST Color* pC, float s) {
		return pOut;
	};
	// Interpolate r,g,b between 50% grey and color.  Grey + s(Color - Grey)
	static Color* D3DXColorAdjustContrast(Color* pOut, CONST Color* pC, float c) {
		return pOut;
	};





	//--------------------------
	// Misc
	//--------------------------
		// Calculate Fresnel term given the cosine of theta (likely obtained by
		// taking the dot of two normals), and the refraction index of the material.
	static float D3DXFresnelTerm(float CosTheta, float RefractionIndex)
	{
		return 0.0f;
	};
}

//  ------------------------------------------------------------------------------
// Support for TMath and Standard C++ Library containers
namespace std
{
	template<> struct less<BASIS_EX::Rectangle>
	{
		bool operator()(const BASIS_EX::Rectangle& r1, const BASIS_EX::Rectangle& r2) const
		{
			return ((r1.x < r2.x)
				|| ((r1.x == r2.x) && (r1.y < r2.y))
				|| ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width < r2.width))
				|| ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width == r2.width) && (r1.height < r2.height)));
		}
	};

	template<> struct less<BASIS_EX::Vector2>
	{
		bool operator()(const BASIS_EX::Vector2& V1, const BASIS_EX::Vector2& V2) const
		{
			return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)));
		}
	};

	template<> struct less<BASIS_EX::Vector3>
	{
		bool operator()(const BASIS_EX::Vector3& V1, const BASIS_EX::Vector3& V2) const
		{
			return ((V1.x < V2.x)
				|| ((V1.x == V2.x) && (V1.y < V2.y))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z)));
		}
	};

	template<> struct less<BASIS_EX::Vector4>
	{
		bool operator()(const BASIS_EX::Vector4& V1, const BASIS_EX::Vector4& V2) const
		{
			return ((V1.x < V2.x)
				|| ((V1.x == V2.x) && (V1.y < V2.y))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z == V2.z) && (V1.w < V2.w)));
		}
	};

	template<> struct less<BASIS_EX::Matrix>
	{
		bool operator()(const BASIS_EX::Matrix& M1, const BASIS_EX::Matrix& M2) const
		{
			if (M1._11 != M2._11) return M1._11 < M2._11;
			if (M1._12 != M2._12) return M1._12 < M2._12;
			if (M1._13 != M2._13) return M1._13 < M2._13;
			if (M1._14 != M2._14) return M1._14 < M2._14;
			if (M1._21 != M2._21) return M1._21 < M2._21;
			if (M1._22 != M2._22) return M1._22 < M2._22;
			if (M1._23 != M2._23) return M1._23 < M2._23;
			if (M1._24 != M2._24) return M1._24 < M2._24;
			if (M1._31 != M2._31) return M1._31 < M2._31;
			if (M1._32 != M2._32) return M1._32 < M2._32;
			if (M1._33 != M2._33) return M1._33 < M2._33;
			if (M1._34 != M2._34) return M1._34 < M2._34;
			if (M1._41 != M2._41) return M1._41 < M2._41;
			if (M1._42 != M2._42) return M1._42 < M2._42;
			if (M1._43 != M2._43) return M1._43 < M2._43;
			if (M1._44 != M2._44) return M1._44 < M2._44;

			return false;
		}
	};

	template<> struct less<BASIS_EX::Plane>
	{
		bool operator()(const BASIS_EX::Plane& P1, const BASIS_EX::Plane& P2) const
		{
			return ((P1.x < P2.x)
				|| ((P1.x == P2.x) && (P1.y < P2.y))
				|| ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z < P2.z))
				|| ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z) && (P1.w < P2.w)));
		}
	};

	template<> struct less<BASIS_EX::Quaternion>
	{
		bool operator()(const BASIS_EX::Quaternion& Q1, const BASIS_EX::Quaternion& Q2) const
		{
			return ((Q1.x < Q2.x)
				|| ((Q1.x == Q2.x) && (Q1.y < Q2.y))
				|| ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z < Q2.z))
				|| ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z == Q2.z) && (Q1.w < Q2.w)));
		}
	};

	template<> struct less<BASIS_EX::Color>
	{
		bool operator()(const BASIS_EX::Color& C1, const BASIS_EX::Color& C2) const
		{
			return ((C1.x < C2.x)
				|| ((C1.x == C2.x) && (C1.y < C2.y))
				|| ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z < C2.z))
				|| ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z == C2.z) && (C1.w < C2.w)));
		}
	};

	template<> struct less<BASIS_EX::Ray>
	{
		bool operator()(const BASIS_EX::Ray& R1, const BASIS_EX::Ray& R2) const
		{
			if (R1.position.x != R2.position.x) return R1.position.x < R2.position.x;
			if (R1.position.y != R2.position.y) return R1.position.y < R2.position.y;
			if (R1.position.z != R2.position.z) return R1.position.z < R2.position.z;

			if (R1.direction.x != R2.direction.x) return R1.direction.x < R2.direction.x;
			if (R1.direction.y != R2.direction.y) return R1.direction.y < R2.direction.y;
			if (R1.direction.z != R2.direction.z) return R1.direction.z < R2.direction.z;

			return false;
		}
	};

	template<> struct less<BASIS_EX::Viewport>
	{
		bool operator()(const BASIS_EX::Viewport& vp1, const BASIS_EX::Viewport& vp2) const
		{
			if (vp1.x != vp2.x) return (vp1.x < vp2.x);
			if (vp1.y != vp2.y) return (vp1.y < vp2.y);

			if (vp1.width != vp2.width) return (vp1.width < vp2.width);
			if (vp1.height != vp2.height) return (vp1.height < vp2.height);

			if (vp1.minDepth != vp2.minDepth) return (vp1.minDepth < vp2.minDepth);
			if (vp1.maxDepth != vp2.maxDepth) return (vp1.maxDepth < vp2.maxDepth);

			return false;
		}
	};


};
using namespace BASIS_EX;
