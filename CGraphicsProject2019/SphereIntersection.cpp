#include "SphereIntersection.h"


SphereIntersection::SphereIntersection()
{
	
}
SphereIntersection::~SphereIntersection(){
}

float SphereIntersection::getX(){return x;}
float SphereIntersection::getY(){return y;}
float SphereIntersection::getZ(){return z;}
float SphereIntersection::getRadius() { return radius; }
void SphereIntersection::setRadius(float rad) { this->radius = rad; }
bool SphereIntersection::isIntersection(SphereIntersection * sphere){
	auto distance = sqrt( pow(x - sphere->getX(), 2) + pow(y - sphere->getY(), 2) + pow(z - sphere->getZ(), 2));
	return (distance <= this->radius + sphere->getRadius());
}
void SphereIntersection::setX(float x){this->x = x + center_x;}
void SphereIntersection::setY(float y){this->y = y + center_y;}
void SphereIntersection::setZ(float z){this->z = z + center_z;}


float SphereIntersection::getCenterX() { return center_x; }
float SphereIntersection::getCenterY() { return center_y; }
float SphereIntersection::getCenterZ() { return center_z; }
void SphereIntersection::setCenterX(float x){this->center_x = x;}
void SphereIntersection::setCenterY(float y){this->center_y = y;}
void SphereIntersection::setCenterZ(float z) { this->center_z = z; }