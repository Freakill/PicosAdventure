#ifndef _SPLASH_SCREEN_STATE_H_
#define _SPLASH_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/staticModelClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

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

		virtual void notify(InputManager* notifier, InputStruct arg);

	private:
		static VisualizerScreenState visualizerScreenState_; //singleton

		GUIManager*				visualizerGUI;

		CameraClass*			camera_;

		StaticModelClass*		model_;
		TextureClass*			texture_;

		std::vector<TextClass*> tempText_;
};

#endif //_SPLASH_SCREEN_STATE_H_
