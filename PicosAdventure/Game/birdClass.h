#ifndef _BIRD_CLASS_H_
#define _BIRD_CLASS_H_

#include "../Application/inputManager.h"

#include "fruitClass.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/sphereCollision.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"

#include <deque>

class PicoFirstClass;

class BirdClass : public Listener<FruitClass, Point>, public Listener<PicoFirstClass, bool>, public Notifier<BirdClass, bool>
{
	private:
		enum BirdState
			{
				HIDDEN,
				WAITING,
				FLYING,
				TEASING,
				RUNNING_AWAY
			};

	public:
		BirdClass();
		BirdClass(const BirdClass&);
		~BirdClass();

		bool setup(GraphicsManager* graphicsManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setStealFood(bool steal);
		bool getStealFood();

		void goToPosition(Point position);

		void scared();

		SphereCollision* getCollisionSphere();

		virtual void notify(FruitClass* notifier, Point arg);
		virtual void notify(PicoFirstClass* notifier, bool arg);

	private:
		float approach(float goal, float current, float dt);
		void fly(float elapsedTime);
		void checkArrivedObjective();

		Object3D*	model_;
		SphereCollision* collisionTest_;

		BirdState	birdState_;
		bool		stealFood_;

		FruitClass* fallenFruit_;

		Point		initialPosition_;
		Point		position_;
		Vector		velocity_;
		Point		objective_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		rotZ_;
};

#endif //_BIRD_CLASS_H_
