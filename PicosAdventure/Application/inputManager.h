#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include <windows.h>

#include <string>
#include <sstream>

#include "../Utils/notifierClass.h"

class InputManager : public Notifier<InputManager, int>
{
	public:
		InputManager();
		InputManager(const InputManager&);
		~InputManager();

		bool setup(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
		bool update();
		void destroy();

		void getMouseLocation(int&, int&);
		bool isLeftMouseButtonDown();

		void keyDown(unsigned int input);
	void keyUp(unsigned int input);

	private:
		bool readMouse();
		void processInput();

		IDirectInput8* directInput_;
		IDirectInputDevice8* mouse_;

		bool keys_[256];
		
		DIMOUSESTATE mouseState_;
		int mouseX_, mouseY_;

		int screenWidth_, screenHeight_;
};

#endif