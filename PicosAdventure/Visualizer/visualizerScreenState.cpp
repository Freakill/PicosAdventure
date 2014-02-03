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
	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	return true;
}

void VisualizerScreenState::update(float elapsedTime)
{	
	
}

void VisualizerScreenState::draw()
{
	XMMATRIX viewMatrix;

	camera_->getViewMatrix(viewMatrix);
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

