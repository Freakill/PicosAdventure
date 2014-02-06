#include "GUIButton.h"

GUIButton::GUIButton()
{
	buttonName_ = "";
	position_ = Point(0.0f, 0.0f);
	width_ = 0;
	height_ = 0;
}

GUIButton::GUIButton(const GUIButton& camera)
{
}

GUIButton::~GUIButton()
{
}

bool GUIButton::setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height, ButtonPurpose purpose)
{
	buttonName_ = name;
	position_ = position;
	width_ = width;
	height_ = height;
	buttonPurpose_ = purpose;

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	// Create the bitmap object.
	background_ = new ImageClass();
	if(!background_)
	{
		return false;
	}

	// Initialize the bitmap object.
	if(!background_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth, screenHeight, "button", width_, height_))
	{
		std::string textToDisplay = "Could not initialize the frame background object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	text_ = new TextClass();
	if(!text_)
	{
		return false;
	}

	// Initialize the text object.
	if(!text_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth, screenHeight, position_.x+10, position_.y+5, buttonName_))
	{
		std::string textToDisplay = "Could not initialize the frame text object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}
	
	return true;
}

void GUIButton::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix)
{
	background_->draw(deviceContext, position_.x, position_.y-2, worldMatrix, viewMatrix, orthoMatrix);

	text_->draw(deviceContext, worldMatrix, viewMatrix, orthoMatrix);
}

std::string GUIButton::getName()
{
	return buttonName_;
}

Point GUIButton::getPosition()
{
	return position_;
}

void GUIButton::setPosition(Point pos)
{
	position_ = pos;
}
		
int GUIButton::getWidth()
{
	return width_;
}

void GUIButton::setWidth(int width)
{
	width = width_;
}

int GUIButton::getHeight()
{
	return height_;
}

void GUIButton::setHeight(int height)
{
	height_ = height;
}

bool GUIButton::offer(Point mouseClick)
{
	if(checkInside(mouseClick))
	{
		ButtonStruct buttonStruct = {buttonPurpose_, buttonName_};
		notifyListeners(buttonStruct);
	}

	return false;
}

bool GUIButton::checkInside(Point pos)
{
	if(pos.x >= position_.x && pos.x <= position_.x+width_ &&
	   pos.y >= position_.y && pos.y <= position_.y+height_)
	{
		return true;
	}

	return false;
}