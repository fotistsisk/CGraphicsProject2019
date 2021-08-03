#pragma once
#include <math.h>

class SphereIntersection
{
private:
	float x, y, z, radius;

	float center_x,center_y,center_z;

public:
	SphereIntersection();
	~SphereIntersection();

	float getX();
	float getY();
	float getZ();
	float getRadius();
	bool isIntersection(SphereIntersection * sphere);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setRadius(float rad);

	float getCenterX();
	float getCenterY();
	float getCenterZ();
	void setCenterX(float x);
	void setCenterY(float y);
	void setCenterZ(float z);
};