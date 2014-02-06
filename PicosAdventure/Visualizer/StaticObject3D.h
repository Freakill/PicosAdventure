#ifndef _STATIC_OBJECT_3D_H_
#define _STATIC_OBJECT_3D_H_

#include "Object3D.h"

class StaticObject3D : public Object3D
{
	public:
		StaticObject3D();
		StaticObject3D(const StaticObject3D &);
		~StaticObject3D();

		virtual bool setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName);
		virtual void update(float dt);
		virtual void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);
		virtual void destroy();

		static Object3D* __stdcall Create(GraphicsManager* graphicsManager, std::string modelName, std::string objectName);
	private:
		
};

#endif //_STATIC_OBJECT_3D_H_