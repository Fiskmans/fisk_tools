#pragma once

#include "tools/MathVector.h"

#include <optional>

#include <cmath>

namespace fisk::tools
{
	template<typename T, size_t Dimensions>
	struct Planar
	{
		MathVector<T, Dimensions> myNormal;
		T myDistance;

		static Planar FromPointandNormal(MathVector<T, Dimensions> aPoint, MathVector<T, Dimensions> aNormal);
	};

	template<typename T, size_t Dimensions>
	struct Spheroid
	{
		MathVector<T, Dimensions> myCenter;
		T myRadius;
	};

	template<typename T, size_t Dimensions>
	struct Ray
	{
		MathVector<T, Dimensions> myOrigin;
		MathVector<T, Dimensions> myDirection;

		static Ray FromPointandTarget(MathVector<T, Dimensions> aPoint, MathVector<T, Dimensions> aTarget);

		MathVector<T, Dimensions> PointAt(T aDistance);
	};

	template<typename T>
	struct Tri
	{
		MathVector<T, 3> myOrigin;
		MathVector<T, 3> mySideA;
		MathVector<T, 3> mySideB;

		static Tri FromCorners(MathVector<T, 3> aA, MathVector<T, 3> aB, MathVector<T, 3> aC);
	};

	template<typename T>
	using Line = Planar<T, 2>;

	template<typename T>
	using Plane = Planar<T, 3>;

	template<typename T>
	using HyperPlane = Planar<T, 4>;

	template<typename T>
	using Circle = Spheroid<T, 2>;

	template<typename T>
	using Sphere = Spheroid<T, 3>;

	template<typename T, size_t Dimensions>
	std::optional<float> Intersect(Ray<T, Dimensions> aRay, Spheroid<T, Dimensions> aSpheroid)
	{
		if (aRay.myOrigin.DistanceSqr(aSpheroid.myCenter) < aSpheroid.myRadius * aSpheroid.myRadius + 0.0001f)
			return {};

		tools::MathVector<T, 3> delta = aRay.myOrigin - aSpheroid.myCenter;

		T a = aRay.myDirection.Dot(aRay.myDirection);
		T b = 2 * aRay.myDirection.Dot(delta);
		T c = delta.Dot(delta) - (aSpheroid.myRadius * aSpheroid.myRadius);

		T discr = b * b - 4 * a * c;
		if (discr < 0)
			return {};

		T distance = ((-b - std::sqrt(discr)) / (2 * a));

		if (distance < 0)
			return {};

		return distance;
	}

	template<typename T, size_t Dimensions>
	std::optional<float> Intersect(Ray<T, Dimensions> aRay, Planar<T, Dimensions> aPlanar)
	{
		T denom = aPlanar.myNormal.Dot(aRay.myDirection);

		if (denom > -0.0001f)
			return {};

		MathVector<T, Dimensions> knownPoint = aPlanar.myNormal * aPlanar.myDistance;

		T distance = (knownPoint - aRay.myOrigin).Dot(aPlanar.myNormal) / denom;

		if (distance < 0)
			return {};

		return distance;
	}

	template<typename T>
	std::optional<float> Intersect(Ray<T, 3> aRay, Tri<T> aTri)
	{	
		MathVector<T, 3> rayXEdgeB = aRay.myDirection.Cross(aTri.mySideB);

		T det = aTri.mySideA.Dot(rayXEdgeB);

		constexpr T eps = 0.00001;

		if (det > -eps && det < eps)
			return {};

		T inverseDet = 1.0 / det;
		MathVector<T, 3> delta = aRay.myOrigin - aTri.myOrigin;

		T u = inverseDet * delta.Dot(aRay.myDirection);

		if (u < 0 || u > 1)
			return {};

		MathVector<T, 3> deltaXEdgeA = delta.Cross(aTri.mySideA);
		T v = inverseDet * aRay.myDirection.Dot(deltaXEdgeA);

		if (v < 0 || u + v > 1)
			return {};

		T t = inverseDet * aTri.mySideB.Dot(deltaXEdgeA);

		if (t < eps)
			return {};

		return t;
	}

	template<typename T, size_t Dimensions>
	inline Planar<T, Dimensions> Planar<T, Dimensions>::FromPointandNormal(MathVector<T, Dimensions> aPoint, MathVector<T, Dimensions> aNormal)
	{
		Planar out;

		out.myNormal = aNormal;
		out.myDistance = aNormal.Dot(aPoint);

		return out;
	}

	template<typename T, size_t Dimensions>
	inline Ray<T, Dimensions> Ray<T, Dimensions>::FromPointandTarget(MathVector<T, Dimensions> aPoint, MathVector<T, Dimensions> aTarget)
	{
		Ray out;

		out.myOrigin = aPoint;
		out.myDirection = aTarget - aPoint;
		out.myDirection.Normalize();

		return out;
	}

	template<typename T, size_t Dimensions>
	inline MathVector<T, Dimensions> Ray<T, Dimensions>::PointAt(T aDistance)
	{
		return myOrigin + myDirection * aDistance;
	}

	template<typename T>
	inline Tri<T> Tri<T>::FromCorners(MathVector<T, 3> aA, MathVector<T, 3> aB, MathVector<T, 3> aC)
	{
		Tri out;

		out.myOrigin = aA;
		out.mySideA = aB - aA;
		out.mySideB = aC - aA;

		return out;
	}
}

