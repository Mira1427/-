#pragma once

#include "Vector.h"

// ===== “–‚½‚è”»’èƒNƒ‰ƒX ===================================================================================================================
class Collision
{
public:
	// --- ‹éŒ`“¯m‚ÌŒğ·”»’è ---
	static bool intersectRectangles(
		const Vector2& centerA, const Vector2& sizeA,
		const Vector2& centerB, const Vector2& sizeB
	);

	// --- ‰~“¯m‚ÌŒğ·”»’è ---
	static bool intersectCircles(
		const Vector2& centerA, float radiusA,
		const Vector2& centerB, float radiusB
	);

	// --- —§•û‘Ì“¯m‚ÌŒğ·”»’è ---
	static bool intersectBoxes(
		const Vector3& positionA,
		const Vector3& sizeA,
		const Vector3& positionB,
		const Vector3& sizeB
	);

	// --- ‹…“¯m‚ÌŒğ·”»’è ---
	static bool intersectSpheres(
		const Vector3& positionA,
		float radiusA,
		const Vector3& positionB,
		float radiusB
	);


	// --- ‰~’Œ“¯m‚ÌŒğ·”»’è ---
	static bool intersectCapsules(
		const Vector3& positionA, float radiusA, float heightA,
		const Vector3& positionB, float radiusB, float heightB
	);
};

