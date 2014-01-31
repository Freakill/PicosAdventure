#include "graphicsManager.h"

GraphicsManager::GraphicsManager()
{
	swapChain_ = 0;
	dx11Device_ = 0;
	dx11DeviceContext_ = 0;
	renderTargetView_ = 0;
	depthStencilBuffer_ = 0;
	depthStencilState_ = 0;
	depthStencilView_ = 0;
	rasterState_ = 0;
	depthDisabledStencilState_ = 0;
	alphaEnableBlendingState_ = 0;
	alphaDisableBlendingState_ = 0;

	//shader2D_ = 0;
	//shader3D_ = 0;
}

GraphicsManager::GraphicsManager(const GraphicsManager& other)
{

}

GraphicsManager::~GraphicsManager()
{

}

bool GraphicsManager::setup(int width, int height, bool vsync, HWND windowHandler, bool fullscreen, float screenDepthPlane, float screenNearPlane)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModesDisplay, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDescription;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_RASTERIZER_DESC rasterDescription;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_BLEND_DESC blendStateDescription;

	vsyncEnabled_ = vsync;

	screenWidth_ = width;
	screenHeight_ = height;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot create DirectX factory.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	i = 0; 
	IDXGIAdapter * adapterTemp;
	DXGI_ADAPTER_DESC adapterDescriptionTemp;
	int videoCardMemoryTemp = 0;
	int bestAdapter = 0;
	while(factory->EnumAdapters(i, &adapterTemp) != DXGI_ERROR_NOT_FOUND) 
	{ 
		// Get the adapter (video card) description.
		result = adapterTemp->GetDesc(&adapterDescriptionTemp);
		if(FAILED(result))
		{
			MessageBoxA(NULL, "Cannot get adapter description.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		int videoCardMemory = (int)(adapterDescriptionTemp.DedicatedVideoMemory / 1024 / 1024);

		/*std::stringstream memory_stream;
		memory_stream << "Adapter number '" << bestAdapter << "' memory is '" << videoCardMemory << "'...";
		MessageBoxA(NULL, memory_stream.str().c_str(), "Open", MB_OK);*/

		if(videoCardMemory > videoCardMemoryTemp)
		{
			videoCardMemoryTemp = videoCardMemory;
			bestAdapter = i;
		}

		i++;
	}

	/*std::stringstream adapter_stream;
	adapter_stream << "Got that best adapter is number '" << bestAdapter << "'...";
	MessageBoxA(NULL, adapter_stream.str().c_str(), "Open", MB_OK);*/

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(bestAdapter, &adapter);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot enumerate graphic interfaces.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot enumerate primary adapter.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModesDisplay, NULL);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot get the number of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM display format.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModesDisplay];
	if(!displayModeList)
	{
		MessageBoxA(NULL, "Cannot list all possible display modes.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModesDisplay, displayModeList);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot fill the display mode list.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModesDisplay; i++)
	{
		if(displayModeList[i].Width == (unsigned int)width)
		{
			if(displayModeList[i].Height == (unsigned int)height)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDescription);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Cannot get adapter description.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory_ = (int)(adapterDescription.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, videoCardDescription_, 128, adapterDescription.Description, 128);
	if(error != 0)
	{
		MessageBoxA(NULL, "Cannot convert adapter name to character array.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

	// Set to a single back buffer.
	swapChainDescription.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDescription.BufferDesc.Width = width;
	swapChainDescription.BufferDesc.Height = height;

	// Set regular 32-bit surface for the back buffer.
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(vsyncEnabled_)
	{
		swapChainDescription.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDescription.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDescription.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDescription.OutputWindow = windowHandler;

	// Turn multisampling off.
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDescription.Windowed = false;
	}
	else
	{
		swapChainDescription.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDescription.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context. 
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, /*D3D11_CREATE_DEVICE_DEBUG |*/ D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1, 
										   D3D11_SDK_VERSION, &swapChainDescription, &swapChain_, &dx11Device_, NULL, &dx11DeviceContext_);
	if(FAILED(result)) // If computer does not support DX11
	{
		MessageBoxA(NULL, "Falling back to CPU mode.", "Info", MB_ICONINFORMATION | MB_OK);

		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, &featureLevel, 1, // Create CPU compatibility
										       D3D11_SDK_VERSION, &swapChainDescription, &swapChain_, &dx11Device_, NULL, &dx11DeviceContext_);
		if(FAILED(result))
		{
			return false;
		}
	}

	// Get the pointer to the back buffer.
	result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = dx11Device_->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView_);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDescription, sizeof(depthBufferDescription));

	// Set up the description of the depth buffer.
	depthBufferDescription.Width = width;
	depthBufferDescription.Height = height;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.SampleDesc.Quality = 0;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDescription.CPUAccessFlags = 0;
	depthBufferDescription.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = dx11Device_->CreateTexture2D(&depthBufferDescription, NULL, &depthStencilBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDescription, sizeof(depthStencilDescription));

	// Set up the description of the stencil state.
	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDescription.StencilEnable = true;
	depthStencilDescription.StencilReadMask = 0xFF;
	depthStencilDescription.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = dx11Device_->CreateDepthStencilState(&depthStencilDescription, &depthStencilState_);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	dx11DeviceContext_->OMSetDepthStencilState(depthStencilState_, 1);

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	// Set up the depth stencil view description.
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = dx11Device_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDescription, &depthStencilView_);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = dx11Device_->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState_);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = dx11Device_->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState_);
	if(FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = dx11Device_->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState_);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	dx11DeviceContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDescription.AntialiasedLineEnable = false;
	rasterDescription.CullMode = D3D11_CULL_BACK;
	rasterDescription.DepthBias = 0;
	rasterDescription.DepthBiasClamp = 0.0f;
	rasterDescription.DepthClipEnable = true;
	rasterDescription.FillMode = D3D11_FILL_SOLID;
	rasterDescription.FrontCounterClockwise = false;
	rasterDescription.MultisampleEnable = false;
	rasterDescription.ScissorEnable = false;
	rasterDescription.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = dx11Device_->CreateRasterizerState(&rasterDescription, &rasterState_);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	dx11DeviceContext_->RSSetState(rasterState_);

	// Setup the viewport for rendering.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	dx11DeviceContext_->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)width / (float)height;

	// Create the projection matrix for 3D rendering.
	//D3DXMatrixPerspectiveFovLH(&projectionMatrix_, fieldOfView, screenAspect, screenNearPlane, screenDepthPlane);

	// Initialize the world matrix to the identity matrix.
	//D3DXMatrixIdentity(&worldMatrix_);

	// Create an orthographic projection matrix for 2D rendering.
	//D3DXMatrixOrthoLH(&orthoMatrix_, (float)width, (float)height, screenNearPlane, screenDepthPlane);

	// We call the function to setup the shaders
	if(!setupShaders(windowHandler))
	{
		MessageBoxA(windowHandler, "Could not setup shaders.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

void GraphicsManager::destroy()
{
	// Release the 2D shader object.
	/*if(shader2D_)
	{
		shader2D_->destroy();
		delete shader2D_;
		shader2D_ = 0;
	}

	// Release the 3D shader object.
	if(shader3D_)
	{
		shader3D_->destroy();
		delete shader3D_;
		shader3D_ = 0;
	}*/

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(swapChain_)
	{
		swapChain_->SetFullscreenState(false, NULL);
	}

	if(alphaEnableBlendingState_)
	{
		alphaEnableBlendingState_->Release();
		alphaEnableBlendingState_ = 0;
	}

	if(alphaDisableBlendingState_)
	{
		alphaDisableBlendingState_->Release();
		alphaDisableBlendingState_ = 0;
	}

	if(rasterState_)
	{
		rasterState_->Release();
		rasterState_ = 0;
	}

	if(depthDisabledStencilState_)
	{
		depthDisabledStencilState_->Release();
		depthDisabledStencilState_ = 0;
	}

	if(depthStencilView_)
	{
		depthStencilView_->Release();
		depthStencilView_ = 0;
	}

	if(depthStencilState_)
	{
		depthStencilState_->Release();
		depthStencilState_ = 0;
	}

	if(depthStencilBuffer_)
	{
		depthStencilBuffer_->Release();
		depthStencilBuffer_ = 0;
	}

	if(renderTargetView_)
	{
		renderTargetView_->Release();
		renderTargetView_ = 0;
	}

	if(dx11DeviceContext_)
	{
		dx11DeviceContext_->Release();
		dx11DeviceContext_ = 0;
	}

	if(dx11Device_)
	{
		dx11Device_->Release();
		dx11Device_ = 0;
	}

	if(swapChain_)
	{
		swapChain_->Release();
		swapChain_ = 0;
	}

	return;
}

bool GraphicsManager::setupShaders(HWND windowHandler)
{
	// Create the image shader object.
	/*shader2D_ = new Shader2DClass;
	if(!shader2D_)
	{
		return false;
	}

	// Initialize the color shader object.
	if(!shader2D_->setup(dx11Device_, windowHandler))
	{
		MessageBoxA(NULL, "Could not initialize the 2D shader object.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Create the 3D shader object.
	shader3D_ = new Shader3DClass;
	if(!shader3D_)
	{
		return false;
	}

	// Initialize the color shader object.
	if(!shader3D_->setup(dx11Device_, windowHandler))
	{
		MessageBoxA(NULL, "Could not initialize the 3D shader object.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}*/

	return true;
}

void GraphicsManager::draw2D(int indexCount, XMMATRIX &worldMatrix, XMMATRIX &viewMatrix, XMMATRIX &orthoMatrix, ID3D11ShaderResourceView* texture, 
	                         XMFLOAT4 color)
{
	//shader2D_->draw(dx11DeviceContext_, indexCount, worldMatrix, viewMatrix, orthoMatrix, texture, color);
}

void GraphicsManager::draw3D(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX &worldMatrix,  XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, 
	                         ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor)
{
	//shader3D_->draw(dx11DeviceContext_, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor);
}

void GraphicsManager::beginDraw(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	dx11DeviceContext_->ClearRenderTargetView(renderTargetView_, color);
    
	// Clear the depth buffer.
	dx11DeviceContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void GraphicsManager::endDraw()
{
	// Present the back buffer to the screen since rendering is complete.
	if(vsyncEnabled_)
	{
		// Lock to screen refresh rate.
		swapChain_->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		swapChain_->Present(0, 0);
	}

	return;
}

/*Shader2DClass* GraphicsManager::getShader2D()
{
	return shader2D_;
}

Shader3DClass* GraphicsManager::getShader3D()
{
	return shader3D_;
}*/

ID3D11Device* GraphicsManager::getDevice()
{
	return dx11Device_;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext()
{
	return dx11DeviceContext_;
}

IDXGISwapChain* GraphicsManager::getSwapChain()
{
	return swapChain_;
}

void GraphicsManager::getProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = projectionMatrix_;
	return;
}

void GraphicsManager::getWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = worldMatrix_;
	return;
}

void GraphicsManager::getOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = orthoMatrix_;
	return;
}

void GraphicsManager::getVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, videoCardDescription_);
	memory = videoCardMemory_;
	return;
}

void GraphicsManager::getScreenSize(int& width, int& height)
{
	width = screenWidth_;
	height = screenHeight_;
	return;
}

void GraphicsManager::turnZBufferOn()
{
	dx11DeviceContext_->OMSetDepthStencilState(depthStencilState_, 1);
	return;
}


void GraphicsManager::turnZBufferOff()
{
	dx11DeviceContext_->OMSetDepthStencilState(depthDisabledStencilState_, 1);
	return;
}

void GraphicsManager::turnOnAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	dx11DeviceContext_->OMSetBlendState(alphaEnableBlendingState_, blendFactor, 0xffffffff);

	return;
}

void GraphicsManager::turnOffAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	dx11DeviceContext_->OMSetBlendState(alphaDisableBlendingState_, blendFactor, 0xffffffff);

	return;
}