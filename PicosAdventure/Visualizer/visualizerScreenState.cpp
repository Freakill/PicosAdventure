#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "visualizerScreenState.h"

VisualizerScreenState VisualizerScreenState::visualizerScreenState_;

VisualizerScreenState::VisualizerScreenState()
{
	camera_ = 0;
}

VisualizerScreenState::~VisualizerScreenState()
{

}

VisualizerScreenState* VisualizerScreenState::Instance()
{
	return (&visualizerScreenState_);
}

bool VisualizerScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	// Create GUI
	visualizerGUI_ = new GUIManager;
	inputManager->addListener(*visualizerGUI_);

	createLoadModelMenu();

	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	// Instantiate text
	/*int screenWidth, screenHeight;
	graphicsManager_->getScreenSize(screenWidth, screenHeight);

	text_ = new TextClass();
	if(!text_)
	{
		return false;
	}

	// Initialize the text object.
	if(!text_->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D() ,screenWidth, screenHeight, 20, 20, "HOLA"))
	{
		MessageBox(NULL, L"Could not initialize the Kinect Hand text object.", L"Error", MB_OK);
		return false;
	}*/

	inputManager->addListener(*this);

	return true;
}

void VisualizerScreenState::update(float elapsedTime)
{	
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		(*it)->update(elapsedTime);
	}
}

void VisualizerScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		(*it)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	}

	graphicsManager_->turnZBufferOff();
	graphicsManager_->turnOnAlphaBlending();

		//text_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);

		visualizerGUI_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);

	graphicsManager_->turnOffAlphaBlending();
	graphicsManager_->turnZBufferOn();
}

void VisualizerScreenState::destroy()
{
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		(*it)->destroy();
	}
}

void VisualizerScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		default:
			{
				
			}
			break;
	}

	switch(arg.mouseButton)
	{
		case WHEEL_SCROLL:
			{
				if(loadedObjects_.size() > 0)
				{
					// Create the new scaling vector
					Vector newScale = loadedObjects_.back()->getScale();

					// If wheel speen has been positive, whe calculate the new scaling vector
					if(arg.mouseInfo.z > 0)
					{
						newScale.x += 0.01f;
						newScale.y += 0.01f;
						newScale.z += 0.01f;
					}

					// otherwise, if the speen has been negative, we calculate new scaling vector and check it does not arrive to 0
					if(arg.mouseInfo.z < 0)
					{
						newScale.x -= 0.01f;
						newScale.y -= 0.01f;
						newScale.z -= 0.01f;

						if(newScale.x < 0.01f)
						{
							newScale = Vector(0.01f, 0.01f, 0.01f);
						}
					}

					loadedObjects_.back()->setScale(newScale);
				}
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void VisualizerScreenState::notify(GUIButton* notifier, std::string arg)
{
	createModel(arg);
}

void VisualizerScreenState::createModel(const std::string &modelName)
{
	Object3D* objectLoadedTemp = NULL;

	if(checkModelHasAnimations(modelName))
	{
		objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager_, modelName);
	}
	else
	{
		objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, modelName);
	}

	if(objectLoadedTemp)
	{
		loadedObjects_.push_back(objectLoadedTemp);
	}
	else
	{
		MessageBoxA(NULL, "Could not load the selected model.", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}
}

bool VisualizerScreenState::checkModelHasAnimations(const std::string &modelName)
{
	HANDLE dir;
    WIN32_FIND_DATAA file_data;

	// We specify in which folder we want to check whether there are animations or not
	std::string modelFolder = "./Data/models/" + modelName + "/*";
	bool hasAnimations = false;

	// If we can access to the folder, we will start checking for the folder "anims"
	if ((dir = FindFirstFileA(modelFolder.c_str(), &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::string file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if(file_name[0] == '.')
    			continue;

			// If we find a directory, check if it is called anims
    		if(is_directory)
			{
    			if(file_name == "anims")
				{
					hasAnimations = true;
				}
			}

			const std::string s( file_name.begin(), file_name.end() );
		} while (FindNextFileA(dir, &file_data));
	}

	// Return true in cas hasAnimations bool is set to true
	if(hasAnimations)
	{
		std::string textToDisplay = "The model " + modelName + " has animations.";
		MessageBoxA(NULL, textToDisplay.c_str(), "Animation Info", MB_OK);
		return true;
	}
	else
	{
		std::string textToDisplay = "The model " + modelName + " does NOT have animations.";
		MessageBoxA(NULL, textToDisplay.c_str(), "Animation Info", MB_OK);
	}
	
	return false;
}

void VisualizerScreenState::createLoadModelMenu()
{
	HANDLE dir;
    WIN32_FIND_DATA file_data;

	// Access to the root model folder (this is given by the structure of the game Data)
	GUIFrame* loadObjectsMenu = new GUIFrame();
	loadObjectsMenu->setup(graphicsManager_, "Load Models", Point(0.0f, 0.0f), 150, 200);

	// If we can access to that sctructure, then create a loadModel button for each found model
    if ((dir = FindFirstFile(L"./Data/models/*", &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::wstring file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if (file_name[0] == '.')
    			continue;

    		if (!is_directory)
    			continue;

			const std::string s( file_name.begin(), file_name.end() );

			// Create the button by calling the GUI frame where we want to add it
			loadObjectsMenu->addButton(graphicsManager_, s, 25)->addListener(*this);

		} while (FindNextFile(dir, &file_data));
	}

	visualizerGUI_->addFrame(loadObjectsMenu);
}