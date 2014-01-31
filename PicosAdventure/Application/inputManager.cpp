#include "inputManager.h"

InputManager::InputManager()
{
	directInput_ = 0;
	mouse_ = 0;
}

InputManager::InputManager(const InputManager& other)
{

}

InputManager::~InputManager()
{

}

bool InputManager::setup(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Initialize the location of the mouse on the screen.
	mouseX_ = 0;
	mouseY_ = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the direct input interface.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize all the keys to being released and not pressed.
	for(int i=0; i<256; i++)
	{
		keys_[i] = false;
	}

	// Initialize the direct input interface for the mouse.
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not create mouse interface.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not set the mouse format.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}
	// Set the cooperative level of the mouse to share with other programs.
	result = mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not set the mouse to not being shared.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Acquire the mouse.
	result = mouse_->Acquire();

	return true;
}

bool InputManager::update()
{
	bool result;

	// Read the current state of the mouse.
	result = readMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	processInput();

	return true;
}

void InputManager::destroy()
{
	// Release the mouse.
	if(mouse_)
	{
		mouse_->Unacquire();
		mouse_->Release();
		mouse_ = 0;
	}

	// Release the main interface to direct input.
	if(directInput_)
	{
		directInput_->Release();
		directInput_ = 0;
	}

	return;
}

void InputManager::getMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = mouseX_;
	mouseY = mouseY_;
	return;
}

bool InputManager::isLeftMouseButtonDown()
{
	// Check if the left mouse button is currently pressed.
	if(mouseState_.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

void InputManager::keyDown(unsigned int input)
{
	if(!keys_[input]){
		notifyListeners((int)input);
	}

	// If a key is pressed then save that state in the key array.
	keys_[input] = true;
	return;
}

void InputManager::keyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	keys_[input] = false;
	return;
}

bool InputManager::readMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState_);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse_->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputManager::processInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	mouseX_ += mouseState_.lX;
	mouseY_ += mouseState_.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(mouseX_ < 0)
	{ 
		mouseX_ = 0; 
	}
	if(mouseY_ < 0)
	{
		mouseY_ = 0;
	}
	
	if(mouseX_ > screenWidth_)
	{ 
		mouseX_ = screenWidth_; 
	}
	if(mouseY_ > screenHeight_)
	{
		mouseY_ = screenHeight_;
	}
	
	return;
}
