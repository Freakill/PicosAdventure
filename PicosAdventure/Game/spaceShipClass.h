#ifndef _SPACE_SHIP_CLASS_H_
#define _SPACE_SHIP_CLASS_H_

#include "../Engine/Object3DFactory.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

class SpaceShipClass
{
	public:
		SpaceShipClass();
		SpaceShipClass(const SpaceShipClass&);
		~SpaceShipClass();

		bool setup(GraphicsManager* graphicsManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void goToPosition(Point position);

	private:
		void fly(float elapsedTime);
		bool checkArrivedObjective();

		Object3D*	model_;

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
