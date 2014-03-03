#ifndef _PICO_SECOND_CLASS_H_
#define _PICO_SECOND_CLASS_H_

#include "../Application/inputManager.h"

#include "../Engine/Object3DFactory.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/clockClass.h"
#include "../Utils/textClass.h"

#include "pieceClass.h"

#include <deque>

class PicoSecondClass : public Listener<InputManager, InputStruct>, public Listener<PieceClass, Point>, public Notifier<PicoSecondClass, bool>
{
	private:
		enum PicoStates
			{
				WAITING,
				WALKING,
				TURNING,
				CELEBRATING,
				LEAVING
			};

		enum FaceStates
			{
				NORMAL,
				CHANGING,
				CHANGED
			};

	public:
		PicoSecondClass();
		PicoSecondClass(const PicoSecondClass&);
		~PicoSecondClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setLevelState(int level);

		void goToPosition(Point position);
		void setToRest();

		void makeHappy();

		void sayHello();

		void makePointing();

		void makeLeave();

		void changeAnimation(std::string name, float time);
		void executeAnimation(std::string name, float time);
		void changeExpression(std::string newExpression);

		Point getPosition();

		void setTipsColor(XMFLOAT4 color);
		void setBodyTexture(TextureClass* texture);
		void setHat(Object3D* hat);
		void setBody(Object3D* body);
		void setTips(Object3D* tips);

		SphereCollision* getCollisionSphere();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(PieceClass* notifier, Point arg);

	private:
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		bool lookAtCamera(bool check);

		bool checkPicoArrivedObjective();

		CameraClass* camera_;

		// 3D Models
		Object3D*	body_;
		Object3D*	tips_;
		Object3D*	eyes_;
		Object3D*	hat_;

		SphereCollision* collisionTest_;

		int			levelState_;

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
		ClockClass* inactivityClock_;

		float		waitedTime_;
		float		eatingWaitTime_;
		float		celebratingWaitTime_;

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

		TextClass*	info_;
};

#endif //_PICO_SECOND_CLASS_H_
