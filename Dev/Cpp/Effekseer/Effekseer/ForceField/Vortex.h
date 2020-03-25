#pragma once

#include <array>
#include <random>

#include "../Effekseer.Vector3D.h"
#include "../Noise/CurlNoise.h"
#include "../Noise/PerlinNoise.h"
#include "../SIMD/Effekseer.Mat44f.h"
#include "../SIMD/Effekseer.Vec3f.h"

namespace Effekseer
{

struct ForceFieldCommonParameter
{
	const Vec3f& Position;
	const Vec3f& PreviousVelocity;
	const Vec3f& PreviousSumVelocity;
	const Vec3f& FieldCenter;
	const Mat44f& FieldRotation;
	const bool IsFieldRotated = false;
};

struct ForceFieldFalloffCommonParameter
{
	float Power = 0.0f;
	float MinDistance = 0.0f;
	float MaxDistance = 0.0f;
};

struct ForceFieldFalloffSphereParameter
{
};

struct ForceFieldFalloffTubeParameter
{
	float RadiusPower = 0.0f;
	float MinRadius = 0.0f;
	float MaxRadius = 0.0f;
};

struct ForceFieldFalloffConeParameter
{
	float Power = 0.0f;
	float MinAngle = 0.0f;
	float MaxAngle = 0.0f;
};

struct ForceFieldForceParameter
{
	// Shape
	float Power;
	bool Gravitation;
};

struct ForceFieldWindParameter
{
	// Shape
	float Power;
};

struct ForceFieldVortexParameter
{
	// Shape
	float Power;
};

struct ForceFieldMagineticParameter
{
	// Shape
	float Power;
};

struct ForceFieldChargeParameter
{
	// Shape
	float Power;
};

struct ForceFieldTurbulenceParameter
{
	float Power;
	CurlNoise Noise;
};

struct ForceFieldDragParameter
{
	float Power;
};

class ForceFieldFalloff
{
public:
	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffSphereParameter& fffs)
	{
		// Sphere
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength();
		if (distance > fffc.MaxDistance)
		{
			return power;
		}

		if (distance < fffc.MinDistance)
		{
			return power;
		}

		return power * fffc.Power;
	}

	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffTubeParameter& ffft)
	{
		// Sphere
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength();
		if (distance > fffc.MaxDistance)
		{
			return power;
		}

		if (distance < fffc.MinDistance)
		{
			return power;
		}

		// Tube
		auto tubePos = localPos;
		if (ffc.IsFieldRotated)
		{
			// TODO
			// tubePos = tubePos - Vec3f::Dot(tubePos, ffc.FieldRotation.)
		}
		else
		{
			tubePos.SetY(0);
		}

		auto tubeRadius = tubePos.GetLength();

		if (tubeRadius > ffft.MaxRadius)
		{
			return power;
		}

		if (tubeRadius < ffft.MinRadius)
		{
			return power;
		}

		return power * fffc.Power;
	}

	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffTubeParameter& ffft)
	{
		// Šp“x‚ðŒvŽZ‚·‚é
		return power;
	}
};

class ForceField
{
	Vec3f
	GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldFalloffCommonParameter& fffc, const ForceFieldForceParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto dir = localPos / distance;

		if (ffp.Gravitation)
		{
			return dir * ffp.Power / distance;
		}

		return dir * ffp.Power;
	}

	Vec3f
	GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldFalloffCommonParameter& fffc, const ForceFieldWindParameter& ffp)
	{
		auto dir = Vec3f(0, 1, 0);
		return dir * ffp.Power;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc,
						  const ForceFieldFalloffCommonParameter& fffc,
						  const ForceFieldVortexParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto axis = Vec3f(0, 1, 0);
		Vec3f front = Vec3f::Cross(axis, localPos);
		front.Normalize();

		return front * ffp.Power;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc,
						  const ForceFieldFalloffCommonParameter& fffc,
						  const ForceFieldMagineticParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto dir = localPos / distance;

		auto forceDir = Vec3f::Cross(ffc.PreviousSumVelocity, dir);

		if (forceDir.GetSquaredLength() < 0.01f)
			return Vec3f(0.0f, 0.0f, 0.0f);

		return forceDir * ffp.Power;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc,
						  const ForceFieldFalloffCommonParameter& fffc,
						  const ForceFieldChargeParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto dir = localPos / distance;

		if (dir.GetLength() < ffp.Power)
		{
			return -ffc.PreviousSumVelocity;
		}

		return -dir * ffp.Power;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc,
						  const ForceFieldFalloffCommonParameter& fffc,
						  const ForceFieldTurbulenceParameter& ffp)
	{

	}

	Vec3f
	GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldFalloffCommonParameter& fffc, const ForceFieldDragParameter& ffp)
	{
		return -ffc.PreviousSumVelocity * ffp.Power;
	}
};

} // namespace Effekseer
