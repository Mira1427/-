#include "Collision.h"

// --- ‹éŒ`“¯m‚ÌŒğ·”»’è ---
bool Collision::intersectRectangles(
    const Vector2& centerA, const Vector2& sizeA,
    const Vector2& centerB, const Vector2& sizeB)
{
    Vector2 leftTopA = centerA - sizeA * 0.5f;
    Vector2 rightBottomA = centerA + sizeA * 0.5f;

    Vector2 leftTopB = centerB - sizeB * 0.5f;
    Vector2 rightBottomB = centerB + sizeB * 0.5f;


    if (leftTopA.y     < rightBottomB.y   &&
        rightBottomA.y > leftTopB.y       &&
        leftTopA.x     < rightBottomB.x   &&
        rightBottomA.x > leftTopB.x)
        return true;

    return false;
}


// --- ‰~“¯m‚ÌŒğ·”»’è ---
bool Collision::intersectCircles(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB)
{
    float length = Vector2(centerA - centerB).length();

    if (length > radiusA + radiusB)
        return false;

    return true;
}


// --- —§•û‘Ì“¯m‚ÌŒğ·”»’è ---
bool Collision::intersectBoxes(const Vector3& positionA, const Vector3& sizeA, const Vector3& positionB, const Vector3& sizeB)
{
    Vector3 rightTopBackA       = positionA + sizeA * 0.5f;
    Vector3 leftBottomFrontA    = positionA - sizeA * 0.5f;

    Vector3 rightTopBackB       = positionB + sizeB * 0.5f;
    Vector3 leftBottomFrontB    = positionB - sizeB * 0.5f;

    if (leftBottomFrontA.x < rightTopBackB.x      &&
        rightTopBackA.x    > leftBottomFrontB.x   &&
        leftBottomFrontA.y < rightTopBackB.y      &&
        rightTopBackA.y    > leftBottomFrontB.y   &&
        leftBottomFrontA.z < rightTopBackB.z      &&
        rightTopBackA.z    > leftBottomFrontB.z)
        return true;

    return false;
}


// --- ‹…“¯m‚ÌŒğ·”»’è ---
bool Collision::intersectSpheres(
    const Vector3& positionA, float radiusA,
    const Vector3& positionB, float radiusB)
{
    Vector3 vec = positionB - positionA;    // A‚©‚çB‚Ö‚ÌƒxƒNƒgƒ‹‚ğZo

    // --- ‹——£”»’è ---
    float lengthSq = vec.length();
    float range = radiusA + radiusB;

    if (range < lengthSq)
        return false;

    return true;
}


// --- ‰~’Œ“¯m‚ÌŒğ·”»’è ---
bool Collision::intersectCapsules(
    const Vector3& positionA, float radiusA, float heightA,
    const Vector3& positionB, float radiusB, float heightB)
{
    // --- ‚‚³”»’è ---
    if (positionA.y > positionB.y + heightB)
        return false;

    if (positionA.y + heightA < positionB.y)
        return false;

    // --- ‹——£”»’è ---
    Vector2 vec = positionB.xz() - positionA.xz();
    float length = vec.length();
    float range = radiusA + radiusB;

    if (range < length)
        return false;

    return true;
}
