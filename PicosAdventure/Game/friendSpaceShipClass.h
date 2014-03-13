#ifndef _FRIEND_SPACESHIP_CLASS_H_
#define _FRIEND_SPACESHIP_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/logClass.h"
#include "../Game/soundSecondClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"

#include <math.h>

class FriendSpaceShipClass : public Notifier<FriendSpaceShipClass, Point>
{
	private:
		enum FriendSpaceShipState
			{
				WAITING,
				ENTERING,
				FLYING,
				GOING_AWAY
			};

	public:
		FriendSpaceShipClass();
		FriendSpaceShipClass(const FriendSpaceShipClass&);
		~FriendSpaceShipClass();

		bool setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager, std::string fileName);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setPosition(Point newPosition);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		Point getViewportPosition();

	private:
		void updateViewportPosition();

		SoundSecondClass*	soundManager_;

		ParticleSystem*		dirt_;

		Object3D*			model_;

		Point				viewportPosition_;
		ImageClass*			viewportImage_;
		XMFLOAT4X4			worldMatrix_, viewMatrix_, projectionMatrix_;

		Point				position_;
		Vector				scaling_; 
		float				rotX_;
		float				rotY_; 
		float				rotZ_;

		Vector				velocity_;

		FriendSpaceShipState friendSpaceShipState_;

};

#endif //_FRIEND_SPACESHIP_CLASS_H_
