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
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; //The refresh rate for the change of buffer
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //This allows the swap chain to be used as rendering output
	swapChainDesc.OutputWindow = windowHandler;
    swapChainDesc.Windowed = true; //Specify if the application works in windowed mode
    swapChainDesc.SampleDesc.Count = 1; 
    swapChainDesc.SampleDesc.Quality = 0;

    unsigned int creationFlags = 0; //Create a bitwise ORed list of flags to activate when creating the device and context

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG; //If in debug mode we set the runtime layer to debug.
#endif

    HRESULT result;
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

	//Creating the Render Target View
    ID3D11Texture2D* backBufferTexture;

    result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBufferTexture);

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to get the swap chain back buffer.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

    result = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView_);

    if(backBufferTexture)
        backBufferTexture->Release();

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to create the render target view.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	d3dDeviceContext_->OMSetRenderTargets(1, &renderTargetView_, 0);

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
	float fieldOfView = (float)XM_PI / 4.0f;
	float screenAspect = (float)screenWidth_ / (float)screenHeight_;

	// Create the projection matrix for 3D rendering.
	projectionMatrix_ = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNearPlane, screenDepthPlane);

	// Initialize the world matrix to the identity matrix.
	worldMatrix_ = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	orthoMatrix_ = XMMatrixOrthographicLH((float)screenWidth_, (float)screenHeight_, screenNearPlane, screenDepthPlane);

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
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	d3dDeviceContext_->ClearRenderTargetView(renderTargetView_, color);

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

void GraphicsManager::getScreenSize(int& width, int& height)
{
	width = screenWidth_;
	height = screenHeight_;
	return;
}