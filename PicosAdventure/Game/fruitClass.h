#ifndef _FRUIT_CLASS_H_
#define _FRUIT_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"

#include "../Engine/sphereCollision.h"

#include <math.h>

enum FruitEffect
	{
		COLOR,
		TEXTURE,
		COMPLEMENT,
		BODY
	};

enum FruitState
	{
		SPAWNING,
		IN_TREE,
		SHACKEN,
		FALLING,
		IN_FLOOR
	};

class FruitClass : public Notifier<FruitClass, Point>
{
	public:
		FruitClass();
		FruitClass(const FruitClass&);
		~FruitClass();

		bool setup(GraphicsManager *graphicsManager, std::string fileName, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ);
		void update(float elapsedTime);
		void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
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

		SphereCollision* getCollisionSphere();

		void setFruitEffectType(FruitEffect effect);
		void setColorEffect(XMFLOAT4 color);

		void makeItFall();
		void resetFruit();

		bool hasFallen();

	private:
		Object3D*	model_;
		SphereCollision* collisionTest_;

		Point		initialPosition_;
		Point		position_;
		Vector		initialScaling_;
		Vector		endScaling_;
		Vector		scaling_; 
		Vector		velocity_;

		float		waitedTime_;
		float		spawningTime_;

		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		float		floorHeight_;

		bool		hasFallen_;

		FruitState	fruitState_;

		// EFFECTS
		FruitEffect fruitEffect_;
		XMFLOAT4	colorEffect_;
};

#endif //_FRUIT_CLASS_H_
