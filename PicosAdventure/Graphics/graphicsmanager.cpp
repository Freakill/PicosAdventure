#include "graphicsManager.h"

GraphicsManager::GraphicsManager()
{
	swapChain_ = 0;
	d3dDevice_ = 0;
	d3dDeviceContext_ = 0;
	renderTargetView_ = 0;
}

GraphicsManager::GraphicsManager(const GraphicsManager& other)
{

}

GraphicsManager::~GraphicsManager()
{

}

bool GraphicsManager::setup(HWND windowHandler, bool vsync, bool fullscreen, float screenDepthPlane, float screenNearPlane)
{
	windowHandler_ = windowHandler;

	//Rectangle of the active region of the screen window
	RECT dimensions; 
    GetClientRect( windowHandler, &dimensions );

    screenWidth_ = dimensions.right - dimensions.left;
    screenHeight_ = dimensions.bottom - dimensions.top;

	HRESULT result;

	//All the driver types we want to try to set when initializing i preference order
    D3D_DRIVER_TYPE driverTypes[] = 
    {
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE
    };

    unsigned int totalDriverTypes = ARRAYSIZE( driverTypes );

	//Feature levels we may accept in order of preference
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
    };

    unsigned int totalFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC swapChainDesc; //Creating the descriptor for the swap chain, collection of rendering destinations
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferCount = 1; //A value that describes the number of buffers in the swap chain. 
    swapChainDesc.BufferDesc.Width = screenWidth_; //Resolution width
    swapChainDesc.BufferDesc.Height = screenHeight_; //Resolution height
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Format of the buffer in 32bit per pixel with alpha channel
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; //The refresh rate for the change of buffer
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //This allows the swap chain to be used as rendering output
	swapChainDesc.OutputWindow = windowHandler;
    swapChainDesc.Windowed = true; //Specify if the application works in windowed mode
	swapChainDesc.SampleDesc.Count = 1; 
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	unsigned int creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED; //Create a bitwise ORed list of flags to activate when creating the device and context

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG; //If in debug mode we set the runtime layer to debug.
#endif

    unsigned int driver = 0;

    for(driver = 0; driver < totalDriverTypes; ++driver) //For each one of the possible specified objective drivers we try to create the device, context and swap chain
    {
        result = D3D11CreateDeviceAndSwapChain(0, //A pointer to the adapter to use. Pass NULL to use the default , the first adapter enumerated by IDXGIFactory1::EnumAdapters.
											   driverTypes[driver], //The driver type to create.
											   0, //A handle to a DLL if we were specifying a propietary software driver.
											   creationFlags, //The runtime layers to enable.
                                               featureLevels, //An array with the feature levels we will atempt to create in order.
											   totalFeatureLevels, //How many feature levels we will try to create.
                                               D3D11_SDK_VERSION, //The SDK version; use D3D11_SDK_VERSION.
											   &swapChainDesc, //A reference to the swap chain descriptor
											   &swapChain_, //Returns the address of a pointer to the IDXGISwapChain object that represents the swap chain used for rendering.
                                               &d3dDevice_, //Returns the address of a pointer to an ID3D11Device object that represents the device created.
											   &featureLevel_, //Returns a pointer to a D3D_FEATURE_LEVEL, which represents the first element of feature levels supported.
											   &d3dDeviceContext_ ); //Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context.

        if(SUCCEEDED(result))
        {
            driverType_ = driverTypes[driver]; //If we succeed we save the driver type and exit the loop for creating the device, context and swap chain.
            break;
        }
    }

    if(FAILED( result)) //If creation given the feature levels and driver types fail for all of them, we quit with a false message.
    {
		MessageBox(NULL, L"Failed to create the Direct3D device.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// Creating the Render Target View
    ID3D11Texture2D* backBufferTexture;

	// We obtain a pointer to the swap chain's back buffer
	result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to get the swap chain back buffer.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// We create a view to access the pipeline resource because DirectX does not allow to bind
	// resources directly to the pipeline, we need to access through the views
    result = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView_);

    if(backBufferTexture)
        backBufferTexture->Release();

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to create the render target view.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	ZeroMemory(&depthBufferDescription, sizeof(depthBufferDescription));

	// Set up the description of the depth buffer.
	depthBufferDescription.Width = screenWidth_;
	depthBufferDescription.Height = screenHeight_;
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
	result = d3dDevice_->CreateTexture2D(&depthBufferDescription, NULL, &depthStencilBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
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
	result = d3dDevice_->CreateDepthStencilState(&depthStencilDescription, &depthStencilState_);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	d3dDeviceContext_->OMSetDepthStencilState(depthStencilState_, 1);

	// Initailze the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	// Set up the depth stencil view description.
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = d3dDevice_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDescription, &depthStencilView_);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	d3dDeviceContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	//Creating the viewport
    D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(screenWidth_);
	viewport.Height = static_cast<float>(screenHeight_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    d3dDeviceContext_->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	float fieldOfView = (float)XM_PIDIV4;
	float screenAspect = (float)screenWidth_ / (float)screenHeight_;

	// Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&projectionMatrix_, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNearPlane, screenDepthPlane));

	// Initialize the world matrix to the identity matrix.
	XMStoreFloat4x4(&worldMatrix_, XMMatrixIdentity());

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&orthoMatrix_, XMMatrixOrthographicLH((float)screenWidth_, (float)screenHeight_, screenNearPlane, screenDepthPlane));

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
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(swapChain_)
	{
		swapChain_->SetFullscreenState(false, NULL);
	}

	if(renderTargetView_)
	{
		renderTargetView_->Release();
		renderTargetView_ = 0;
	}

	if(d3dDeviceContext_)
	{
		d3dDeviceContext_->Release();
		d3dDeviceContext_ = 0;
	}

	if(d3dDevice_)
	{
		d3dDevice_->Release();
		d3dDevice_ = 0;
	}

	if(swapChain_)
	{
		swapChain_->Release();
		swapChain_ = 0;
	}

	return;
}

void GraphicsManager::beginDraw(float red, float green, float blue, float alpha)
{
	// Setup the color to clear the buffer to.
	float color[] = {red, green, blue, alpha};

	// Clear the back buffer.
	d3dDeviceContext_->ClearRenderTargetView(renderTargetView_, color);

	// Clear the depth buffer.
	d3dDeviceContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

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

bool GraphicsManager::setupShaders(HWND windowHandler)
{
	// Create the 3D shader object.
	shader3D_ = new Shader3DClass;
	if(!shader3D_)
	{
		return false;
	}

	// Initialize the color shader object.
	if(!shader3D_->setup(d3dDevice_, windowHandler))
	{
		MessageBoxA(NULL, "Could not initialize the 3D shader object.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}
void GraphicsManager::draw3D(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 worldMatrix,  XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, 
	                         ID3D11ShaderResourceView* texture)
{
	shader3D_->draw(d3dDeviceContext_, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

ID3D11Device* GraphicsManager::getDevice()
{
	return d3dDevice_;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext()
{
	return d3dDeviceContext_;
}

IDXGISwapChain* GraphicsManager::getSwapChain()
{
	return swapChain_;
}

void GraphicsManager::getProjectionMatrix(XMFLOAT4X4& projectionMatrix)
{
	projectionMatrix = projectionMatrix_;
	return;
}

void GraphicsManager::getWorldMatrix(XMFLOAT4X4& worldMatrix)
{
	worldMatrix = worldMatrix_;
	return;
}

void GraphicsManager::getOrthoMatrix(XMFLOAT4X4& orthoMatrix)
{
	orthoMatrix = orthoMatrix_;
	return;
}

void GraphicsManager::getScreenSize(int& width, int& height)
{
	width = screenWidth_;
	height = screenHeight_;
	return;
}