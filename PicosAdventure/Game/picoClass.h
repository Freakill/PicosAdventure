#ifndef _PICO_CLASS_H_
#define _PICO_CLASS_H_

#include "../Engine/Object3DFactory.h"

#include "../Application/inputManager.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "fruitClass.h"
//#include "birdClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"

#include "../Utils/clockClass.h"

#include <deque>

enum PicoStates
	{
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

class PicoClass : public Listener<InputManager, InputStruct>, public Listener<FruitClass, Point>, public Notifier<PicoClass, bool>
{
	public:
		PicoClass();
		PicoClass(const PicoClass&);
		~PicoClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera);
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

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(FruitClass* notifier, Point arg);
		//virtual void notify(BirdClass* notifier, bool arg);

		SphereCollision* getCollisionSphere();

	private:
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		void lookAtCamera();

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
		float		rotZ_;

		// Unhidding
		Point		positionUnhidding_[2];
		int			unhiddingStep_;
};

#endif //_PICO_CLASS_H_
