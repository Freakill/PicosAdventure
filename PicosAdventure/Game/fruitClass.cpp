#include "fruitClass.h"

FruitClass::FruitClass()
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
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;
}

FruitClass::FruitClass(const FruitClass& other)
{
}

FruitClass::~FruitClass()
{
}

bool FruitClass::setup(GraphicsManager *graphicsManager, std::string fileName, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

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
	
	rotX_ = rotX;
	rotY_ = rotY; 
	rotZ_ = rotZ;

	fruitState_ = SPAWNING;

	floorHeight_ = floorHeight;

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 0.4f);

	return true;
}

void FruitClass::update(float elapsedTime)
{
	switch(fruitState_)
	{
		case SPAWNING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > spawningTime_)
				{
					scaling_.x = endScaling_.x;
					scaling_.y = endScaling_.y;
					scaling_.z = endScaling_.z;

					fruitState_ = IN_TREE;
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
					fruitState_ = IN_FLOOR;
					notifyListeners(position_);
				}
			}
			break;
		case IN_FLOOR:
			{
				//position_.y = -1.4f;
			}
			break;
	}

	collisionTest_->setPosition(position_);
}

void FruitClass::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		collisionTest_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
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

	model_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
}

void FruitClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void FruitClass::makeItFall()
{
	if(fruitState_ == IN_TREE)
	{
		velocity_.x = 0.0f;
		velocity_.y = -1.6f;
		velocity_.z = 0.0f;

		fruitState_ = FALLING;
	}
}

void FruitClass::resetFruit()
{
	position_.x = initialPosition_.x;
	position_.y = initialPosition_.y;
	position_.z = initialPosition_.z;

	scaling_.x = initialScaling_.x;
	scaling_.y = initialScaling_.y;
	scaling_.z = initialScaling_.z;

	waitedTime_ = 0.0f;

	fruitState_ = SPAWNING;
}

void FruitClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point FruitClass::getPosition()
{
	return position_;
}

void FruitClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector FruitClass::getScale()
{
	return scaling_;
}

void FruitClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float FruitClass::getRotationX()
{
	return rotX_;
}

void FruitClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float FruitClass::getRotationY()
{
	return rotY_;
}

void FruitClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float FruitClass::getRotationZ()
{
	return rotZ_;
}

SphereCollision* FruitClass::getCollisionSphere()
{
	return collisionTest_;
}

void FruitClass::setFruitEffectType(FruitEffect effect)
{
	fruitEffect_ = effect;
}

void FruitClass::setColorEffect(XMFLOAT4 color)
{
	colorEffect_ = color;
}