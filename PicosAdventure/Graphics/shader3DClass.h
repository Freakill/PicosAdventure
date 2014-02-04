#ifndef _SHADER_3D_CLASS_H_
#define _SHADER_3D_CLASS_H_

#pragma warning disable C4005

#include <dxgi.h>
#include <d3d11.h>
#include <d3dx11async.h>
#include <xnamath.h>

#include <fstream>

struct TexturedVertexType
		{
			XMFLOAT3 position;
			XMFLOAT3 normal;
			float tu, tv;
		};

class Shader3DClass
{
	private:
		struct MatrixBufferType
		{
			XMFLOAT4X4 world;
			XMFLOAT4X4 view;
			XMFLOAT4X4 projection;
		};

	public:
		Shader3DClass();
		Shader3DClass(const Shader3DClass&);
		~Shader3DClass();

		bool setup(ID3D11Device* device, HWND windowHandler);
		void destroy();
		bool draw(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 worldMatrix, 
			      XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView* texture);

	private:
		bool setupShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
		void drawShader(ID3D11DeviceContext* deviceContext, int indexCount);
		void destroyShader();
		void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

		bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, 
								 XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView* texture);

	private:
		ID3D11VertexShader* vertexShader_;
		ID3D11PixelShader* pixelShader_;
		ID3D11InputLayout* layout_;
		ID3D11SamplerState* sampleState_;
		ID3D11Buffer* matrixBuffer_;
};

#endif
