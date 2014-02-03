#ifndef _CAMERA_CLASS_H_
#define _CAMERA_CLASS_H_

#include <dxgi.h>
#include <xnamath.h>

class CameraClass
{
	public:
		CameraClass();
		CameraClass(const CameraClass& camera);
		~CameraClass();

		void setup(XMFLOAT3 position, XMFLOAT3 rotation);
		void update();

		void setPosition(float x, float y, float z);
		void setRotation(float x, float y, float z);

		XMFLOAT3 getPosition();
		XMFLOAT3 getRotation();

		void getViewMatrix(XMMATRIX& viewMatrix);

	private:
		float positionX_, positionY_, positionZ_;
		float rotationX_, rotationY_, rotationZ_;
		XMMATRIX viewMatrix_;
};

#endif //_CAMERA_CLASS_H_