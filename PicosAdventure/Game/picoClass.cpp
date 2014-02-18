#include "picoClass.h"

PicoClass::PicoClass()
{
	body_ = 0;
	tips_ = 0;
	eyes_ = 0;
	hat_ = 0;

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
	
	lookAt_.x = 0.0f;
	lookAt_.y = 0.0f;
	lookAt_.z = 0.0f;

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

bool PicoClass::setup(GraphicsManager* graphicsManager, CameraClass* camera)
{
	camera_ = camera;

	body_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossCuerpo");
	tips_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossExtremidades");
	eyes_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossOjos");

	// Set specific multitexture shader for tips and increment textures array
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("MultiTextureShader3D", graphicsManager);
	tips_->setShader3D(shaderTemp);

	tips_->getTextureArrayClass()->setNumberTextures(2);

	loadExpressions(graphicsManager);

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 1.6f, 0.0f), 0.8f);

	position_.x = -5.25f;
	position_.y = 0.0f;
	position_.z = 0.25f;

	scaling_.x = 0.041f;
	scaling_.y = 0.041f;
	scaling_.z = 0.041f;

	lookAtCamera();

	positionUnhidding_[0].x = -3.0f;
	positionUnhidding_[0].y = 0.0f;
	positionUnhidding_[0].z = -0.25f;

	positionUnhidding_[1].x = -3.0f;
	positionUnhidding_[1].y = 0.0f;
	positionUnhidding_[1].z = -2.75f;

	unhiddingStep_ = 0;

	tipsColor_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	tipsLight_ = new LightClass;
	tipsLight_->setDiffuseColor(tipsColor_.x, tipsColor_.y, tipsColor_.z, 1.0f);

	faceState_ = NORMAL;

	expressionChangeTime_ = 0.75f;
	expressionPercentage_ = 0.0f;
	actualExpression_ = "normal";
	newExpression_ = "normal";

	MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_->getShader3D());
	multitextureShader->setPercentage(0.1);

	// Setup clock at the end so it starts when we run
	expressionClock_ = new ClockClass();
	if(!expressionClock_)
	{
		return false;
	}
	expressionClock_->reset();

	eatingWaitTime_ = 2.0f;
	celebratingWaitTime_ = 1.2f;

	return true;
}

void PicoClass::update(float elapsedTime)
{
	expressionClock_->tick();

	body_->update(elapsedTime);
	tips_->update(elapsedTime);
	eyes_->update(elapsedTime);
	if(hat_)
	{
		hat_->update(elapsedTime);
	}

	// Update textures for the tips
	tips_->getTextureArrayClass()->getTexturesArray()[0] = expressions_.at(actualExpression_)->getTexture();
	tips_->getTextureArrayClass()->getTexturesArray()[1] = expressions_.at(newExpression_)->getTexture();

	MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_->getShader3D());
	multitextureShader->setPercentage(expressionPercentage_);

	switch(picoState_)
	{
		case HIDDEN:
			{
				if(fallenFruits_.size() > 0)
				{
					changeExpression("sorpresa");
					goToPosition(positionUnhidding_[unhiddingStep_]);
					picoState_ = UNHIDDING;
				}
			}
			break;
		case UNHIDDING:
			{
				walk(elapsedTime);

				if(checkPicoArrivedObjective())
				{
					switch(unhiddingStep_)
					{
						case 0:
							{
								unhiddingStep_++;
								goToPosition(positionUnhidding_[unhiddingStep_]);
								picoState_ = UNHIDDING;
							}
							break;
						case 1:
							{
								Point fallenFruitPos = fallenFruits_.front()->getPosition();
								goToPosition(fallenFruitPos);
								picoState_ = WAITING;
							}
							break;
						default:
							{
								Point fallenFruitPos = fallenFruits_.front()->getPosition();
								goToPosition(fallenFruitPos);
								picoState_ = WAITING;
							}
							break;
					}
				}
			}
			break;
		case WAITING:
			{
				if(fallenFruits_.size() > 0)
				{
					Point fallenFruitPos = fallenFruits_.front()->getPosition();
					changeExpression("sorpresa");
					goToPosition(fallenFruitPos);
				}
			}
			break;
		case WALKING:
			{
				walk(elapsedTime);

				if(checkPicoArrivedObjective())
				{
					eatFruit();
				}
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
					changeExpression("feliz");

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
					switch(fallenFruits_.front()->getFruitEffect())
					{
						case COLOR:
							{
								tipsColor_ = fallenFruits_.front()->getColorEffect();
							}
							break;
						case TEXTURE:
							{
								body_->getTextureArrayClass()->getTexturesArray()[0] = fallenFruits_.front()->getTextureEffect()->getTexture();
							}
							break;
						case HAT:
							{
								hat_ = fallenFruits_.front()->getHatEffect();
							}
							break;
					}

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

	switch(faceState_)
	{
		case NORMAL:
			{
				expressionPercentage_ = 0.0f;
			}
			break;
		case CHANGING:
			{
				expressionPercentage_ = expressionClock_->getTime()/expressionChangeTime_;
				if(expressionClock_->getTime() > expressionChangeTime_)
				{
					faceState_ = CHANGED;
				}
			}
			break;
		case CHANGED:
			{
				expressionPercentage_ = 1.0f;
				actualExpression_ = newExpression_;
				faceState_ = NORMAL;
			}
			break;
	}

	collisionTest_->setPosition(position_);
}

void PicoClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
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

	body_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	eyes_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	if(hat_)
	{
		hat_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	}

	tipsLight_->setAmbientColor(light->getAmbientColor().x, light->getAmbientColor().y, light->getAmbientColor().z, 1.0f);
	tipsLight_->setDiffuseColor(tipsColor_.x*light->getDiffuseColor().x, tipsColor_.y*light->getDiffuseColor().y, tipsColor_.z*light->getDiffuseColor().z, 1.0f);
	tipsLight_->setDirection(light->getDirection().x, light->getDirection().y, light->getDirection().z);

	tips_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tipsLight_);

	if(hat_)
	{
		hat_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	}
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

	// Release the model object
	if(hat_)
	{
		hat_->destroy();
		delete hat_;
		hat_ = 0;
	}

	std::map<std::string, TextureClass*>::iterator it;
	for(it = expressions_.begin(); it != expressions_.end(); it++)
	{
		it->second->destroy();
	}
	expressions_.clear();
}

void PicoClass::goToPosition(Point position)
{
	changeAnimation("walk", 0.4f);

	objective_.x = position.x;
	//objective_.y = position.y;
	objective_.z = position.z;

	picoState_ = WALKING;
}

void PicoClass::setToRest()
{
	changeAnimation("idle", 0.2f);

	lookAtCamera();
	picoState_ = WAITING;

	fallenFruits_.clear();
}

void PicoClass::setTipsColor(XMFLOAT4 color)
{
	tipsColor_ = color;
}

void PicoClass::setBodyTexture(TextureClass* texture)
{
	body_->getTextureArrayClass()->getTexturesArray()[0] = texture->getTexture();
}

void PicoClass::setHat(Object3D* hat)
{
	hat_ = hat;
}

SphereCollision* PicoClass::getCollisionSphere()
{
	return collisionTest_;
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

void PicoClass::lookAtCamera()
{
	Point cameraPos = Point(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	lookAt_.x = cameraPos.x-position_.x;
	lookAt_.y = cameraPos.y-position_.y;
	lookAt_.z = cameraPos.z-position_.z;

	Vector normalizedLookAt = lookAt_.normalize();

	rotX_ = 0.0f;
	rotY_ = acos(normalizedLookAt.x);//3.141592f-1.570796f/4; 
	rotZ_ = 0.0f;
	if(normalizedLookAt.x > 0)
	{
		rotY_ += XM_PIDIV2;
	}
	else
	{
		rotY_ += XM_PIDIV2;
	}
}

bool PicoClass::checkPicoArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

void PicoClass::eatFruit()
{
	if(fallenFruits_.size() > 0)
	{
		changeAnimation("eat", 0.2f);

		Point fruitPos = fallenFruits_.front()->getPosition();
		fruitPos.y = fruitPos.y + 1.6f;
		fruitPos.z = fruitPos.z - 0.85f;
		fallenFruits_.front()->setPosition(fruitPos);

		lookAtCamera();

		waitedTime_ = 0.0f;

		picoState_ = EATING;
		notifyListeners(false);
	}
	else
	{
		changeAnimation("idle", 0.2f);

		lookAtCamera();

		picoState_ = WAITING;
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

		lookAtCamera();

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
	if(hat_)
	{
		animatedTemp = dynamic_cast<AnimatedObject3D*>(hat_);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToPlay(name, time);
	}
}

void PicoClass::changeExpression(std::string newExpression)
{
	newExpression_ = newExpression;
	faceState_ = CHANGING;
	expressionClock_->reset();
}

void PicoClass::loadExpressions(GraphicsManager* graphicsManager)
{
	TextureClass* temp1 = new TextureClass;
	std::string filePath = "./Data/models/miniBossExtremidades/d-e-normal.dds";
	bool result = temp1->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the normal expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("normal", temp1));

	TextureClass* temp2 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-feliz.dds";
	result = temp2->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the feliz expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("feliz", temp2));

	TextureClass* temp3 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa.dds";
	result = temp3->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the sorpresa expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa", temp3));

	TextureClass* temp4 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-triste.dds";
	result = temp4->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the triste expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("triste", temp4));

	TextureClass* temp5 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa2.dds";
	result = temp5->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the triste expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa2", temp5));

	TextureClass* temp6 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-superfeliz.dds";
	result = temp6->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the triste expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("superfeliz", temp6));
}