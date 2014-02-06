#ifndef _SPLASH_SCREEN_STATE_H_
#define _SPLASH_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/staticModelClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

#include "Object3DFactory.h"

#include <mmsystem.h>

class VisualizerScreenState: public ApplicationState, public Listener<GUIButton, std::string>
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
		virtual void notify(GUIButton* notifier, std::string arg);

	private:
		static VisualizerScreenState visualizerScreenState_; //singleton

		void createModel(const std::string &modelName);
		bool checkModelHasAnimations(const std::string &modelName);

		void createLoadModelMenu();

		GUIManager*				visualizerGUI_;

		CameraClass*			camera_;

		std::vector<Object3D*>	loadedObjects_;
};

#endif //_SPLASH_SCREEN_STATE_H_
