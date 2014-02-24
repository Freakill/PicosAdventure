#ifndef _SECOND_SCREEN_STATE_H_
#define _SECOND_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

#include "../Engine/Object3DFactory.h"

#include "../Utils/clockClass.h"

#include "picoFirstClass.h"
#include "pieceClass.h"

#include <mmsystem.h>

class SecondScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>
{
	enum LevelState
		{
			INTRODUCTION,
			MOUNTING = 1,
			COLLECTING,
			ENDING
		};

	enum IntroLevel
		{
			TO_PIECE,
			LIGHT_IT,
			DRAG_IT,
			WORK,
			GIVE_POWER
		};

	public:
		SecondScreenState();
		virtual ~SecondScreenState();
		static SecondScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);

	private:
		static SecondScreenState secondScreenState_; //singleton

		void loadConfigurationFromXML();

		void loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		bool loadPieces();

		void changeLevel(LevelState level);

		CameraClass*				camera_;
		LightClass*					light_;
		ClockClass*					gameClock_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		IntroLevel					introLevelState_;

		float						playingTime_;
		float						fadeTime_;

		// Kinect
		Point						kinectHandPos_;
		ImageClass*					kinectHand_;
		TextClass*					kinectHandText_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		Object3D*					spaceShipWireframe_;
		std::vector<PieceClass*>	pieces_;

		// Pointlights
		XMFLOAT4					lightPos_[2];

		// Debug
		bool						debug_;
		TextClass*					FPS_;
};

#endif //_SECOND_SCREEN_STATE_H_
