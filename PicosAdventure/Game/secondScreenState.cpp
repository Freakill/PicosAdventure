#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "secondScreenState.h"

SecondScreenState SecondScreenState::secondScreenState_;

SecondScreenState::SecondScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	spaceShipWireframe_ = 0;
	pico_ = 0;

	background_ = 0;
}

SecondScreenState::~SecondScreenState()
{

}

SecondScreenState* SecondScreenState::Instance()
{
	return (&secondScreenState_);
}

bool SecondScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

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
	light_->setAmbientColor(0.0f, 0.05f, 0.15f, 1.0f);
	light_->setDiffuseColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDirection(0.0f, -1.0f, 1.0f);

	// load background and calculate its position
	background_ = new ImageClass;
	if(!background_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "night_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

	// Lights
	lightPos_[0] = XMFLOAT4(-3.5f, 2.0f, -4.0f, 0.0f);
	lightPos_[1] = XMFLOAT4(3.5f, 2.0f, -4.0f, 0.0f);

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

	// Load spaceship
	spaceShipWireframe_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, "nave");
	spaceShipWireframe_->setPosition(Point(-4.0f, 0.0f, -3.25f));
	spaceShipWireframe_->setScale(Vector(0.0110476f, 0.0110476f, 0.0110476f));
	spaceShipWireframe_->setRotationX(0);
	spaceShipWireframe_->setRotationY(0);
	spaceShipWireframe_->setRotationZ(0);
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	spaceShipWireframe_->setShader3D(shaderTemp);

	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShipWireframe_->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1]);

	// Load spaceship pieces to collect
	loadPieces();

	// Create Pico
	pico_ = new PicoSecondClass();
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

	// Set the level state to the first iteration and load fruits accordingly
	levelState_ = INTRODUCTION;

	introLevelState_ = TO_PIECE;

	loadConfigurationFromXML();

	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 20, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	// kinect debug text object
	kinectHandText_ = new TextClass();
	if(!kinectHandText_)
	{
		return false;
	}

	// Initialize the text object.
	if(!kinectHandText_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 20, "Kinect: "))
	{
		std::string textToDisplay = "Could not initialize the frame text object for Kinect text.";
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	kinectManager->addListener(*this);
	inputManager->addListener(*this);

	return true;
}

void SecondScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	// Update light positions
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>((*objectsIt)->getShader3D());
		pointlightShader->setPositions(lightPos_[0], lightPos_[1]);
	}
	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShipWireframe_->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1]);

	pico_->update(elapsedTime);

	switch(levelState_)
	{
		default:
			{
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					(*pieceIt)->update(elapsedTime);
				}
			}
			break;
	}

	std::stringstream FPSText;
	FPSText << "FPS: " << 1/elapsedTime;
	FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());
}

void SecondScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// Despite not being a drawing method, we call it here to reuse matrices
	updatePicoScreenposition(worldMatrix, viewMatrix, projectionMatrix);

	graphicsManager_->turnZBufferOff();
		background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	graphicsManager_->turnZBufferOn();

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	graphicsManager_->turnOnWireframeRasterizer();
		spaceShipWireframe_->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	graphicsManager_->turnOnSolidRasterizer();

	// Draw the loaded pieces
	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		(*pieceIt)->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	}

	// Draw pico
	pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void SecondScreenState::destroy()
{
	if(spaceShipWireframe_)
	{
		spaceShipWireframe_->destroy();
		delete spaceShipWireframe_;
		spaceShipWireframe_ = 0;
	}

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}

	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}

	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		(*pieceIt)->destroy();
	}
}

void SecondScreenState::updatePicoScreenposition(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	XMFLOAT4 picoPos;
	Point picoPoint = pico_->getPosition();
	picoPos.x = picoPoint.x;
	picoPos.y = picoPoint.y;
	picoPos.z = picoPoint.z;
	picoPos.w = 1.0f;

	XMFLOAT4 resultPos;
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&picoPos), XMLoadFloat4x4(&worldMatrix)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&projectionMatrix)));

	picoScreenPos_.x = resultPos.x;
	picoScreenPos_.y = resultPos.y;
}

void SecondScreenState::notify(InputManager* notifier, InputStruct arg)
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
				lightPos_[0].x = ((arg.mouseInfo.x/screenWidth_)*8)-4;
				lightPos_[0].y = (1-(arg.mouseInfo.y/screenHeight_))*5;

				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
					{
						(*pieceIt)->makeItFall();
					}
				}
			}
			break;
		case RIGHT_BUTTON:
			{
				lightPos_[1].x = ((arg.mouseInfo.x/screenWidth_)*8)-4;
				lightPos_[1].y = (1-(arg.mouseInfo.y/screenHeight_))*5;
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void SecondScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	kinectHandPos_ = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

	std::stringstream kinectext;
	kinectext << "Kinect: " << kinectHandPos_.x << "x" << kinectHandPos_.y;
	kinectHandText_->setText(kinectext.str(), graphicsManager_->getDeviceContext());

	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
		{
			(*pieceIt)->lightIt();
		}
	}
}

void SecondScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level2/main_configuration.xml";

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
}

void SecondScreenState::loadScenario(std::string scenario)
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

void SecondScreenState::createScenarioObject(std::string scenario, std::string xmlName)
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

	// Add pointlight shader
	// Set specific multitexture shader for tips and increment textures array
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	objectLoadedTemp->setShader3D(shaderTemp);

	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(objectLoadedTemp->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1]);

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

bool SecondScreenState::loadPieces()
{
	std::stringstream root;
	root << "./Data/configuration/level2/pieces/pieces.xml";

	//Loading animations XML file
	pugi::xml_document fruitsDoc;
	if(!fruitsDoc.load_file(root.str().c_str()))
	{
		std::stringstream error;
		error << "Could not load object .xml file!" << root.str();
		MessageBoxA(NULL, error.str().c_str(), "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = fruitsDoc.child("pieces")))
	{
		MessageBoxA(NULL, "Could not find the fruits root node.", "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
	{
		std::string node_name = fruitNode.name();

		if(node_name ==  "piece")
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

			PieceClass* piece = new PieceClass();
			if(!piece)
			{
				return false;
			}

			if(!piece->setup(graphicsManager_, modelName.as_string(), pos, terrainHeight_, scale, rotX, rotY, rotZ))
			{
				MessageBoxA(NULL, "Could not initialize fruit.", "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
				return false;
			}				

			pugi::xml_node collisionNode = fruitNode.child("collision");

			positionNode = collisionNode.child("position");
			pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float());

			piece->getCollisionSphere()->setRelativePosition(pos);

			pugi::xml_node radiusNode;
			radiusNode = collisionNode.child("radius");
			float radius = radiusNode.text().as_float();

			piece->getCollisionSphere()->setRadius(radius);

			pieces_.push_back(piece);
		}
	}

	return true;
}

void SecondScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}
