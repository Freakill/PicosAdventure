#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include <map>

#include "../Application/inputManager.h"

#include "../Utils/listenerClass.h"

#include "GUIFrame.h"

class GUIManager : public Listener<InputManager, InputStruct>
{
	public:
		GUIManager();
		GUIManager(const GUIManager& camera);
		~GUIManager();

		void update(float elapsedTime);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix);

		void addFrame(GUIFrame* frame);

	private:
		virtual void notify(InputManager* notifier, InputStruct arg);

		std::map<std::string, GUIFrame*> guiFrames_;
};

#endif //_GUI_MANAGER_H_