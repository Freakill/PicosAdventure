#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include "../Application/inputManager.h"

#include "../Utils/listenerClass.h"

class GUIManager : public Listener<InputManager, InputStruct>
{
	public:
		GUIManager();
		GUIManager(const GUIManager& camera);
		~GUIManager();

	private:
		virtual void notify(InputManager* notifier, InputStruct arg);
};

#endif //_GUI_MANAGER_H_