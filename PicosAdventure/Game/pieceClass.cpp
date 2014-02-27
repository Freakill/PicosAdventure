#include "pieceClass.h"

PieceClass::PieceClass()
{
	model_ = 0;

	initialPosition_.x = 0.0f;
	initialPosition_.y = 0.0f;
	initialPosition_.z = 0.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;

	initialScaling_.x = 1.0f;
	initialScaling_.y = 1.0f;
	initialScaling_.z = 1.0f;

	spawningTime_ = 0.0f;
	waitedTime_ = 0.0f;

	lightedTime_ = 0.0f;
	fallTime_ = 0.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;
}

PieceClass::PieceClass(const PieceClass& other)
{
}

PieceClass::~PieceClass()
{
}

bool PieceClass::setup(GraphicsManager *graphicsManager, std::string fileName, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

	name_ = fileName;

	initialPosition_.x = position.x;
	initialPosition_.y = position.y;
	initialPosition_.z = position.z;

	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	endScaling_.x = scaling.x;
	endScaling_.y = scaling.y;
	endScaling_.z = scaling.z;

	initialScaling_.x = 0.0001f;
	initialScaling_.y = 0.0001f;
	initialScaling_.z = 0.0001f;

	scaling_.x = initialScaling_.x;
	scaling_.y = initialScaling_.y;
	scaling_.z = initialScaling_.z;

	spawningTime_ = 2.0f;

	lightLevel_ = 0.0f;

	fallTime_ = 1.0f;
	shaken_ = false;
	
	rotX_ = rotX;
	rotY_ = rotY; 
	rotZ_ = rotZ;

	pieceState_ = SPAWNING;

	floorHeight_ = floorHeight;

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 0.4f);

	hasFallen_ = false;

	return true;
}

void PieceClass::update(float elapsedTime)
{
	collisionTest_->setPosition(position_);

	switch(pieceState_)
	{
		case SPAWNING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > spawningTime_)
				{
					scaling_.x = endScaling_.x;
					scaling_.y = endScaling_.y;
					scaling_.z = endScaling_.z;

					lightedTime_ = 0;

					pieceState_ = IN_TREE;
				}
				else
				{
					scaling_.x = (waitedTime_*endScaling_.x)/spawningTime_;
					scaling_.y = (waitedTime_*endScaling_.y)/spawningTime_;
					scaling_.z = (waitedTime_*endScaling_.z)/spawningTime_;
				}
			}
			break;
		case IN_TREE:
			{
				if(shaken_)
				{
					lightedTime_ += elapsedTime;
					shaken_ = false;
					return;
				}
				else
				{
					lightedTime_ -= 0.008;
					if(lightedTime_ < 0.0)
					{
						lightedTime_ = 0.0f;
					}
				}
			}
			break;
		case FALLING:
			{
				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(position_.y < floorHeight_)
				{
					position_.y = floorHeight_;
					pieceState_ = IN_FLOOR;
					notifyListeners(position_);
				}
			}
			break;
		case IN_FLOOR:
			{
				
			}
			break;
	}
}

void PieceClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		graphicsManager->turnOnWireframeRasterizer();
		collisionTest_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnOnSolidRasterizer();
	}

	XMFLOAT4X4 rotatingMatrixZ;
	XMStoreFloat4x4(&rotatingMatrixZ, XMMatrixRotationZ(rotZ_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixZ)));

	XMFLOAT4X4 rotatingMatrixY;
	XMStoreFloat4x4(&rotatingMatrixY, XMMatrixRotationY(rotY_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixY)));

	XMFLOAT4X4 rotatingMatrixX;
	XMStoreFloat4x4(&rotatingMatrixX, XMMatrixRotationX(rotX_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixX)));

	XMFLOAT4X4 scalingMatrix;
	XMStoreFloat4x4(&scalingMatrix, XMMatrixScaling(scaling_.x, scaling_.y, scaling_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&scalingMatrix)));

	XMFLOAT4X4 movingMatrix;
	XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(position_.x, position_.y, position_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&movingMatrix)));

	LightClass tempLight;
	tempLight.setAmbientColor(light->getAmbientColor().x, light->getAmbientColor().y, light->getAmbientColor().z, light->getAmbientColor().w);
	tempLight.setDirection(light->getDirection().x, light->getDirection().y, light->getDirection().z);
	lightLevel_ = lightedTime_/fallTime_;
	tempLight.setDiffuseColor(lightLevel_, lightLevel_, lightLevel_, 1.0f);

	model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
}

void PieceClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}

	// Release the collision object
	if(collisionTest_)
	{
		collisionTest_->destroy();
		delete collisionTest_;
		collisionTest_ = 0;
	}
}

std::string PieceClass::getName()
{
	return name_;
}

void PieceClass::lightIt()
{
	if(pieceState_ == IN_TREE)
	{
		shaken_ = true;

		if(lightedTime_ > fallTime_)
		{
			makeItFall();
		}
	}
}

void PieceClass::makeItFall()
{
	if(pieceState_ == IN_TREE)
	{
		velocity_.x = 0.0f;
		velocity_.y = -1.6f;
		velocity_.z = 0.0f;

		pieceState_ = FALLING;
		hasFallen_ = true;
	}
}


void PieceClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point PieceClass::getPosition()
{
	return position_;
}

void PieceClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector PieceClass::getScale()
{
	return scaling_;
}

void PieceClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float PieceClass::getRotationX()
{
	return rotX_;
}

void PieceClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float PieceClass::getRotationY()
{
	return rotY_;
}

void PieceClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float PieceClass::getRotationZ()
{
	return rotZ_;
}

SphereCollision* PieceClass::getCollisionSphere()
{
	return collisionTest_;
}

bool PieceClass::hasFallen()
{
	return hasFallen_;
}
