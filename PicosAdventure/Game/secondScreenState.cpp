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

	spaceShip_ = 0;
	pico_ = 0;

	background_ = 0;

	for(int i = 0; i < 3; i++)
	{
		initialPositions_[i].x = 0.0f;
		initialPositions_[i].y = 0.0f;
		initialPositions_[i].z = 0.0f;

		finalPositions_[i].x = 0.0f;
		finalPositions_[i].y = 0.0f;
		finalPositions_[i].z = 0.0f;
	}
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

	LogClass::Instance()->setup("second_level_log");

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
	light_->setAmbientColor(0.6f, 0.6f, 0.6f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light_->setDirection(0.0f, -0.5f, 1.0f);

	// Create the light object.
	piecesLight_ = new LightClass;
	if(!piecesLight_)
	{
		return false;
	}

	// Initialize the light object.
	piecesLight_->setAmbientColor(0.2f, 0.2f, 0.3f, 1.0f);
	piecesLight_->setDiffuseColor(0.2f, 0.2f, 0.3f, 1.0f);
	piecesLight_->setDirection(0.0f, -1.0f, 1.0f);

	// SOUND
	soundManager_ = new SoundSecondClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler()))
	{
		MessageBox(NULL, L"Could not initialize Direct Sound.", L"SecondScreen - Error", MB_OK);
		return false;
	}

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

	background2_ = new ImageClass;
	if(!background2_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background2_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "sky_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

	// Lights
	lightPos_[0] = XMFLOAT4(-3.5f, 2.0f, -4.0f, 0.0f);
	lightPos_[1] = XMFLOAT4(30.5f, 20.0f, -40.0f, 0.0f);

	lightBody_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, "sphere");
	lightBody_->setPosition(Point(-4.0f, 0.0f, -3.25f));
	lightBody_->setScale(Vector(0.02f, 0.02f, 0.02f));

	lightParticles_[0] = new ParticleSystem;
	if(!lightParticles_[0])
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[0] && !lightParticles_[0]->setup(graphicsManager, "star", Point(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z) , 2.8, 30, 60, XMFLOAT4(1.00f, 1.00f, 0.0f, 1.0f)))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	lightParticles_[1] = new ParticleSystem;
	if(!lightParticles_[1])
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[1] && !lightParticles_[1]->setup(graphicsManager, "star", Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z) , 2.8, 30, 60, XMFLOAT4(1.00f, 1.00f, 0.0f, 1.0f)))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	// Load the first level scenario
	loadScenario("level2");

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("terreno") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y+0.1f;
		}
	}
	
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
	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1]);
	spaceShip_->setInitialPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setScale(Vector(0.0115476f, 0.0110476f, 0.0115476f));
	spaceShip_->setRotation(0.0f, 0.0f, XM_PI/2);
	spaceShip_->setFloorHeight(terrainHeight_);

	spaceShipLevel_ = 1;

	// Create Pico
	pico_ = new PicoSecondClass();
	if(!pico_)
	{
		return false;
	}
	if(!pico_->setup(graphicsManager_, camera_, soundManager_))
	{
		MessageBoxA(NULL, "Could not initialize Pico :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	pico_->setSpaceShipPosition(spaceShip_->getPosition());
	inputManager->addListener(*pico_);

	// Create the light object for Pico.
	picoLight_ = new LightClass;
	if(!picoLight_)
	{
		return false;
	}

	// Initialize the light object for Pico.
	picoLight_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	picoLight_->setDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
	picoLight_->setDirection(0.0f, -0.5f, 1.0f);

	// Load spaceship pieces to collect
	loadPieces();

	// Stars
	loadStars();
	timeToWaitFirstStar_ = 8.0f;

	starLevel_ = 1;
	starsFalling_ = false;
	starFallTime_ = 8.0f;
	betweenStarsTime_ = 10.0f;

	initialPositions_[0].x = -4.75f;
	initialPositions_[0].y = 8.2f;
	initialPositions_[0].z = -3.0f;

	initialPositions_[1].x = 0.0f;
	initialPositions_[1].y = 8.2f;
	initialPositions_[1].z = -3.0f;

	initialPositions_[2].x = 4.75f;
	initialPositions_[2].y = 8.2f;
	initialPositions_[2].z = -3.0f;

	finalPositions_[0].x = -3.5f;
	finalPositions_[0].y = terrainHeight_;
	finalPositions_[0].z = -3.5f;

	finalPositions_[1].x = 0.0f;
	finalPositions_[1].y = terrainHeight_;
	finalPositions_[1].z = -3.5f;

	finalPositions_[2].x = 4.0f;
	finalPositions_[2].y = terrainHeight_;
	finalPositions_[2].z = -3.5f;

	starTouched_ = false;

	starTouchedClock_ = new ClockClass();
	if(!starTouchedClock_)
	{
		return false;
	}
	starTouchedClock_->reset();

	// Set the level state to the introduction, and it to greetings
	levelState_ = INTRODUCTION;
	introLevelState_ = TO_NIGHT;
	starsIntroLevelState_ = FIRST_STAR;
	// Setup Pico at the initial state
	hasPicoGreeted_ = false;
	
	loadConfigurationFromXML();

	soundManager_->playFile("background_music", true);

	// Set up game clock and debug
	introClock_ = new ClockClass();
	if(!introClock_)
	{
		return false;
	}
	introClock_->reset();

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

	kinectHands_ = new TextClass();
	if(!kinectHands_)
	{
		return false;
	}

	// Initialize the text object.
	if(!kinectHands_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 60, "Kinect: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	lightPositions_ = new TextClass();
	if(!lightPositions_)
	{
		return false;
	}

	// Initialize the text object.
	if(!lightPositions_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 80, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the lights text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	inputManager->addListener(*this);

	// Change kinect settings to make user being drawn darker
	kinectClass_ = kinectManager;

	kinectClass_->setUserColor(XMFLOAT4(0.25f, 0.25f, 0.35f, 0.5f));

	return true;
}

void SecondScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	LogClass::Instance()->update();

	// Update light positions
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>((*objectsIt)->getShader3D());
		pointlightShader->setPositions(lightPos_[0], lightPos_[1]);
	}
	
	pico_->update(elapsedTime);

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
				pointlightShader->setPositions(lightPos_[0], lightPos_[1]);

				if(introLevelState_ != TO_NIGHT)
				{
					updatePieces(elapsedTime);
				}

				updateIntroduction(elapsedTime);
			}
			break;
		case MOUNTING:
			{
				updateLightPositions();

				PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
				pointlightShader->setPositions(lightPos_[0], lightPos_[1]);

				lightParticles_[0]->update(elapsedTime*1000, true);
				lightParticles_[1]->update(elapsedTime*1000, true);

				updatePieces(elapsedTime);

				// Check if the spaceship has been mounted
				bool allPiecesInPlace = true;
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					if(!(*pieceIt)->getInFinalPosition())
					{
						allPiecesInPlace = false;
					}
				}

				// When all pieces are in place and pico is done
				if(allPiecesInPlace && pico_->isPicoWaiting())
				{
					// Make Pico celebrate it!
					pico_->makeRest(false);
					pico_->makeDanceAss();
					soundManager_->playFile("spaceship_finished", false);

					LogClass::Instance()->addEntry("SPACESHIP_FINNISHED", 0, 0);

					Shader3DClass *diffuseShader3DTemp = Shader3DFactory::Instance()->CreateShader3D("DiffuseShader3D", graphicsManager_);
					spaceShip_->getObject()->setShader3D(diffuseShader3DTemp);

					levelState_ = TRANSITION;
					gameClock_->reset();
				}
			}
			break;
		case TRANSITION:
			{
				updateLightPositions();

				if(gameClock_->getTime() > timeToWaitFirstStar_)
				{
					makeFirstStarFall();
					pico_->makePointing();

					levelState_ = INTRO_COLLECTING;
				}
			}
			break;
		case INTRO_COLLECTING:
			{
				updateLightPositions();

				stars_[0]->update(elapsedTime);
				spaceShip_->update(elapsedTime);

				updateIntroCollecting(elapsedTime);
			}
			break;
		case COLLECTING:
			{
				updateLightPositions();
				starTouchedClock_->tick();
				spaceShip_->update(elapsedTime);

				for(int i = 0; i < 3; i++)
				{
					stars_[i]->update(elapsedTime);
				}

				if(!starsFalling_)
				{
					if(gameClock_->getTime() > starFallTime_)
					{
						origins_.clear();
						endings_.clear();

						LogClass::Instance()->addEntry("MAKE_STARS_FALL", starLevel_, 0);

						for(int i = 0; i < starLevel_; i++)
						{
							makeStarFall(i);
						}

						gameClock_->reset();
					}
				}
				else
				{
					if(gameClock_->getTime() > betweenStarsTime_)
					{
						
						gameClock_->reset();
					}
				}

				if(starTouched_ && starTouchedClock_->getTime() > 8.0f)
				{
					spaceShip_->makeLaunch(spaceShipLevel_);
					pico_->makeCelebrate();
					spaceShipLevel_++;
					starLevel_++;

					if(starLevel_ > 3)
					{
						starLevel_ = 3;
					}

					starTouched_ = false;
					starTouchedClock_->reset();

					if(spaceShipLevel_ > 5)
					{
						spaceShip_->makeBig(Point(-4.5f, terrainHeight_+0.5f, -3.25f), Vector(0.03f, 0.03f, 0.03f));
						levelState_ = ENDING;
					}
				}
			}
			break;
		case ENDING:
			{
				spaceShip_->update(elapsedTime);

				if(spaceShip_->isPrepared())
				{
					pico_->goToPosition(Point(spaceShip_->getPosition().x-1.0f, terrainHeight_, spaceShip_->getPosition().z));
					kinectClass_->setUserColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
				}
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

	std::stringstream kinectext;
	kinectext << "Kinect: " << kinectHandWorldPos_[0].x << "x" << kinectHandWorldPos_[0].y << " " << kinectHandWorldPos_[1].x << "x" << kinectHandWorldPos_[1].y;
	kinectHands_->setText(kinectext.str(), graphicsManager_->getDeviceContext());

	std::stringstream lightstext;
	lightstext << "Light: " << lightPos_[0].x << "x" << lightPos_[0].y << " " << lightPos_[1].x << "x" << lightPos_[1].y;
	lightPositions_->setText(lightstext.str(), graphicsManager_->getDeviceContext());
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

	if(introLevelState_ == TO_NIGHT)
	{
		graphicsManager_->turnOnAlphaBlending();
		graphicsManager_->turnZBufferOff();
			background2_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, light_->getAmbientColor());
		graphicsManager_->turnZBufferOn();
		graphicsManager_->turnOffAlphaBlending();
	}
	else
	{
		graphicsManager_->turnZBufferOff();
			background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager_->turnZBufferOn();
	}

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
		case MOUNTING:
			{
				graphicsManager_->turnOnWireframeRasterizer();
					//spaceShipObject_->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
					spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
				graphicsManager_->turnOnSolidRasterizer();

				// Draw the loaded pieces
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					(*pieceIt)->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, piecesLight_, debug_);
				}

				if(introLevelState_ == GIVE_POWER)
				{
					lightBody_->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
				}

				// Draw light Particles
				graphicsManager_->turnOnParticlesAlphaBlending();
				graphicsManager_->turnZBufferOff();
					lightParticles_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
					lightParticles_[1]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
				graphicsManager_->turnZBufferOn();
				graphicsManager_->turnOffAlphaBlending();
			}
			break;
		case TRANSITION:
		case INTRO_COLLECTING:
		case COLLECTING:
			{
				for(int i = 0; i < starLevel_; i++)
				{
					stars_[i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
				}

				spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
			}
			break;
		case ENDING:
			{
				spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Draw pico
	pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);

	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			lightPositions_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void SecondScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	if(spaceShip_)
	{
		spaceShip_->destroy();
		delete spaceShip_;
		spaceShip_ = 0;
	}

	if(lightBody_)
	{
		lightBody_->destroy();
		delete lightBody_;
		lightBody_ = 0;
	}

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}

	for(int i = 0; i < 3; i++)
	{
		if(stars_[i])
		{
			stars_[i]->destroy();
			delete stars_[i];
			stars_[i] = 0;
		}
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

void SecondScreenState::updateIntroduction(float elapsedTime)
{
	introClock_->tick();

	if(introLevelState_ != GIVE_POWER)
	{
		lightPos_[0].x = pico_->getPosition().x;
		lightPos_[0].y = pico_->getPosition().y+1.5;
		lightPos_[0].z = pico_->getPosition().z-1.0f;
	}

	switch(introLevelState_)
	{
		case TO_NIGHT:
			{
				float lightToNight = 0.6f-0.55f*(introClock_->getTime()/timeToNight_);
				light_->setAmbientColor(lightToNight, lightToNight, lightToNight, 1.0f);

				pico_->makeRest(false);

				// When greeting time has passed we make Pico go next to the piece with a little margin
				if(introClock_->getTime() > timeToNight_)
				{
					light_->setAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);

					if(!hasPicoGreeted_)
					{
						pico_->makeGreeting();
						hasPicoGreeted_ = true;
					}

					introLevelState_ = GREETING;
					introClock_->reset();
				}
			}
			break;
		case GREETING:
			{
				// When greeting time has passed we make Pico go next to the piece with a little margin
				if(introClock_->getTime() > timeGreeting_)
				{
					Point positionFirstPiece = pieces_.at(0)->getPosition();
					positionFirstPiece.x += 0.7f;
					pico_->goToPosition(positionFirstPiece);

					introLevelState_ = TO_PIECE;
				}
			}
			break;
		case TO_PIECE:
			{
				// Check whether Pico arrived or not to the piece
				if(pico_->checkPicoArrivedObjective())
				{
					// When arrive we make him surprised and light the piece
					pico_->makeRest(false);
					pico_->changeExpression("sorpresa");
					soundManager_->playSurprise();
					introLevelState_ = LIGHT_IT;
				}
			}
			break;
		case LIGHT_IT:
			{
				pieces_.at(0)->lightIt();
				pieces_.at(0)->update(elapsedTime);

				// When lighted we make Pico drag it until the spaceship
				if(pieces_.at(0)->hasFallen())
				{
					Point positionSpaceship = spaceShip_->getPosition(); //spaceShipObject_->getPosition();
					positionSpaceship.x += 1.2f;
					pico_->goToPosition(positionSpaceship);
					introLevelState_ = DRAG_IT;
				}
			}
			break;
		case DRAG_IT:
			{
				// Put the piece in front of Pico to simulate that he is dragging it
				Point newPiecePosition = pico_->getPosition();
				newPiecePosition.x -= 0.7f;
				pieces_.at(0)->setPosition(newPiecePosition);

				// When arrived to the SpaceShip, place piece on final position and make Pico work
				if(pico_->checkPicoArrivedObjective())
				{
					pieces_.at(0)->setFinalPosition();
					pico_->makeWork();
					introLevelState_ = WORK;
				}
			}
			break;
		case WORK:
			{
				if(pico_->isPicoWaiting())
				{
					// Not until we want users to start playing with the pieces we activate the kinect notifier
					kinectClass_->addListener(*this);
					introLevelState_ = GIVE_POWER;
				}
			}
			break;
		case GIVE_POWER:
			{
				if(powerToUser(elapsedTime))
				{
					introLevelState_ = POWER_GIVEN;
					levelState_ = MOUNTING;
					kinectClass_->setUserColor(XMFLOAT4(0.75f, 0.75f, 0.85f, 0.5f));
					pico_->makeCelebrate();
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void SecondScreenState::updateIntroCollecting(float elapsedTime)
{
	switch(starsIntroLevelState_)
	{
		case FIRST_STAR:
			{
				if(stars_[0]->isInTheFloor() || stars_[0]->isInTheSky())
				{
					//make the spaceship simulate some kind of reaction
					spaceShip_->makeLaunch(1);

					starsIntroLevelState_ = FIRST_REACTION;
				}
			}
			break;
		case FIRST_REACTION:
			{
				if(stars_[0]->isInTheFloor() || stars_[0]->isInTheSky())
				{
					pico_->makeRest(false);
					pico_->makeCelebrate();

					starsIntroLevelState_ = PICO_GUIDING;
				}
			}
			break;
		case PICO_GUIDING:
			{
				levelState_ = COLLECTING;
				gameClock_->reset();
			}
			break;
		default:
			{

			}
			break;
	}
}

void SecondScreenState::updatePieces(float elapsedTime)
{
	// Update pieces
	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		(*pieceIt)->update(elapsedTime);
	}
}

void SecondScreenState::updateLightPositions()
{
	lightPos_[0].x = kinectHandWorldPos_[0].x;
	lightPos_[0].y = kinectHandWorldPos_[0].y;

	lightPos_[1].x = kinectHandWorldPos_[1].x;
	lightPos_[1].y = kinectHandWorldPos_[1].y;

	lightParticles_[0]->setPosition(Point(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z));
	lightParticles_[1]->setPosition(Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z));
}

void SecondScreenState::makeFirstStarFall()
{
	stars_[0]->setInitialPosition(Point(5.75f, 8.2f, -1.0f));
	stars_[0]->setFinalPosition(spaceShip_->getPosition());
	stars_[0]->makeItFall(true);
}

void SecondScreenState::makeStarFall(int i)
{
	int init = rand() % 3;
	bool keepSearching = true;
	while(keepSearching)
	{
		bool notIn = true;
		std::vector<int>::iterator originIt;
		for(originIt = origins_.begin(); originIt != origins_.end(); originIt++)
		{
			if((*originIt) == init)
			{
				notIn = false;
			}
		}

		if(notIn)
		{
			keepSearching = false;
			origins_.push_back(init);
		}
		else
		{
			init = rand() % 3;
		}
	}
	stars_[i]->setInitialPosition(initialPositions_[init]);

	int end = rand() % 3;
	keepSearching = true;
	while(keepSearching)
	{
		bool notIn = true;
		std::vector<int>::iterator endIt;
		for(endIt = endings_.begin(); endIt != endings_.end(); endIt++)
		{
			if((*endIt) == end)
			{
				notIn = false;
			}
		}

		if(notIn)
		{
			keepSearching = false;
			endings_.push_back(end);
		}
		else
		{
			end = rand() % 3;
		}
	}
	stars_[i]->setFinalPosition(finalPositions_[end]);

	if(i == 0)
	{
		stars_[i]->makeItFall(true);
	}
	else
	{
		stars_[i]->makeItFall(false);
	}
}

void SecondScreenState::makeShipIgnite(int level)
{

}

void SecondScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 8, 0);
				pieces_.at(8)->makeItFall();
			}
			break;
		case 50:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 4, 0);
				pieces_.at(4)->makeItFall();
			}
			break;
		case 51:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 2, 0);
				pieces_.at(2)->makeItFall();
			}
			break;
		case 52:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 7, 0);
				pieces_.at(7)->makeItFall();
			}
			break;
		case 53:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 6, 0);
				pieces_.at(6)->makeItFall();
			}
			break;
		case 54:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 3, 0);
				pieces_.at(3)->makeItFall();
			}
			break;
		case 55:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 9, 0);
				pieces_.at(9)->makeItFall();
			}
			break;
		case 56:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 1, 0);
				pieces_.at(1)->makeItFall();
			}
			break;
		case 57:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 5, 0);
				pieces_.at(5)->makeItFall();
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
				switch(levelState_ )
				{
					case MOUNTING:
						{
							LogClass::Instance()->addEntry("KEYBOARD_SPACESHIP_MOUNTED", 1, 0);

							std::vector<PieceClass*>::iterator pieceIt;
							for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
							{
								(*pieceIt)->setFinalPosition();
							}
						}
						break;
					case COLLECTING:
						{
							LogClass::Instance()->addEntry("KEYBOARD_GOOD_STAR_CATCHED", 1, 0);

							stars_[0]->reset();
							starTouched_ = true;
						}
						break;
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
		// Check if the left mouse is pressed to interested objects
		case LEFT_BUTTON:
			{
				//if(levelState_ != IN
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
	if(arg.type == FIRST_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		kinectHandViewPos_[0] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

		// Calculate world position
		kinectHandWorldPos_[0].x = ((kinectHandViewPos_[0].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[0].y = (1-(kinectHandViewPos_[0].y/screenHeight_))*5;
		kinectHandWorldPos_[0].z = 0;

		if(levelState_ == MOUNTING)
		{
			std::vector<PieceClass*>::iterator pieceIt;
			for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
			{
				if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y))
				{
					(*pieceIt)->lightIt();
				}
			}
		}

		if(levelState_ == COLLECTING)
		{
			for(int i = 0; i < starLevel_; i++)
			{
				if(stars_[i]->isFalling())
				{
					if(stars_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y))
					{
						if(stars_[i]->isGood())
						{
							LogClass::Instance()->addEntry("GOOD_STAR_CATCHED", i+1, 0);
							stars_[i]->reset();
							starTouched_ = true;
						}
						else
						{
							LogClass::Instance()->addEntry("BAD_STAR_CATCHED", i+1, 0);
							soundManager_->playSad();
							pico_->makeNo();
						}
					}
				}
			}
		}
	}
	
	if(arg.type == SECOND_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		kinectHandViewPos_[1] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

		// Calculate world position
		kinectHandWorldPos_[1].x = ((kinectHandViewPos_[1].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[1].y = (1-(kinectHandViewPos_[1].y/screenHeight_))*5;
		kinectHandWorldPos_[1].z = 0;

		if(levelState_ == MOUNTING)
		{
			std::vector<PieceClass*>::iterator pieceIt;
			for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
			{
				if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[1].x, kinectHandViewPos_[1].y))
				{
					(*pieceIt)->lightIt();
				}
			}
		}
	}

	if(levelState_ == COLLECTING && arg.type == HOLD_HANDS)
	{
		kinectHoldViewPos_ = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);

		// Calculate world position
		kinectHoldWorldPos_.x = ((kinectHoldViewPos_.x/screenWidth_)*8)-4;
		kinectHoldWorldPos_.y = (1-(kinectHoldViewPos_.y/screenHeight_))*5;
		kinectHoldWorldPos_.z = 0;

		for(int i = 0; i < starLevel_; i++)
		{
			if(stars_[i]->isFalling())
			{
				if(stars_[i]->getCollisionSphere()->testIntersection(camera_, kinectHoldViewPos_.x, kinectHoldViewPos_.y))
				{
					if(stars_[i]->isGood())
					{
						LogClass::Instance()->addEntry("GOOD_STAR_CATCHED", i+1, 0);
						stars_[i]->reset();
						starTouched_ = true;
					}
					else
					{
						LogClass::Instance()->addEntry("BAD_STAR_CATCHED", i+1, 0);
						soundManager_->playSad();
						pico_->makeNo();
					}
				}
			}
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
		MessageBoxA(NULL, "Could not load configuration .xml file!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = configurationDoc.child("configuration")))
	{
		MessageBoxA(NULL, "Could not load configuration node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	// Time Pico greeting
	pugi::xml_node greetingNode;
	if(!(greetingNode = rootNode.child("greeting_time")))
	{
		MessageBoxA(NULL, "Could not load greeting time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeGreeting_ = 5;
	}
	else
	{
		timeGreeting_ = greetingNode.text().as_float();
	}

	// Time night fading
	pugi::xml_node fadingNode;
	if(!(fadingNode = rootNode.child("fading_time")))
	{
		MessageBoxA(NULL, "Could not load fading time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeToNight_ = 5;
	}
	else
	{
		timeToNight_ = fadingNode.text().as_float();
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

	int pieces = 0;
	for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
	{
		std::string node_name = fruitNode.name();

		if(node_name ==  "piece")
		{
			pugi::xml_node modelNode;
			modelNode = fruitNode.child("model");

			pugi::xml_text modelName = modelNode.text();

			// Parse transformation data
			pugi::xml_node finalPositionNode;
			finalPositionNode = fruitNode.child("final_position");
			Point finalPos = Point(finalPositionNode.attribute("x").as_float(), finalPositionNode.attribute("y").as_float(), finalPositionNode.attribute("z").as_float());

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

			if(!piece->setup(graphicsManager_, soundManager_, modelName.as_string(), finalPos, pos, terrainHeight_, scale, rotX, rotY, rotZ, pieces))
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

			piece->addListener(*pico_);

			pieces_.push_back(piece);
			pieces++;
		}
	}

	return true;
}

bool SecondScreenState::loadStars()
{
	for(int i = 0; i < 3; i++)
	{
		stars_[i] = new StarClass;
		if(!stars_[i])
		{
			MessageBoxA(NULL, "Could not create star instance.", "SecondScreen - Error", MB_ICONERROR | MB_OK);
			return false;
		}
		if(!stars_[i]->setup(graphicsManager_, soundManager_, terrainHeight_))
		{
			MessageBoxA(NULL, "Could not create star object.", "SecondScreen - Error", MB_ICONERROR | MB_OK);
			return false;
		}
	}

	return true;
}

bool SecondScreenState::powerToUser(float elapsedTime)
{
	Vector velocity;
	velocity.x = kinectHandWorldPos_[0].x - lightPos_[0].x;
	velocity.y = kinectHandWorldPos_[0].y - lightPos_[0].y;
	velocity.z = kinectHandWorldPos_[0].z - lightPos_[0].z;

	Vector normalizedVelocity = velocity.normalize();
	velocity = normalizedVelocity * 4.0f;

	lightPos_[0].x += velocity.x*elapsedTime;
	lightPos_[0].y += velocity.y*elapsedTime;

	// Update light body model position
	lightBody_->setPosition(Point(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z));

	// Add particles effect
	lightParticles_[0]->setPosition(Point(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z));
	lightParticles_[0]->update(elapsedTime*1000, true);

	if(lightPos_[0].x < kinectHandWorldPos_[0].x+0.15 && lightPos_[0].x > kinectHandWorldPos_[0].x-0.15 && lightPos_[0].y < kinectHandWorldPos_[0].y+0.15 && lightPos_[0].y > kinectHandWorldPos_[0].y-0.15)
	{
		return true;
	}

	return false;
}

void SecondScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}
