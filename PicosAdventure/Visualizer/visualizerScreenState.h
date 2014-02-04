#ifndef _SPLASH_SCREEN_STATE_H_
#define _SPLASH_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/staticModelClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/textClass.h"

#include <mmsystem.h>

class VisualizerScreenState: public ApplicationState
{
	public:
		VisualizerScreenState();
		virtual ~VisualizerScreenState();
		static VisualizerScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, int arg);

	private:
		static VisualizerScreenState visualizerScreenState_; //singleton

		CameraClass*		camera_;

		StaticModelClass*	model_;
		TextureClass*		texture_;

		TextClass*			tempText_[2];
};

#endif //_SPLASH_SCREEN_STATE_H_
