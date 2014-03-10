#ifndef _POINTLIGHT_MULTITEXTURE_SHADER_3D_CLASS_H_
#define _POINTLIGHT_MULTITEXTURE_SHADER_3D_CLASS_H_

#include "shader3DClass.h"

class GraphicsManager;

#define NUM_LIGHTS 2

class PointlightMultiTextureShader3DClass : public Shader3DClass
{
	private:
		struct MixingBufferType
			{
				float percentage;
				XMFLOAT3 padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
			};

		struct LightColorBufferType
			{
				XMFLOAT4 diffuseColor[NUM_LIGHTS];
			};

		struct LightPositionBufferType
			{
				XMFLOAT4 lightPosition[NUM_LIGHTS];
			};

	public:
		PointlightMultiTextureShader3DClass();
		PointlightMultiTextureShader3DClass(const PointlightMultiTextureShader3DClass&);
		~PointlightMultiTextureShader3DClass();

		virtual bool setup(ID3D11Device* device);

		void setPositions(XMFLOAT4 pos1, XMFLOAT4 pos2);

		float getPercentage();
		void setPercentage(float percentage);

		static Shader3DClass* __stdcall Create(GraphicsManager* graphicsManager);
	private:
		virtual bool setupShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
		virtual bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
										 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light);

		// Multitexture
		ID3D11Buffer* mixingBuffer_;

		float mixingPercentage_;

		// Pointlights
		ID3D11Buffer* lightColorBuffer_;
		ID3D11Buffer* lightPositionBuffer_;

		XMFLOAT4	  lightPos1_;
		XMFLOAT4	  lightPos2_;

};

#endif //_MULTITEXTURE_SHADER_3D_CLASS_H_