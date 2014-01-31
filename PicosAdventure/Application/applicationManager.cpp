#include "applicationManager.h"

#include "applicationState.h"

ApplicationManager::ApplicationManager()
{
	appState_ = 0;
	graphicsManager_ = 0;
	clockClass_ = 0;
	//kinectClass_ = 0;
}

ApplicationManager::~ApplicationManager()
{

}

bool ApplicationManager::setup(HWND windowsHandler, InputManager* inputManager, int width, int height, bool fullscreen)
{
	windowHandler_ = windowsHandler;

	inputManager_ = inputManager;

	graphicsManager_ = new GraphicsManager();
	if(!graphicsManager_)
	{
		return false;
	}

	if(!graphicsManager_->setup(windowsHandler, false, fullscreen, 1000.0f, 0.1f))
	{
		MessageBox(NULL, L"Could not initialize DirectX11.", L"ApplicationManager - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	clockClass_ = new ClockClass();
	if(!clockClass_)
	{
		return false;
	}
	clockClass_->setup();

	/*kinectClass_ = new KinectClass;
	if(!kinectClass_)
	{
		return false;
	}
	
	if(!kinectClass_->setup(windowHandler_, graphicsManager_->getSwapChain()))
	{
		return false;
	}*/

	/*if(!changeState(FirstScreenState::Instance()))
	{
		return false;
	}*/

	return true;
}

void ApplicationManager::update()
{
	// we update the State with the frame elapsed time
	if(appState_ != 0)
	{
		appState_->update(clockClass_->calculateDeltaSeconds());
	}

	/*if(kinectClass_)
	{
		kinectClass_->update();
	}*/
}

void ApplicationManager::draw()
{
	graphicsManager_->beginDraw(1.0f, 0.5f, 0.0f, 1.0f);

	// We call the draw function of the active state
	if(appState_ != 0)
	{
		appState_->draw();
	}

	/*if(kinectClass_)
	{
		graphicsManager_->turnZBufferOff();

		// Turn on the alpha blending before rendering the text.
		graphicsManager_->turnOnAlphaBlending();

		kinectClass_->draw();

		// Turn off alpha blending after rendering the text.
		graphicsManager_->turnOffAlphaBlending();

		graphicsManager_->turnZBufferOn();
	}*/

	graphicsManager_->endDraw();
}

void ApplicationManager::destroy()
{
	if(clockClass_)
	{
		delete clockClass_;
		clockClass_ = 0;
	}

	if(appState_)
	{
		appState_->destroy();
	}

	/*if(kinectClass_)
	{
		kinectClass_->destroy();
		delete kinectClass_;
		kinectClass_ = 0;
	}*/

	if(graphicsManager_)
	{
		graphicsManager_->destroy();
		delete graphicsManager_;
		graphicsManager_ = 0;
	}
}

bool ApplicationManager::changeState(ApplicationState* appState)
{
	if (appState != appState_) {
		inputManager_->removeListener((*appState_));
		appState_ = appState;
		return appState_->setup(this, graphicsManager_, inputManager_, windowHandler_);
	}

	return true;
}
