#include "AnimatedObject3D.h"

AnimatedObject3D::AnimatedObject3D()
{
	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;
}

AnimatedObject3D::AnimatedObject3D(const AnimatedObject3D &)
{
}

AnimatedObject3D::~AnimatedObject3D()
{
}

bool AnimatedObject3D::setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName)
{
	objectName_ = objectName;

	model_ = new AnimatedCal3DModelClass;
	if(!model_)
	{
		return false;
	}

	// Initialize the model object.
	if(!model_->setup(graphicsManager->getDevice(), modelName))
	{
		MessageBox(NULL, L"Could not initialize the model object.", L"Visualizer - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	diffuseTexture_ = new TextureClass;
	if(!diffuseTexture_)
	{
		return false;
	}

	// Initialize the texture object.
	std::string filePath = "./Data/models/" + modelName + "/d-" + modelName + ".dds";
	bool result = diffuseTexture_->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load texture!", "Visualizer - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	modelShader_ = graphicsManager->getShader3D();

	return true;

	return true;
}

void AnimatedObject3D::update(float dt)
{
	AnimatedCal3DModelClass* animatedModel = dynamic_cast<AnimatedCal3DModelClass*>(model_);
	if(animatedModel)
	{
		animatedModel->update(dt);
	}
}

void AnimatedObject3D::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light)
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

	model_->draw(device, deviceContext);
	modelShader_->draw(deviceContext, model_->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix, diffuseTexture_->getTexture(), light);
}

void AnimatedObject3D::destroy()
{
	if(diffuseTexture_)
	{
		diffuseTexture_->destroy();
		delete diffuseTexture_;
		diffuseTexture_ = 0;
	}

	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

Object3D * __stdcall AnimatedObject3D::Create(GraphicsManager* graphicsManager, std::string modelName, std::string objectName)
{
	AnimatedObject3D* animatedObject3DTemp = new AnimatedObject3D();

	if(!animatedObject3DTemp->setup(graphicsManager, modelName, objectName))
	{

		return NULL;
	}

	return animatedObject3DTemp;
}

void AnimatedObject3D::playAnimation()
{
	AnimatedCal3DModelClass* animatedModel = dynamic_cast<AnimatedCal3DModelClass*>(model_);
	if(animatedModel)
	{
		animatedModel->playAnimation();
	}
}

void AnimatedObject3D::stopAnimation()
{
	AnimatedCal3DModelClass* animatedModel = dynamic_cast<AnimatedCal3DModelClass*>(model_);
	if(animatedModel)
	{
		animatedModel->stopAnimation();
	}
}