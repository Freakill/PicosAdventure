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

	debug_ = false;
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
	loadScenario("scenario1");

	float terrainHeight = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("terreno") != std::string::npos)
		{
			terrainHeight = (*it)->getPosition().y-0.1f;
		}
	}

	fruitTest_ = new FruitClass();
	fruitTest_->setup(graphicsManager_, "arandanos", Point(3.0f, 1.0f, -1.0f), terrainHeight, Vector(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);

	inputManager->addListener(*this);

	return true;
}

void FirstScreenState::update(float elapsedTime)
{	
	fruitTest_->update(elapsedTime);
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
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		(*it)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	fruitTest_->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
}

void FirstScreenState::destroy()
{
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		(*it)->destroy();
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
		case LEFT_BUTTON:
			{
				if(fruitTest_->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
				{
					fruitTest_->makeItFall();
				}
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void FirstScreenState::loadScenario(std::string scenario)
{
	HANDLE dir;
    WIN32_FIND_DATAA file_data;

	std::string scenarioToLoad = "./Data/scenario/"+scenario+"/*";

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

			createScenarioObject(s);

		} while (FindNextFileA(dir, &file_data));
	}
}

void FirstScreenState::createScenarioObject(std::string xmlName)
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

