#ifndef _THIRD_SCREEN_STATE_H_
#define _THIRD_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Game/friendSpaceShipClass.h"
#include "../Game/logClass.h"
//#include "../Game/soundSecondClass.h"
#include "../Game/spaceShipClass.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/imageClass.h"
#include "../Graphics/lightClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Utils/clockClass.h"

//#include "picoSecondClass.h"

#include <mmsystem.h>

class ThirdScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>
{
	enum LevelState
		{
			INTRODUCTION,
			PLAYING,
			ENDING
		};

	enum IntroLevel
		{
			ARRIBING,
			LANDING,
			TRANSITION
		};

	public:
		ThirdScreenState();
		virtual ~ThirdScreenState();
		static ThirdScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);

	private:
		static ThirdScreenState thirdScreenState_; //singleton

		bool setupDebugOptions();
		bool setupKinectHands();
		bool setupSplashScreen();

		void updateIntroduction(float elapsedTime);
		void updateKinectHands();

		void drawSplashScreen();

		void loadConfigurationFromXML();

		bool loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		void changeLevel(LevelState level);

		ImageClass*					splashScreen_;

		CameraClass*				camera_;
		LightClass*					light_;
		LightClass*					piecesLight_;
		ClockClass*					gameClock_;
		SoundSecondClass*			soundManager_;
		KinectClass*				kinectClass_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		IntroLevel					introLevelState_;

		// Introduction control variables
		ClockClass*					introClock_;

		// Kinect
		Point						kinectHandViewPos_[2];
		Point						kinectHandWorldPos_[2];
		ImageClass*					kinectHand_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		//Object3D*					spaceShipObject_;
		SpaceShipClass*				spaceShip_;
		FriendSpaceShipClass*		friendSpacheShips_[5];
	
		// Debug
		bool						debug_;
		TextClass*					FPS_;
		TextClass*					kinectHands_[2];
};

#endif //_THIRD_SCREEN_STATE_H_
