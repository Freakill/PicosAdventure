#include "picoClass.h"

PicoClass::PicoClass()
{
	body_ = 0;
	tips_ = 0;
	eyes_ = 0;

	picoState_ = HIDDEN;
	waitedTime_ = 0.0f;
	eatingWaitTime_ = 0.0f;
	celebratingWaitTime_ = 0.0f;

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

PicoClass::PicoClass(const PicoClass& other)
{
}

PicoClass::~PicoClass()
{
}

bool PicoClass::setup(GraphicsManager *graphicsManager, CameraClass* camera)
{
	camera_ = camera;

	body_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossCuerpo");
	tips_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossExtremidades");
	eyes_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossOjos");

	position_.x = -5.25f;
	position_.y = 0.0f;
	position_.z = 0.25f;

	scaling_.x = 0.041f;
	scaling_.y = 0.041f;
	scaling_.z = 0.041f;
	
	rotX_ = 0.0f;
	rotY_ = 3.141592f-1.570796f/4; 
	rotZ_ = 0.0f;

	bodyColor_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tipsColor_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	eatingWaitTime_ = 2.0f;
	celebratingWaitTime_ = 1.2f;

	return true;
}

void PicoClass::update(float elapsedTime)
{
	body_->update(elapsedTime);
	tips_->update(elapsedTime);
	eyes_->update(elapsedTime);

	switch(picoState_)
	{
		case HIDDEN:
			{
				if(fallenFruits_.size() > 0)
				{
					Point fallenFruitPos = fallenFruits_.front()->getPosition();

					goToPosition(fallenFruitPos);
				}
			}
			break;
		case WAITING:
			{
				if(fallenFruits_.size() > 0)
				{
					Point fallenFruitPos = fallenFruits_.front()->getPosition();

					goToPosition(fallenFruitPos);
				}
			}
			break;
		case WALKING:
			{
				walk(elapsedTime);

				checkPicoArrivedObjective();
			}
			break;
		case TURNING:
			{
				
			}
			break;
		case EATING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > eatingWaitTime_)
				{
					changeAnimation("celebration", 0.4f);

					waitedTime_ = 0.0f;

					picoState_ = CELEBRATING;
				}
			}
			break;
		case CELEBRATING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > celebratingWaitTime_)
				{
					/*if(!changingClothes_)
					{
						tipsColor_ = fallenFruits_.front()->getColor();
					}
					else
					{
						//bodyColor_ = fallenFruits_.front()->getColor();
						bodyModel_->setTexture(fallenFruits_.front()->getTexture());
					}*/
					fallenFruits_.front()->resetFruit();
					fallenFruits_.pop_front();

					changeAnimation("idle", 0.2f);

					waitedTime_ = 0.0f;

					picoState_ = WAITING;
				}
			}
			break;
		case SCARED:
			{
				
			}
			break;
	}
}

void PicoClass::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light)
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

	body_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
	tips_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
	eyes_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
}

void PicoClass::destroy()
{
	// Release the model object
	if(body_)
	{
		body_->destroy();
		delete body_;
		body_ = 0;
	}

	// Release the model object
	if(tips_)
	{
		tips_->destroy();
		delete tips_;
		tips_ = 0;
	}

	// Release the model object
	if(eyes_)
	{
		eyes_->destroy();
		delete eyes_;
		eyes_ = 0;
	}
}

void PicoClass::goToPosition(Point position)
{
	changeAnimation("walk", 0.4f);

	objective_.x = position.x;
	//objective_.y = position.y;
	objective_.z = position.z;

	picoState_ = WALKING;
}

float PicoClass::approach(float goal, float current, float dt)
{
	float difference = goal - current;

	if(difference > dt)
		return current + dt;
	if(difference < dt)
		return current - dt;

	return goal;
}

void PicoClass::walk(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity * 1.8f;

	rotY_ = acos(normalizedVelocity.z);
	if(normalizedVelocity.x < 0)
	{
		rotY_ += 3.141592f;
	}

	position_.x += velocity_.x*elapsedTime;
	position_.y += velocity_.y*elapsedTime;
	position_.z += velocity_.z*elapsedTime;
}

void PicoClass::checkPicoArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		if(fallenFruits_.size() > 0)
		{
			changeAnimation("eat", 0.2f);

			Point fruitPos = fallenFruits_.front()->getPosition();
			fruitPos.y = fruitPos.y + 1.6f;
			fruitPos.z = fruitPos.z - 0.85f;
			fallenFruits_.front()->setPosition(fruitPos);

			rotY_ = 3.141592f;

			waitedTime_ = 0.0f;

			picoState_ = EATING;
			notifyListeners(false);
		}
		else
		{
			changeAnimation("idle", 0.2f);

			rotY_ = 3.141592f;

			picoState_ = WAITING;
		}
	}
}

void PicoClass::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		default:
			{
				
			}
			break;
	}
}

void PicoClass::notify(FruitClass* notifier, Point arg)
{
	fallenFruits_.push_back(notifier);
}

/*void PicoClass::notify(BirdClass* notifier, bool arg)
{
	if(picoState_ == WALKING && !arg)
	{
		bodyModel_->setAnimationToPlay("negation", 0.2f);
		tipsModel_->setAnimationToPlay("negation", 0.2f);

		rotY_ = 3.141592f;

		picoState_ = SCARED;
	}

	if(picoState_ == SCARED && arg)
	{
		Point fallenFruitPos = fallenFruits_.front()->getPosition();

		goToPosition(fallenFruitPos);
	}
}*/

void PicoClass::changeAnimation(std::string name, float time)
{
	AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(body_);
	AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(tips_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(eyes_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
}