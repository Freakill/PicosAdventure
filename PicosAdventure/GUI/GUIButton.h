#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_

#include "../Application/inputManager.h"

#include "../Math/pointClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Utils/textClass.h"
#include "../Utils/notifierClass.h"

class GUIButton : public Notifier<GUIButton, std::string>
{
	public:
		GUIButton();
		GUIButton(const GUIButton& camera);
		~GUIButton();

		bool setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix);

		std::string getName();

		Point getPosition();
		void setPosition(Point pos);
		
		int getWidth();
		void setWidth(int width);
		int getHeight();
		void setHeight(int height);

		bool offer(Point mouseClick);

	private:
		bool checkInside(Point pos);

		TextClass*  text_;
		ImageClass* background_;

		std::string buttonName_;

		Point position_;
		int	  width_;
		int   height_;
		
};

#endif //_GUI_BUTTON_H_