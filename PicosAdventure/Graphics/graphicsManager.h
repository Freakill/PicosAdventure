#ifndef _GRAPHICS_MANAGER_H_
#define _GRAPHICS_MANAGER_H_

//Including DirectX libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include <dxgi.h>
#include <d3d11.h>
#include <xnamath.h>

#include <iostream>
#include <fstream>
#include <sstream>

//#include "shader3DClass.h"
//#include "shader2DClass.h"

class GraphicsManager
{
	public:
		GraphicsManager();
		GraphicsManager(const GraphicsManager&);
		~GraphicsManager();

		bool setup(int width, int height, bool vsync, HWND windowHandler, bool fullscreen, float screenDepthPlane, float screenNearPlane);
		void destroy();

		bool setupShaders(HWND windowHandler);
	
		void draw2D(int indexCount, XMMATRIX &worldMatrix, XMMATRIX &viewMatrix, XMMATRIX &orthoMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color);
		void draw3D(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX &worldMatrix, 
			        XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture,
				    XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor);

		void beginDraw(float red, float green, float blue, float alpha);
		void endDraw();

		//Shader2DClass* getShader2D();
		//Shader3DClass* getShader3D();

		ID3D11Device* getDevice();
		ID3D11DeviceContext* getDeviceContext();
		IDXGISwapChain* getSwapChain();

		void getProjectionMatrix(XMMATRIX& projectionMatrix);
		void getWorldMatrix(XMMATRIX& worldMatrix);
		void getOrthoMatrix(XMMATRIX& orthoMatrix);

		void getVideoCardInfo(char* cardName, int& memory);

		void turnZBufferOn();
		void turnZBufferOff();

		void turnOnAlphaBlending();
		void turnOffAlphaBlending();

		void getScreenSize(int&, int&);

	private:
		bool vsyncEnabled_;
		int videoCardMemory_;
		char videoCardDescription_[128];

		int screenWidth_;
		int screenHeight_;

		// Pointer to all the variables, interfaces and structures to manage the rendering
		IDXGISwapChain* swapChain_;
		ID3D11Device* dx11Device_;
		ID3D11DeviceContext* dx11DeviceContext_;
		ID3D11RenderTargetView* renderTargetView_;
		ID3D11Texture2D* depthStencilBuffer_;
		ID3D11DepthStencilState* depthStencilState_;
		ID3D11DepthStencilView* depthStencilView_;
		ID3D11RasterizerState* rasterState_;
		ID3D11DepthStencilState* depthDisabledStencilState_;
		ID3D11BlendState* alphaEnableBlendingState_;
		ID3D11BlendState* alphaDisableBlendingState_;

		XMMATRIX projectionMatrix_;
		XMMATRIX worldMatrix_;
		XMMATRIX orthoMatrix_;

		// Pointers to shaders
		//Shader2DClass* shader2D_;
		//Shader3DClass* shader3D_;

};

#endif
