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
	light_ = 0;

	lastLoadedObject_ = 0;
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
	createLoadXMLMenu();

	loadedObjectsMenu_ = new GUIFrame();
	loadedObjectsMenu_->setup(graphicsManager_, "Loaded Models", Point(300.0f, 0.0f), 150, 200);
	visualizerGUI_->addFrame(loadedObjectsMenu_);

	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	// Create the light object.
	light_ = new LightClass;
	if(!light_)
	{
		return false;
	}

	// Initialize the light object.
	light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light_->setDirection(0.0f, -1.0f, 1.0f);

	inputManager->addListener(*this);

	playingAnimations_ = false;

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

	// We iterate over each loaded Object to call its draw function
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		(*it)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	// Draw the GUI
	graphicsManager_->turnZBufferOff();
	graphicsManager_->turnOnAlphaBlending();
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
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case 65: //A
		case 81: //Q
		case 97: //a
		case 113: //q
			{
				moveSelectedObjects(arg);
			}
			break;
		case 79: //O
		case 111: //o
		case 80: //P
		case 112: //p
			{
				rotateSelectedObjects(arg);
			}
			break;
		case 115: //s
		case 83: //S
			{
				saveSelectedObjects();
			}
			break;
		case 100: //d
		case 68: //D
			{
				deleteSelectedObjects();
			}
			break;
		case 99: //m
		case 77: //M
			{
				increaseAnimations();
				if(!playingAnimations_)
				{
					stopAnimations();
				}
			}
			break;
		case 110: //n
		case 78: //N
			{
				decreaseAnimations();
				if(!playingAnimations_)
				{
					stopAnimations();
				}
			}
			break;
		case VK_SPACE:
			{
				if(playingAnimations_)
				{
					stopAnimations();
				}
				else
				{
					playAnimations();
				}
			}
			break;
		default:
			{
				
			}
			break;
	}

	switch(arg.mouseButton)
	{
		case WHEEL_SCROLL:
			{
				resizeSelectedObjects(arg);
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void VisualizerScreenState::notify(GUIButton* notifier, ButtonStruct arg)
{
	switch(arg.buttonPurpose)
	{
		case(LOAD_OBJECT):
			{
				if(arg.buttonInfo.substr(arg.buttonInfo.size()-4, arg.buttonInfo.size()-1) == ".xml")
				{
					createXMLModel(arg.buttonInfo);
				}
				else
				{
					createModel(arg.buttonInfo);
				}
			}
			break;
		case(SELECT_OBJECT):
			{
				int index = 0;
				std::vector<Object3D*>::iterator it;
				for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
				{
					if(arg.buttonInfo == (*it)->getName())
					{
						lastLoadedObject_ = index;
					}
					index++;
				}
			}
			break;
		default:
			{
			}
			break;
	}
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
		createLoadedObjectButton();
		stopAnimations();
	}
	else
	{
		MessageBoxA(NULL, "Could not load the selected model.", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}
}

void VisualizerScreenState::createXMLModel(const std::string &xmlName)
{
	Object3D* objectLoadedTemp = NULL;

	std::string root = "./Data/scenario/" + xmlName;

	//Loading animations XML file
	pugi::xml_document objectDoc;
	if (!objectDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load object .xml file!", "AnimatedModel - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node where all "anim" nodes should be
	pugi::xml_node rootNode;
	if(!(rootNode = objectDoc.child(xmlName.substr(0, xmlName.size()-4).c_str())))
	{
		MessageBoxA(NULL, "Invalid .xml file! Could not find base node (must have object name).", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node modelNode;
	modelNode = rootNode.child("model");

	pugi::xml_text modelName = modelNode.text();

	if(checkModelHasAnimations(modelNode.value()))
	{
		objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager_, modelName.as_string());
	}
	else
	{
		objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, modelName.as_string());
	}

	if(objectLoadedTemp)
	{
		// Parse transformation data
		pugi::xml_node positionNode;
		positionNode = rootNode.child("position");
		objectLoadedTemp->setPosition(Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float()));

		pugi::xml_node scaleNode;
		scaleNode = rootNode.child("scale");
		objectLoadedTemp->setScale(Vector(scaleNode.attribute("x").as_float(), scaleNode.attribute("y").as_float(), scaleNode.attribute("z").as_float()));

		pugi::xml_node rotationNode;
		rotationNode = rootNode.child("rotation");
		objectLoadedTemp->setRotationX(rotationNode.attribute("x").as_float());
		objectLoadedTemp->setRotationY(rotationNode.attribute("y").as_float());
		objectLoadedTemp->setRotationZ(rotationNode.attribute("z").as_float());

		loadedObjects_.push_back(objectLoadedTemp);
		createLoadedObjectButton();
		stopAnimations();
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
	loadModelsMenu_ = new GUIFrame();
	loadModelsMenu_->setup(graphicsManager_, "Load Models", Point(0.0f, 0.0f), 150, 200);

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
			loadModelsMenu_->addButton(graphicsManager_, s, 25, LOAD_OBJECT)->addListener(*this);

		} while (FindNextFile(dir, &file_data));
	}

	visualizerGUI_->addFrame(loadModelsMenu_);
}

void VisualizerScreenState::createLoadXMLMenu()
{
	HANDLE dir;
    WIN32_FIND_DATA file_data;

	// Access to the root model folder (this is given by the structure of the game Data)
	loadXMLMenu_ = new GUIFrame();
	loadXMLMenu_->setup(graphicsManager_, "Load XMLs", Point(150.0f, 0.0f), 150, 200);

	// If we can access to that sctructure, then create a loadModel button for each found model
    if ((dir = FindFirstFile(L"./Data/scenario/*", &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::wstring file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if (file_name[0] == '.')
    			continue;

    		if (is_directory)
    			continue;

			const std::string s( file_name.begin(), file_name.end() );

			// Create the button by calling the GUI frame where we want to add it
			loadXMLMenu_->addButton(graphicsManager_, s, 25, LOAD_OBJECT)->addListener(*this);

		} while (FindNextFile(dir, &file_data));
	}

	visualizerGUI_->addFrame(loadXMLMenu_);
}

void VisualizerScreenState::createLoadedObjectButton()
{
	loadedObjectsMenu_->addButton(graphicsManager_, loadedObjects_.back()->getName(), 25, SELECT_OBJECT)->addListener(*this);
}

void VisualizerScreenState::playAnimations()
{
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		AnimatedObject3D* animatedObject = dynamic_cast<AnimatedObject3D*>((*it));
		if(animatedObject)
		{
			animatedObject->playAnimation();
		}
	}

	playingAnimations_ = true;
}

void VisualizerScreenState::stopAnimations()
{
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		AnimatedObject3D* animatedObject = dynamic_cast<AnimatedObject3D*>((*it));
		if(animatedObject)
		{
			animatedObject->stopAnimation();
		}
	}

	playingAnimations_ = false;
}

void VisualizerScreenState::increaseAnimations()
{
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		AnimatedObject3D* animatedObject = dynamic_cast<AnimatedObject3D*>((*it));
		if(animatedObject)
		{
			AnimatedCal3DModelClass* model = dynamic_cast<AnimatedCal3DModelClass*>(animatedObject->getModel());
			model->increaseAnimationToDisplay();
		}
	}

	playingAnimations_ = false;
}

void VisualizerScreenState::decreaseAnimations()
{
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		AnimatedObject3D* animatedObject = dynamic_cast<AnimatedObject3D*>((*it));
		if(animatedObject)
		{
			AnimatedCal3DModelClass* model = dynamic_cast<AnimatedCal3DModelClass*>(animatedObject->getModel());
			model->decreaseAnimationToDisplay();
		}
	}

	playingAnimations_ = false;
}

void VisualizerScreenState::moveSelectedObjects(InputStruct arg)
{
	// Iterate over the loaded objects
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		// Check if object button is active
		if(loadedObjectsMenu_->getButtonIsActive((*it)->getName()))
		{
			// Create the new poisiton
			Point newPosition = (*it)->getPosition();

			switch(arg.keyPressed){
				case VK_LEFT:
					{
						newPosition.x -= 0.25f;
					}
					break;
				case VK_RIGHT:
					{
						newPosition.x += 0.25f;
					}
					break;
				case VK_UP:
					{
						newPosition.z += 0.25f;
					}
					break;
				case VK_DOWN:
					{
						newPosition.z -= 0.25f;
					}
					break;
				case 65: //A
				case 97: //a
					{
						newPosition.y -= 0.1f;
					}
					break;
				case 81: //Q
				case 113: //q
					{
						newPosition.y += 0.1f;
					}
					break;
			}

			(*it)->setPosition(newPosition);
		}
	}
}

void VisualizerScreenState::rotateSelectedObjects(InputStruct arg)
{
	// Iterate over the loaded objects
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		// Check if object button is active
		if(loadedObjectsMenu_->getButtonIsActive((*it)->getName()))
		{
			// Create the new poisiton
			float newRotY = (*it)->getRotationY();

			switch(arg.keyPressed){
				case 79: //O
				case 111: //o
					{
						newRotY += XM_PI/16.0f;
						if(newRotY > XM_2PI)
						{
							newRotY -= XM_2PI;
						}
					}
					break;
				case 80: //P
				case 112: //p
					{
						newRotY -= XM_PI/16.0f;
						if(newRotY < 0.0f)
						{
							newRotY += XM_2PI;
						}
					}
					break;
			}

			(*it)->setRotationY(newRotY);
		}
	}
}

void VisualizerScreenState::resizeSelectedObjects(InputStruct arg)
{
	// Iterate over the loaded objects
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		// Check if object button is active
		if(loadedObjectsMenu_->getButtonIsActive((*it)->getName()))
		{
			// Create the new scaling vector
			Vector newScale = (*it)->getScale();

			// If wheel speen has been positive, whe calculate the new scaling vector
			if(arg.mouseInfo.z > 0)
			{
				newScale.x += newScale.x*0.1f;
				newScale.y += newScale.y*0.1f;
				newScale.z += newScale.z*0.1f;
			}

			// otherwise, if the speen has been negative, we calculate new scaling vector and check it does not arrive to 0
			if(arg.mouseInfo.z < 0)
			{
				newScale.x -= newScale.x*0.1f;
				newScale.y -= newScale.y*0.1f;
				newScale.z -= newScale.z*0.1f;

				if(newScale.x < 0.01f)
				{
					newScale = Vector(0.01f, 0.01f, 0.01f);
				}
			}

			(*it)->setScale(newScale);
		}
	}
}

void VisualizerScreenState::saveSelectedObjects()
{
	// Iterate over the loaded objects
	std::vector<Object3D*>::iterator it;
	for(it = loadedObjects_.begin(); it != loadedObjects_.end(); it++)
	{
		// Check if object button is active
		if(loadedObjectsMenu_->getButtonIsActive((*it)->getName()))
		{
			// get a test document
			pugi::xml_document doc;

			// add node with some name
			pugi::xml_node node = doc.append_child((*it)->getName().c_str());

			pugi::xml_node model = node.append_child("model");
			model.append_child(pugi::node_pcdata).set_value((*it)->getModelName().c_str());

			// add position node
			pugi::xml_node position = node.append_child("position");

			// add attributes to position node
			Point pos = (*it)->getPosition();
			position.append_attribute("x") = pos.x;
			position.append_attribute("y") = pos.y;
			position.append_attribute("z") = pos.z;

			// add scale node
			pugi::xml_node scale = node.append_child("scale");

			// add attributes to scale node
			Vector scal = (*it)->getScale();
			scale.append_attribute("x") = scal.x;
			scale.append_attribute("y") = scal.y;
			scale.append_attribute("z") = scal.z;

			// add rotation node
			pugi::xml_node rotation = node.append_child("rotation");

			// add attributes to scale node
			rotation.append_attribute("x") = (*it)->getRotationX();
			rotation.append_attribute("y") = (*it)->getRotationY();
			rotation.append_attribute("z") = (*it)->getRotationZ();

			// Generate fileName
			std::string fileName = "Data/scenario/" + (*it)->getName() + ".xml";

			// save document to file
			std::cout << "Saving result: " << doc.save_file(fileName.c_str()) << std::endl;

			loadXMLMenu_->addButton(graphicsManager_, (*it)->getName()+".xml", 25, LOAD_OBJECT)->addListener(*this);
		}
	}
}

void VisualizerScreenState::deleteSelectedObjects()
{
	// Iterate over the loaded objects
	std::vector<Object3D*>::iterator it;
	it = it = loadedObjects_.begin(); 
	while(it != loadedObjects_.end())
	{
		// Check if object button is active
		if(loadedObjectsMenu_->getButtonIsActive((*it)->getName()))
		{
			loadedObjectsMenu_->deleteButton((*it)->getName());
			it = loadedObjects_.erase(it);
			continue;
		}

		it++;
	}
}
