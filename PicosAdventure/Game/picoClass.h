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

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(FruitClass* notifier, Point arg);
		//virtual void notify(BirdClass* notifier, bool arg);

	private:
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		void checkPicoArrivedObjective();

		void changeAnimation(std::string name, float time);

		CameraClass* camera_;

		Object3D*	body_;
		Object3D*	tips_;
		Object3D*	eyes_;
		Object3D*	hat_;

		SphereCollision* collisionTest_;

		XMFLOAT4	bodyColor_;
		XMFLOAT4	tipsColor_;

		PicoStates  picoState_;

		float		waitedTime_;
		float		eatingWaitTime_;
		float		celebratingWaitTime_;

		std::deque<FruitClass*> fallenFruits_;

		Point		position_;
		Vector		velocity_;
		Point		objective_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		rotZ_;
};

#endif //_PICO_CLASS_H_
