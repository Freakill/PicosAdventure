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

#include "../Utils/clockClass.h"

#include "picoClass.h"
#include "fruitClass.h"

#include <mmsystem.h>

enum LevelState
	{
		INTRODUCTION,
		FIRST_LEVEL = 1,
		SECOND_LEVEL,
		THIRD_LEVEL,
		FOURTH_LEVEL
	};

enum SubLevelState
	{
		PLAYING,
		FADING,
		SELECT_POLAROID
	};

class FirstScreenState: public ApplicationState, public Listener<GUIButton, ButtonStruct>
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
		void notify(GUIButton* notifier, ButtonStruct arg);

	private:
		static FirstScreenState firstScreenState_; //singleton

		void updateFirsLevel();

		void setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager);

		void loadConfigurationFromXML();

		void loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		bool createFruits(std::string scenario, LevelState level);
		void clearFruits();

		bool createPolaroids(std::string scenario, LevelState level);
		void clearPolaroids();

		void changeLevel(LevelState level);

		CameraClass*				camera_;
		LightClass*					light_;
		ClockClass*					gameClock_;

		LevelState					levelState_;
		SubLevelState				subLevelState_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		PicoClass*					pico_;

		std::vector<FruitClass*>	fruits_;
		GUIManager*					polaroidGUI_;
		GUIFrame*					polaroidFrame_;

		float						playingTime_;
		float						fadeTime_;

		bool						debug_;
};

#endif //_FIRST_SCREEN_STATE_H_
