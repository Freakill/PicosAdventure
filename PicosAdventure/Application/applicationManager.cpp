#include "applicationManager.h"

#include "applicationState.h"

#include "../Game/firstScreenState.h"
#include "../Game/secondScreenState.h"

ApplicationManager::ApplicationManager()
{
	appState_ = 0;
	graphicsManager_ = 0;
	clockClass_ = 0;
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
	clockClass_->reset();

	if(!changeState(SecondScreenState::Instance()))
	{
		return false;
	}

	return true;
}

void ApplicationManager::update()
{
	clockClass_->tick();

	inputManager_->update();

	// we update the State with the frame elapsed time
	if(appState_ != 0)
	{
		appState_->update(clockClass_->getDeltaTime());
	}
}

void ApplicationManager::draw()
{
	graphicsManager_->beginDraw(1.0f, 0.5f, 0.0f, 1.0f);

	// We call the draw function of the active state
	if(appState_ != 0)
	{
		appState_->draw();
	}

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
		return appState_->setup(this, graphicsManager_, inputManager_);
	}

	return true;
}
