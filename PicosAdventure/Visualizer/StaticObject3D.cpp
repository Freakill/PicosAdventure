#include "StaticObject3D.h"

StaticObject3D::StaticObject3D()
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

StaticObject3D::StaticObject3D(const StaticObject3D &)
{
}

StaticObject3D::~StaticObject3D()
{
}

bool StaticObject3D::setup(GraphicsManager* graphicsManager, std::string modelName)
{
	model_ = new StaticModelClass;
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
}

void StaticObject3D::update(float dt)
{
	// DO NOTHING
}

void StaticObject3D::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
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
	modelShader_->draw(deviceContext, model_->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix, diffuseTexture_->getTexture());
}

void StaticObject3D::destroy()
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

Object3D* __stdcall StaticObject3D::Create(GraphicsManager* graphicsManager, std::string modelName)
{
	StaticObject3D* staticObject3DTemp = new StaticObject3D();

	if(!staticObject3DTemp->setup(graphicsManager, modelName))
	{
		return NULL;
	}

	return staticObject3DTemp;
}