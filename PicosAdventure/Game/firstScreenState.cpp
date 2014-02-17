#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "firstScreenState.h"

FirstScreenState FirstScreenState::firstScreenState_;

FirstScreenState::FirstScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;
}

FirstScreenState::~FirstScreenState()
{

}

FirstScreenState* FirstScreenState::Instance()
{
	return (&firstScreenState_);
}

bool FirstScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	int screenWidth, screenHeight;
	graphicsManager_->getScreenSize(screenWidth, screenHeight);

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

	// Load the first level scenario
	loadScenario("level1");

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("terreno") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y-0.1f;
		}
	}

	// Set the level state to the first iteration and load fruits accordingly
	levelState_ = INTRODUCTION;
	subLevelState_ = PLAYING;

	loadConfigurationFromXML();

	// Create the Pico object.
	pico_ = new PicoClass();
	if(!pico_)
	{
		return false;
	}

	if(!pico_->setup(graphicsManager_, camera_))
	{
		MessageBoxA(NULL, "Could not initialize Pico :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	inputManager->addListener(*pico_);

	// Load the fruits
	loadFruits();

	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth, screenHeight, 20, 20, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "GUIFrame - Error", MB_OK);
		return false;
	}

	setupGUI(graphicsManager, inputManager);

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	return true;
}

void FirstScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	// Update fruits logic
	if(subLevelState_ == PLAYING)
	{
		std::vector<FruitClass*>::iterator fruitIt;
		for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
		{
			(*fruitIt)->update(elapsedTime);
		}
	
		pico_->update(elapsedTime);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				if(gameClock_->getTime() > fadeTime_)
				{
					changeLevel(FIRST_LEVEL);
				}
			}
			break;
		case FIRST_LEVEL:
			{
				updateFirsLevel();
			}
			break;
		default:
			{

			}
			break;
	}

	std::stringstream FPSText;
	FPSText << "FPS: " << 1/elapsedTime;
	FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());
}

void FirstScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	// Draw the loaded fruits in game
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		(*fruitIt)->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	}

	pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

	if(subLevelState_ == SELECT_POLAROID)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			polaroidGUI_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}

	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void FirstScreenState::destroy()
{
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}

	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		(*fruitIt)->destroy();
	}
}

void FirstScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 68: //D
		case 100: //d
			{
				debug_ = !debug_;
			}
			break;
		default:
			{
				
			}
			break;
	}

	switch(arg.mouseButton)
	{
		// Check if the left mouse is pressed to interested objects
		case LEFT_BUTTON:
			{
				std::vector<FruitClass*>::iterator fruitIt;
				for(fruitIt = fruits_.begin(); fruitIt != fruits_.end(); fruitIt++)
				{
					if((*fruitIt)->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
					{
						(*fruitIt)->makeItFall();
					}
				}
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void FirstScreenState::notify(GUIButton* notifier, ButtonStruct arg)
{
    switch(arg.buttonPurpose)
    {
        case(SELECT_OBJECT):
            {
				if(levelState_ == FIRST_LEVEL)
				{
					changeLevel(SECOND_LEVEL);
				}
            }
            break;
        default:
            {
            }
            break;
    }
}

void FirstScreenState::updateFirsLevel()
{
	switch(subLevelState_)
	{
		case PLAYING:
			{
				if(gameClock_->getTime() > playingTime_)
				{
					// check if Pico is idle
					subLevelState_ = FADING;
					gameClock_->reset();
				}
			}
			break;
		case FADING:
			{
				float diffuseTemp = 1.0f - gameClock_->getTime()/fadeTime_;
				light_->setDiffuseColor(diffuseTemp, diffuseTemp, diffuseTemp, 1.0f);

				float ambientTemp = 0.1f - (gameClock_->getTime()/fadeTime_)*0.075f;
				light_->setAmbientColor(ambientTemp, ambientTemp, ambientTemp, 1.0f);

				if(gameClock_->getTime() > fadeTime_)
				{
					createPolaroids();
					subLevelState_ = SELECT_POLAROID;
				}
			}
			break;
		case SELECT_POLAROID:
			{

			}
			break;
		default:
			{

			}
			break;
	}
}

void FirstScreenState::setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager)
{
	polaroidGUI_ = new GUIManager;

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	polaroidFrame_ = new GUIFrame;
	polaroidFrame_->setup(graphicsManager, "Polaroids", Point(0, 0), screenWidth, screenHeight);
	polaroidGUI_->addFrame(polaroidFrame_);

	inputManager->addListener(*this);
	inputManager->addListener(*polaroidGUI_);
}

void FirstScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level1/main_configuration.xml";

	//Loading animations XML file
	pugi::xml_document configurationDoc;
	if (!configurationDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load configuration .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = configurationDoc.child("configuration")))
	{
		MessageBoxA(NULL, "Could not load configuration node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node playTimeNode;
	if(!(playTimeNode = rootNode.child("play_time")))
	{
		MessageBoxA(NULL, "Could not load play time node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		playingTime_ = 15;
	}
	else
	{
		playingTime_ = playTimeNode.text().as_float();
	}

	pugi::xml_node fadeTimeNode;
	if(!(fadeTimeNode = rootNode.child("fade_time")))
	{
		MessageBoxA(NULL, "Could not load fade time node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		fadeTime_ = 10;
	}
	else
	{
		fadeTime_ = fadeTimeNode.text().as_float();
	}
}

void FirstScreenState::loadScenario(std::string scenario)
{
	HANDLE dir;
    WIN32_FIND_DATAA file_data;

	std::string scenarioToLoad = "./Data/configuration/"+scenario+"/scenario/*";

	// If we can access to that sctructure, then create a loadModel button for each found model
    if ((dir = FindFirstFileA(scenarioToLoad.c_str(), &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::string file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if (file_name[0] == '.')
    			continue;

    		if (is_directory)
    			continue;

			const std::string s( file_name.begin(), file_name.end() );

			createScenarioObject(scenario, s);

		} while (FindNextFileA(dir, &file_data));
	}
}

void FirstScreenState::createScenarioObject(std::string scenario, std::string xmlName)
{
	Object3D* objectLoadedTemp = NULL;

	std::string root = "./Data/configuration/"+ scenario + "/scenario/" + xmlName;

	//Loading animations XML file
	pugi::xml_document objectDoc;
	if (!objectDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load object .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = objectDoc.child(xmlName.substr(0, xmlName.size()-4).c_str())))
	{
		MessageBoxA(NULL, "Invalid .xml file! Could not find base node (must have object name).", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node modelNode;
	modelNode = rootNode.child("model");

	pugi::xml_text modelName = modelNode.text();
	objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, modelName.as_string());

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

		scenario_.push_back(objectLoadedTemp);
	}
	else
	{
		MessageBoxA(NULL, "Could not load the selected model.", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}
}

bool FirstScreenState::loadFruits()
{
	for(int i = 1; i < 5; i++)
	{
		std::stringstream root;
		root << "./Data/configuration/level1/fruits/fruits_" << i << ".xml";

		//Loading animations XML file
		pugi::xml_document fruitsDoc;
		if(!fruitsDoc.load_file(root.str().c_str()))
		{
			std::stringstream error;
			error << "Could not load object .xml file!" << root.str();
			MessageBoxA(NULL, error.str().c_str(), "FirstScreen - Fruit - Error", MB_ICONERROR | MB_OK);
			continue;
		}

		pugi::xml_node root_node;
		// Get initial node
		if(!(root_node = fruitsDoc.child("fruits")))
		{
			MessageBoxA(NULL, "Could not find the fruits root node.", "FirstScreen - Fruit - Error", MB_ICONERROR | MB_OK);
			continue;
		}

		for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
		{
			std::string node_name = fruitNode.name();
			// Actuamos en consecuencia segun el tipo de nodo
			if(node_name ==  "fruit")
			{
				pugi::xml_node modelNode;
				modelNode = fruitNode.child("model");

				pugi::xml_text modelName = modelNode.text();

				// Parse transformation data
				pugi::xml_node positionNode;
				positionNode = fruitNode.child("position");
				Point pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float());

				pugi::xml_node scaleNode;
				scaleNode = fruitNode.child("scale");
				Vector scale = Vector(scaleNode.attribute("x").as_float(), scaleNode.attribute("y").as_float(), scaleNode.attribute("z").as_float());

				pugi::xml_node rotationNode;
				rotationNode = fruitNode.child("rotation");
				float rotX = rotationNode.attribute("x").as_float();
				float rotY = rotationNode.attribute("y").as_float();
				float rotZ = rotationNode.attribute("z").as_float();

				FruitClass* fruit = new FruitClass();
				if(!fruit)
				{
					return false;
				}

				if(!fruit->setup(graphicsManager_, modelName.as_string(), pos, terrainHeight_, scale, rotX, rotY, rotZ))
				{
					MessageBoxA(NULL, "Could not initialize fruit.", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
					return false;
				}

				pugi::xml_node effectNode = fruitNode.child("effects");
				std::string effectType = effectNode.attribute("type").as_string();

				if(effectType == "color")
				{
					pugi::xml_node colorNode = effectNode.child("color");
					XMFLOAT4 color = XMFLOAT4(colorNode.attribute("r").as_float(), colorNode.attribute("g").as_float(), colorNode.attribute("b").as_float(), 1.0f);
				}

				if(effectType == "texture")
				{
					pugi::xml_node textureNode = effectNode.child("texture");

					TextureClass* temp = new TextureClass;
					if(!temp)
					{
						MessageBoxA(NULL, "Could not create texture!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}
					std::string textureName = textureNode.text().as_string();
					std::string filePath = "./Data/models/miniBossCuerpo/" + textureName + ".dds";
					bool result = temp->setup(graphicsManager_->getDevice(), filePath);
					if(!result)
					{
						MessageBoxA(NULL, "Could not load texture!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					fruit->setFruitEffectType(TEXTURE);
					fruit->setTextureEffect(temp);
				}

				if(effectType == "hat")
				{
					pugi::xml_node modelNode = effectNode.child("model");

					std::string modelName = modelNode.text().as_string();

					Object3D* temp = 0;
					temp = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, modelName);
					if(!temp)
					{
						MessageBoxA(NULL, "Could not load model!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					fruit->setFruitEffectType(HAT);
					fruit->setHatEffect(temp);
				}

				pugi::xml_node collisionNode = fruitNode.child("collision");

				positionNode = collisionNode.child("position");
				pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float());

				fruit->getCollisionSphere()->setRelativePosition(pos);

				pugi::xml_node radiusNode;
				radiusNode = collisionNode.child("radius");
				float radius = radiusNode.text().as_float();

				fruit->getCollisionSphere()->setRadius(radius);

				fruit->addListener(*pico_);

				fruits_.push_back(fruit);
			}
		}
	}

	return true;
}

void FirstScreenState::addFruitsToGame()
{
	for(int i = (levelState_-1)*4; i < (levelState_-1)*4+4; i++)
	{
		fruitsInGame_.push_back(fruits_.at(i));
	}
}

void FirstScreenState::clearFruits()
{
	// First we clean the fruits vector just in case
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruits_.begin(); fruitIt != fruits_.end(); fruitIt++)
	{
		(*fruitIt)->destroy();
	}
	fruits_.clear();
}

bool FirstScreenState::createPolaroids()
{
	std::stringstream root;
	root << "./Data/configuration/scenario1/polaroids/polaroids_" << levelState_ << ".xml";

	//Loading animations XML file
	pugi::xml_document polaroidsDoc;
	if(!polaroidsDoc.load_file(root.str().c_str()))
	{
		MessageBoxA(NULL, "Could not load polaroid .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = polaroidsDoc.child("polaroids")))
	{
		MessageBoxA(NULL, "Could not find the polaroids root node.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	int fruitIndex = 0;
	for(pugi::xml_node polaroidNode = root_node.first_child(); polaroidNode; polaroidNode = polaroidNode.next_sibling())
	{
		std::string node_name = polaroidNode.name();
		// Actuamos en consecuencia segun el tipo de nodo
		if(node_name ==  "polaroid")
		{
			if(fruits_.at(fruitIndex)->hasFallen())
			{
				pugi::xml_node imageNode;
				imageNode = polaroidNode.child("image");

				pugi::xml_text imageName = imageNode.text();

				// Parse transformation data
				pugi::xml_node positionNode;
				Point pos;
				if(!(positionNode = polaroidNode.child("position")))
				{
					MessageBoxA(NULL, "Could not find the polaroids position.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
					pos = Point(0, 0);
				}
				else{
					pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float());
				}

				pugi::xml_node sizeNode;
				Point size;
				if(!(sizeNode = polaroidNode.child("size")))
				{
					MessageBoxA(NULL, "Could not find the polaroids size.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
					size = Point(0, 0);
				}
				else{
					size = Point(sizeNode.attribute("x").as_float(), sizeNode.attribute("y").as_float());
				}

				polaroidFrame_->addButton(graphicsManager_, imageName.as_string(), pos, size)->addListener(*this);
			}
		}
		fruitIndex++;
	}

	return true;
}

void FirstScreenState::clearPolaroids()
{
	
}

void FirstScreenState::changeLevel(LevelState level)
{
	// Clear the fruits and polaroids vectors for next level
	fruitsInGame_.clear();
	clearPolaroids();

	// Reset light
	light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Set new level
	levelState_ = level;
	subLevelState_ = PLAYING;

	addFruitsToGame();

	// Reset clock
	gameClock_->reset();
}
