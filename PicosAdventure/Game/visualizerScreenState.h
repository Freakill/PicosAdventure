#ifndef _VISUALIZER_H_
#define _VISUALIZER_H_

#include "../Application/applicationState.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

#include "../Engine/Object3DFactory.h"

#include <mmsystem.h>

class Visualizer: public ApplicationState, public Listener<GUIButton, ButtonStruct>
{
	public:
		Visualizer();
		virtual ~Visualizer();
		static Visualizer* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(GUIButton* notifier, ButtonStruct arg);

	private:
		static Visualizer visualizerState_; //singleton

		void createModel(const std::string &modelName);
		void createXMLModel(const std::string &xmlName);
		bool checkModelHasAnimations(const std::string &modelName);

		void createLoadModelMenu();
		void createLoadXMLMenu();
		void createLoadedObjectButton();

		void playAnimations();
		void stopAnimations();
		void increaseAnimations();
		void decreaseAnimations();

		void moveSelectedObjects(InputStruct arg);
		void rotateSelectedObjects(InputStruct arg);
		void resizeSelectedObjects(InputStruct arg);

		void saveSelectedObjects();
		void deleteSelectedObjects();

		GUIManager*				visualizerGUI_;
		GUIFrame*				loadedObjectsMenu_;
		GUIFrame*				loadModelsMenu_;
		GUIFrame*				loadXMLMenu_;

		CameraClass*			camera_;
		LightClass*				light_;

		std::vector<Object3D*>	loadedObjects_;
		int						lastLoadedObject_;

		bool					playingAnimations_;

		bool					drawMenus_;
};

#endif //_VISUALIZER_H_