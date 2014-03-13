#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "thirdScreenState.h"

ThirdScreenState ThirdScreenState::thirdScreenState_;

ThirdScreenState::ThirdScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	spaceShip_ = 0;

	background_ = 0;

	for(int i = 0; i < 5; i++)
	{
		friendSpacheShips_[0];
	}
}

ThirdScreenState::~ThirdScreenState()
{

}

ThirdScreenState* ThirdScreenState::Instance()
{
	return (&thirdScreenState_);
}

bool ThirdScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

	LogClass::Instance()->setup("second_level_log");

	// Subscribe to keyboard and mouse manager
	inputManager->addListener(*this);

	// Change kinect settings to make user being drawn darker
	kinectClass_ = kinectManager;

	kinectClass_->setUserColor(XMFLOAT4(0.25f, 0.25f, 0.35f, 0.5f));
	kinectClass_->addListener(*this);

	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	if(!setupSplashScreen())
	{
		return false;
	}

	drawSplashScreen();

	loadConfigurationFromXML();

	// Create the light object.
	light_ = new LightClass;
	if(!light_)
	{
		return false;
	}

	// Initialize the light object.
	light_->setAmbientColor(0.6f, 0.6f, 0.6f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light_->setDirection(0.0f, -0.5f, 1.0f);

	// SOUND
	/*soundManager_ = new SoundSecondClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler()))
	{
		MessageBox(NULL, L"Could not initialize Direct Sound.", L"SecondScreen - Error", MB_OK);
		return false;
	}*/

	// Load the first level scenario
	loadScenario("level3");
	
	// Create the space ship.
	spaceShip_ = new SpaceShipClass;
	if(!spaceShip_)
	{
		return false;
	}
	if(!spaceShip_->setup(graphicsManager_, soundManager_))
	{
		MessageBoxA(NULL, "Could not initialize the SpaceShip.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	spaceShip_->getObject()->setShader3D(shaderTemp);
	spaceShip_->setInitialPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setScale(Vector(0.0115476f, 0.0110476f, 0.0115476f));
	spaceShip_->setRotation(0.0f, 0.0f, XM_PI/2);
	spaceShip_->setFloorHeight(terrainHeight_);

	//soundManager_->playFile("background_music", true);

	if(!setupDebugOptions())
	{
		return false;
	}
	
	// Setup initial game state
	levelState_ = INTRODUCTION;
	introLevelState_ = ARRIBING;

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	// Set up game clock and debug
	introClock_ = new ClockClass();
	if(!introClock_)
	{
		return false;
	}
	introClock_->reset();

	return true;
}

void ThirdScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	LogClass::Instance()->update();

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				updateIntroduction(elapsedTime);
			}
			break;
		case PLAYING:
			{
				for(int i = 0; i < 5; i++)
				{
					friendSpacheShips_[0]->update(elapsedTime);
				}
			}
			break;
		case ENDING:
			{
				
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Update debug texts
	if(debug_)
	{
		std::stringstream FPSText;
		FPSText << "FPS: " << 1/elapsedTime;
		FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());

		updateKinectHands();
	}
}

void ThirdScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// Draw background
	graphicsManager_->turnZBufferOff();
		background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	graphicsManager_->turnZBufferOn();

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				
			}
			break;
		case PLAYING:
			{
				for(int i = 0; i < 5; i++)
				{
					friendSpacheShips_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}
			}
			break;
		case ENDING:
			{
				
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Draw all debug stuff!
	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_[1]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[0].x, ((screenHeight_/2))-kinectHandViewPos_[0].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[1].x, ((screenHeight_/2))-kinectHandViewPos_[1].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void ThirdScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	if(spaceShip_)
	{
		spaceShip_->destroy();
		delete spaceShip_;
		spaceShip_ = 0;
	}

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}

	// Destroy scenario models
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}
}

void ThirdScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49: //1
			{

			}
			break;
		case 68: //D
		case 100: //d
			{
				debug_ = !debug_;
			}
			break;
		case 32: // Space
			{
				
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
				
			}
			break;
		case RIGHT_BUTTON:
			{

			}
			break;
		default:
			{
				
			}
			break;
	}
}

void ThirdScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	if(arg.type == FIRST_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		kinectHandViewPos_[0] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

		// Calculate world position
		kinectHandWorldPos_[0].x = ((kinectHandViewPos_[0].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[0].y = (1-(kinectHandViewPos_[0].y/screenHeight_))*5;
		kinectHandWorldPos_[0].z = 0;
	}
	
	if(arg.type == SECOND_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		kinectHandViewPos_[1] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

		// Calculate world position
		kinectHandWorldPos_[1].x = ((kinectHandViewPos_[1].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[1].y = (1-(kinectHandViewPos_[1].y/screenHeight_))*5;
		kinectHandWorldPos_[1].z = 0;
	}
}

bool ThirdScreenState::setupDebugOptions()
{
	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 10, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!setupKinectHands())
	{
		return false;
	}

	return true;
}

bool ThirdScreenState::setupKinectHands()
{
	kinectHands_[0] = new TextClass();
	if(!kinectHands_[0])
	{
		return false;
	}
	// Initialize the text object.
	if(!kinectHands_[0]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 30, "Kinect1: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectHands_[1] = new TextClass();
	if(!kinectHands_[1])
	{
		return false;
	}
	// Initialize the text object.
	if(!kinectHands_[1]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 50, "Kinect2: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectHand_ = new ImageClass;
	if(!kinectHand_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!kinectHand_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "hand", 30, 50))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

bool ThirdScreenState::setupSplashScreen()
{
	// load splashscreen and calculate its position
	splashScreen_ = new ImageClass;
	if(!splashScreen_)
	{
		MessageBoxA(NULL, "Could not initialize the splashscreen image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!splashScreen_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "splashscreen3", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the splashscreen image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

void ThirdScreenState::updateIntroduction(float elapsedTime)
{
	introClock_->tick();


	switch(introLevelState_)
	{
		case ARRIBING:
			{
				
			}
			break;
		case LANDING:
			{
				
			}
			break;
		case TRANSITION:
			{
				
			}
			break;
		default:
			{

			}
			break;
	}
}

void ThirdScreenState::updateKinectHands()
{
	std::stringstream kinectext1;
	kinectext1 << "Kinect 1: " << kinectHandWorldPos_[0].x << "x" << kinectHandWorldPos_[0].y;
	kinectHands_[0]->setText(kinectext1.str(), graphicsManager_->getDeviceContext());

	std::stringstream kinectext2;
	kinectext2 << "Kinect 2: " << kinectHandWorldPos_[1].x << "x" << kinectHandWorldPos_[1].y;
	kinectHands_[1]->setText(kinectext2.str(), graphicsManager_->getDeviceContext());
}

void ThirdScreenState::drawSplashScreen()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	graphicsManager_->beginDraw(1.0f, 0.5f, 0.0f, 1.0f);
		graphicsManager_->turnZBufferOff();
			splashScreen_->draw(graphicsManager_->getDeviceContext(), (screenWidth_/2)*-1, (screenHeight_/2)+2, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager_->turnZBufferOn();
	graphicsManager_->endDraw();
}

void ThirdScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level3/main_configuration.xml";

	//Loading animations XML file
	pugi::xml_document configurationDoc;
	if (!configurationDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load configuration .xml file!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = configurationDoc.child("configuration")))
	{
		MessageBoxA(NULL, "Could not load configuration node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	// Time Pico greeting
	/*pugi::xml_node greetingNode;
	if(!(greetingNode = rootNode.child("greeting_time")))
	{
		MessageBoxA(NULL, "Could not load greeting time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeGreeting_ = 5;
	}
	else
	{
		timeGreeting_ = greetingNode.text().as_float();
	}*/
}

bool ThirdScreenState::loadScenario(std::string scenario)
{
	// load background and calculate its position
	background_ = new ImageClass;
	if(!background_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "planet_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

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

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("suelociudad") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y+0.1f;
		}
	}

	return true;
}

void ThirdScreenState::createScenarioObject(std::string scenario, std::string xmlName)
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

void ThirdScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}
