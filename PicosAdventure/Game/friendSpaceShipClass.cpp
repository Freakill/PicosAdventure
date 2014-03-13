#include "friendSpaceShipClass.h"

FriendSpaceShipClass::FriendSpaceShipClass()
{
	model_ = 0;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	viewportPosition_.x = 0;
	viewportPosition_.y = 0;
	viewportPosition_.z = 0;
}

FriendSpaceShipClass::FriendSpaceShipClass(const FriendSpaceShipClass& other)
{
}

FriendSpaceShipClass::~FriendSpaceShipClass()
{
}

bool FriendSpaceShipClass::setup(GraphicsManager *graphicsManager, SoundSecondClass* soundManager, std::string fileName)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

	soundManager_ = soundManager;

	// load dirt particles that will cover the spaceship
	dirt_ = new ParticleSystem;
	if(!dirt_)
	{
		MessageBoxA(NULL, "Could not create dirt instance", "FreindSpaceShip - Error", MB_ICONERROR | MB_OK);
	}

	if(dirt_ && !dirt_->setup(graphicsManager, "star", position_, 0.8, 30, 90, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)))
	{
		MessageBoxA(NULL, "Could not setup dirt object", "FreindSpaceShip - Error", MB_ICONERROR | MB_OK);
	}
	dirt_->setParticlesDeviation(Point(0.0f, 0.1f, 0.5f));
	dirt_->setParticlesVelocity(Point(0.0f, 0.0f, 0.0f), Point(2.0f, 2.0f, 0.1f));
	dirt_->setParticleSize(0.24f);

	// Load debug image for viewport transformation
	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	viewportImage_ = new ImageClass;
	if(!viewportImage_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	if(!viewportImage_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth, screenHeight, "star", 40, 40))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	friendSpaceShipState_ = WAITING;

	return true;
}

void FriendSpaceShipClass::update(float elapsedTime)
{
	updateViewportPosition();

	switch(friendSpaceShipState_)
	{
		default:
			{

			}
			break;
	}
}

void FriendSpaceShipClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	graphicsManager->turnOnParticlesAlphaBlending();
	graphicsManager->turnZBufferOff();
		dirt_->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	graphicsManager->turnZBufferOn();
	graphicsManager->turnOffAlphaBlending();

	if(debug)
	{
		XMFLOAT4X4 orthoMatrix;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		int screenWidth, screenHeight;
		graphicsManager->getScreenSize(screenWidth, screenHeight);

		graphicsManager->turnOnWireframeRasterizer();
			viewportImage_->draw(graphicsManager->getDeviceContext(), ((screenWidth/2)*-1)+viewportPosition_.x, ((screenHeight/2))-viewportPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager->turnOnSolidRasterizer();
	}

	worldMatrix_ = worldMatrix;
	viewMatrix_ = viewMatrix;
	projectionMatrix_ = projectionMatrix;

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

void FriendSpaceShipClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}

	// Release the collision object
	if(viewportImage_)
	{
		viewportImage_->destroy();
		delete viewportImage_;
		viewportImage_ = 0;
	}
}

void FriendSpaceShipClass::updateViewportPosition()
{
	XMFLOAT4 position = XMFLOAT4(position_.x, position_.y, position_.z, 1.0f);

	XMFLOAT4 resultPos;
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&position), XMLoadFloat4x4(&worldMatrix_)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&viewMatrix_)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&projectionMatrix_)));

	viewportPosition_.x = resultPos.x;
	viewportPosition_.y = resultPos.y;
}

void FriendSpaceShipClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point FriendSpaceShipClass::getPosition()
{
	return position_;
}


void FriendSpaceShipClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector FriendSpaceShipClass::getScale()
{
	return scaling_;
}

void FriendSpaceShipClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float FriendSpaceShipClass::getRotationX()
{
	return rotX_;
}

void FriendSpaceShipClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float FriendSpaceShipClass::getRotationY()
{
	return rotY_;
}

void FriendSpaceShipClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float FriendSpaceShipClass::getRotationZ()
{
	return rotZ_;
}

Point FriendSpaceShipClass::getViewportPosition()
{
	return viewportPosition_;
}
