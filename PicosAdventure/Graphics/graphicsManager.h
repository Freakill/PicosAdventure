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

		bool setup(HWND windowHandler, bool vsync, bool fullscreen, float screenDepthPlane, float screenNearPlane);
		void destroy();

		void beginDraw(float red, float green, float blue, float alpha);
		void endDraw();

		ID3D11Device* getDevice();
		ID3D11DeviceContext* getDeviceContext();
		IDXGISwapChain* getSwapChain();

		void getProjectionMatrix(XMMATRIX& projectionMatrix);
		void getWorldMatrix(XMMATRIX& worldMatrix);
		void getOrthoMatrix(XMMATRIX& orthoMatrix);

		void getScreenSize(int&, int&);

	private:
		HINSTANCE instanceHandler_;
		HWND windowHandler_;

		bool vsyncEnabled_;

		int screenWidth_;
		int screenHeight_;

		D3D_DRIVER_TYPE driverType_;
		D3D_FEATURE_LEVEL featureLevel_;

		// Pointer to all the variables, interfaces and structures to manage the rendering
		ID3D11Device* d3dDevice_;
		ID3D11DeviceContext* d3dDeviceContext_;
		IDXGISwapChain* swapChain_;
		ID3D11RenderTargetView* renderTargetView_;
		ID3D11DepthStencilView* depthStencilView_;

		XMMATRIX projectionMatrix_;
		XMMATRIX worldMatrix_;
		XMMATRIX orthoMatrix_;
};

#endif
