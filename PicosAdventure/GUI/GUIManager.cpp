#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(const GUIManager& camera)
{
}

GUIManager::~GUIManager()
{
}

void GUIManager::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed)
	{
		default:
			{

			}
			break;
	}

	switch(arg.mouseButton)
	{
		case LEFT_BUTTON:
			{
				MessageBoxA(NULL, "Left pressed!", "Input", MB_OK);
			}
			break;
		case RIGHT_BUTTON:
			{
				MessageBoxA(NULL, "Right pressed!", "Input", MB_OK);
			}
			break;
		default:
			{

			}
			break;
	}
}