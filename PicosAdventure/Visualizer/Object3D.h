#ifndef _OBJECT_3D_H_
#define _OBJECT_3D_H_

#include "../Graphics/graphicsManager.h"
#include "../Graphics/modelClass.h"
#include "../Graphics/animatedModelClass.h"
#include "../Graphics/staticModelClass.h"
#include "../Graphics/textureClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

class Object3D
{
	public:
		virtual bool setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName) = 0;
		virtual void update(float dt) = 0;
		virtual void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix) = 0;
		virtual void destroy() = 0;

		void setName(std::string newName);
		std::string getName();

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

	protected:
		ModelClass*		model_;
		TextureClass*	diffuseTexture_;

		Shader3DClass*	modelShader_;

		std::string		objectName_;

		Point			position_;
		Vector			scaling_; 
		float			rotX_;
		float			rotY_; 
		float			rotZ_;
};

// Create function pointer
typedef Object3D* (__stdcall *CreateObject3DFn)(GraphicsManager*, std::string, std::string);

#endif //_OBJECT_3D_H_