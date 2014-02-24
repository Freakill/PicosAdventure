#ifndef _PICO_FIRST_CLASS_H_
#define _PICO_FIRST_CLASS_H_

#include "../Application/inputManager.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/soundClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"
#include "../Utils/clockClass.h"

#include "fruitClass.h"
#include "birdClass.h"

#include <deque>

#define UNHIDDING_STEPS 2

class PicoFirstClass : public Listener<InputManager, InputStruct>, public Listener<FruitClass, Point>, public Listener<BirdClass, bool>, public Notifier<PicoFirstClass, bool>
{
	private:
		enum PicoStates
			{
				HIDDING,
				HIDDEN,
				UNHIDDING,
				WAITING,
				WALKING,
				TURNING,
				EATING,
				CELEBRATING,
				SCARED
			};

		enum FaceStates
			{
				NORMAL,
				CHANGING,
				CHANGED
			};

	public:
		PicoFirstClass();
		PicoFirstClass(const PicoFirstClass&);
		~PicoFirstClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundClass* soundManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void goToPosition(Point position);
		void setToRest();

		void changeAnimation(std::string name, float time);
		void changeExpression(std::string newExpression);

		void setTipsColor(XMFLOAT4 color);
		void setBodyTexture(TextureClass* texture);
		void setHat(Object3D* hat);

		SphereCollision* getCollisionSphere();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(FruitClass* notifier, Point arg);
		virtual void notify(BirdClass* notifier, bool arg);

	private:
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		bool lookAtCamera(bool check);

		bool checkPicoArrivedObjective();
		void eatFruit();

		CameraClass* camera_;

		// 3D Models
		Object3D*	body_;
		Object3D*	tips_;
		Object3D*	eyes_;
		Object3D*	hat_;

		SphereCollision* collisionTest_;

		// Expressions
		std::map<std::string, TextureClass*> expressions_;

		XMFLOAT4	tipsColor_;
		LightClass* tipsLight_;

		PicoStates  picoState_;

		FaceStates	faceState_;
		float		expressionChangeTime_;
		float		expressionPercentage_;
		std::string actualExpression_;
		std::string newExpression_;
		ClockClass* expressionClock_;

		float		waitedTime_;
		float		eatingWaitTime_;
		float		celebratingWaitTime_;

		// Sound
		SoundClass*	soundManager_;

		// Fruits
		std::deque<FruitClass*> fallenFruits_;

		// Position and movement
		Point		position_;
		Vector		velocity_;
		Point		objective_;
		Vector		lookAt_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		newRotY_;
		float		rotZ_;

		// Beginning
		Point		hiddingPosition_;
		bool		hasToHide_;

		// Unhidding
		Point		positionUnhidding_[UNHIDDING_STEPS];
		int			unhiddingStep_;

		TextClass*	info_;
};

#endif //_PICO_FIRST_CLASS_H_
