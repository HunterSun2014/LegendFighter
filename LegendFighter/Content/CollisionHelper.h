#pragma once

class BoundingShape
{
public:
	void SetBoundingBox(const DirectX::BoundingBox & boundingBox)
	{
		box = boundingBox;

		Type = BOX;
	}

	const DirectX::BoundingBox & GetBoundBox() { return box; }

	void SetBoundingSphere(const DirectX::BoundingSphere & boundingSphere)
	{
		sphere = boundingSphere;

		Type = SPHERE;
	}

	const DirectX::BoundingSphere & GetBoundingSphere() { return sphere; }

	enum BoundingType { SPHERE, BOX, ORIENTBOX, FRUSTUM };

	BoundingType GetBoundingType() { return Type; }

private:

	BoundingType Type;

	DirectX::BoundingSphere			sphere;
	DirectX::BoundingBox			box;
	DirectX::BoundingOrientedBox	orientBox;
	DirectX::BoundingFrustum		frustum;
};

class CollisionHelper : DirectX::GeometricPrimitive
{
public:
	/*void DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color);
	void DrawFrustum(const BoundingFrustum& frustum, FXMVECTOR color);
	void DrawAabb(const BoundingBox& box, FXMVECTOR color);
	void DrawObb(const BoundingOrientedBox& obb, FXMVECTOR color);
	void DrawSphere(const BoundingSphere& sphere, FXMVECTOR color);
	void DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color);
	void DrawTriangle(FXMVECTOR PointA, FXMVECTOR PointB, FXMVECTOR PointC, CXMVECTOR color);*/

private:

};

