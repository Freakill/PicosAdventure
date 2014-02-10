#ifndef _FIRST_SCREEN_STATE_H_
#define _FIRST_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

#include "../Engine/Object3DFactory.h"

#include "fruitClass.h"

#include <mmsystem.h>

class FirstScreenState: public ApplicationState
{
	public:
		FirstScreenState();
		virtual ~FirstScreenState();
		static FirstScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);

	private:
		static FirstScreenState firstScreenState_; //singleton

		void loadScenario(std::string scenario);
		void createScenarioObject(std::string xmlName);

		CameraClass*			camera_;
		LightClass*				light_;

		std::vector<Object3D*>	scenario_;
		FruitClass*				fruitTest_;

		bool					debug_;
};

#endif //_FIRST_SCREEN_STATE_H_
