#ifndef _APPLICATION_STATE_H_
#define _APPLICATION_STATE_H_

#include "applicationManager.h"

#include "inputManager.h"
#include "../Graphics/graphicsManager.h"

#include <string>

class ApplicationState : public Listener<InputManager, int>
{
	public:
		ApplicationState();
		virtual ~ApplicationState();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, HWND windowHandler) = 0;
		virtual void update(float elapsedTime) = 0;
		virtual void draw() = 0;
		virtual void destroy() = 0;

		bool changeState(ApplicationState* appState);

	protected:
		ApplicationManager* appManager_;
		GraphicsManager* graphicsManager_;
};

#endif
