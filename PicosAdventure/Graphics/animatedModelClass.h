#ifndef _ANIMATED_MODEL_CLASS_H_
#define _ANIMATED_MODEL_CLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include <Windows.h>

#include "modelClass.h"

#include "../Utils/pugiconfig.h"
#include "../Utils/pugixml.h"

#include <iostream>
#include <fstream>
#include <sstream>

class AnimatedModelClass : public ModelClass
{
	public:
		AnimatedModelClass();
		AnimatedModelClass(const AnimatedModelClass&);
		virtual ~AnimatedModelClass();

		virtual bool setup(ID3D11Device*, std::string);
		void update(float elapsedTime);
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*);

		void decreaseAnimationToDisplay();
		void increaseAnimationToDisplay();

		void setAnimationToExecute(std::string name, float interpolationTime);
		void setAnimationToPlay(std::string name, float interpolationTime);

	private:
		virtual bool setupBuffers(ID3D11Device*);
		bool updateBuffers(ID3D11DeviceContext*);

		bool parseModelAnimations(const std::string& strFilename);

		// Animations
		std::map<std::string, int> animations_;
		std::string animationToPlay_;
		std::string initialAnimationToPlay_;
};

#endif //_ANIMATED_MODEL_CLASS_H_
