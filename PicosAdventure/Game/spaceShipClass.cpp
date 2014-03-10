#include "spaceShipClass.h"

SpaceShipClass::SpaceShipClass()
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

	objective_.x = 0.0f;
	objective_.y = 0.0f;
	objective_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;
}

SpaceShipClass::SpaceShipClass(const SpaceShipClass& other)
{
}

SpaceShipClass::~SpaceShipClass()
{
}

bool SpaceShipClass::setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager)
{
	soundManager_ = soundManager;

	model_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "nave");

	AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(model_);
	AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay("nave", 0.4f);

	spaceShipParticles_ = new ParticleSystem;
	if(!spaceShipParticles_)
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(spaceShipParticles_ && !spaceShipParticles_->setup(graphicsManager, "star", getPosition(), 2.8, XMFLOAT4(1.00f, 1.00f, 0.0f, 1.0f)))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	spaceShipState_ = WAITING;

	return true;
}

void SpaceShipClass::update(float elapsedTime)
{
	model_->update(elapsedTime);

	switch(spaceShipState_)
	{
		case WAITING:
			{

			}
			break;
		case LAUNCHING:
			{
				position_.y += velocity_.y*elapsedTime;

				velocity_.y -= 2.0f*elapsedTime;

				if(position_.y < floorHeight_)
				{
					soundManager_->playFile("piece_fall", false);

					position_.y = floorHeight_;

					spaceShipState_ = WAITING;
				}
			}
			break;
		case FLYING:
			{
				fly(elapsedTime);
			}
			break;
		default:
			{

			}
			break;
	}
}

void SpaceShipClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
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

	model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
}

void SpaceShipClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void SpaceShipClass::goToPosition(Point position)
{
	objective_.x = position.x;
	objective_.y = position.y;
	objective_.z = position.z;

	spaceShipState_ = FLYING;
}

void SpaceShipClass::makeLaunch(int level)
{
	if(spaceShipState_ == WAITING)
	{
		std::stringstream sound;
		sound << "fuel" << level;
		//soundManager_->playFile(sound.str(), false);

		velocity_.x = 0.0f;
		velocity_.y = level*3.0f;
		velocity_.z = 0.0f;

		spaceShipState_ = LAUNCHING;
	}
}

Object3D* SpaceShipClass::getObject()
{
	return model_;
}

void SpaceShipClass::setInitialPosition(Point pos)
{
	initialPosition_.x = pos.x;
	initialPosition_.y = pos.y;
	initialPosition_.z = pos.z;
}

void SpaceShipClass::setPosition(Point pos)
{
	position_.x = pos.x;
	position_.y = pos.y;
	position_.z = pos.z;
}

Point SpaceShipClass::getPosition()
{
	return position_;
}

void SpaceShipClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

void SpaceShipClass::setRotation(float x, float y, float z)
{
	rotX_ = x;
	rotY_ = y;
	rotZ_ = z;
}

void SpaceShipClass::setFloorHeight(float floor)
{
	floorHeight_ = floor;
}

void SpaceShipClass::fly(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity*7;

	//rotY_ = acos(normalizedVelocity.z);
	/*if(normalizedVelocity.x < 0)
	{
		rotY_ += 3.141592f;
	}*/

	position_.x += velocity_.x*elapsedTime;
	position_.y += velocity_.y*elapsedTime;
	position_.z += velocity_.z*elapsedTime;
}

bool SpaceShipClass::checkArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

