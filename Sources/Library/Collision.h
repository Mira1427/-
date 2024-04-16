#pragma once

#include "Vector.h"

// ===== �����蔻��N���X ===================================================================================================================
class Collision
{
public:
	// --- ��`���m�̌������� ---
	static bool intersectRectangles(
		const Vector2& centerA, const Vector2& sizeA,
		const Vector2& centerB, const Vector2& sizeB
	);

	// --- �~���m�̌������� ---
	static bool intersectCircles(
		const Vector2& centerA, float radiusA,
		const Vector2& centerB, float radiusB
	);

	// --- �����̓��m�̌������� ---
	static bool intersectBoxes(
		const Vector3& positionA,
		const Vector3& sizeA,
		const Vector3& positionB,
		const Vector3& sizeB
	);

	// --- �����m�̌������� ---
	static bool intersectSpheres(
		const Vector3& positionA,
		float radiusA,
		const Vector3& positionB,
		float radiusB
	);


	// --- �~�����m�̌������� ---
	static bool intersectCapsules(
		const Vector3& positionA, float radiusA, float heightA,
		const Vector3& positionB, float radiusB, float heightB
	);
};

