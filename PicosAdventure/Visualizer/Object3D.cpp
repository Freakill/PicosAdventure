#include "Object3D.h"

void Object3D::setName(std::string newName)
{
	objectName_ = newName;
}

std::string Object3D::getName()
{
	return objectName_;
}

void Object3D::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point Object3D::getPosition()
{
	return position_;
}

void Object3D::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector Object3D::getScale()
{
	return scaling_;
}

void Object3D::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float Object3D::getRotationX()
{
	return rotX_;
}

void Object3D::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float Object3D::getRotationY()
{
	return rotY_;
}

void Object3D::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float Object3D::getRotationZ()
{
	return rotZ_;
}