#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "visualizerScreenState.h"

VisualizerScreenState VisualizerScreenState::visualizerScreenState_;

VisualizerScreenState::VisualizerScreenState()
{
	camera_ = 0;

	tempText_[0] = 0;
	tempText_[1] = 0;
}

VisualizerScreenState::~VisualizerScreenState()
{

}

VisualizerScreenState* VisualizerScreenState::Instance()
{
	return (&visualizerScreenState_);
}

bool VisualizerScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	model_ = new StaticModelClass;
	if(!model_)
	{
		return false;
	}

	// Initialize the model object.
	if(!model_->setup(graphicsManager->getDevice(), "carne"))
	{
		MessageBox(NULL, L"Could not initialize the model object.", L"Visualizer - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	texture_ = new TextureClass;
	if(!texture_)
	{
		return false;
	}

	// Initialize the texture object.
	std::string strFilename = "carne";
	std::string filePath = "./Data/models/" + strFilename + "/d-" + strFilename + ".dds";
	bool result = texture_->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load texture!", "Visualizer - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Instantiate text
	XMFLOAT4X4 baseViewMatrix;
	camera_->getViewMatrix(baseViewMatrix);

	int screenWidth, screenHeight;
	graphicsManager_->getScreenSize(screenWidth, screenHeight);

	return true;
}

void VisualizerScreenState::update(float elapsedTime)
{	
	
}

void VisualizerScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	model_->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext());
	graphicsManager_->draw3D(model_->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix, texture_->getTexture());
}

void VisualizerScreenState::destroy()
{
	
}

void VisualizerScreenState::notify(InputManager* notifier, int arg)
{
	switch(arg){
		default:
			{
				/*std::stringstream keyStream;
				keyStream << "Key pressed " << arg;
				MessageBoxA(NULL, keyStream.str().c_str(), "First Screen", MB_OK);*/
			}
			break;
	}
}

