#ifndef _STATIC_MODEL_CLASS_H_
#define _STATIC_MODEL_CLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include <Windows.h>

#include "modelClass.h"

#include <iostream>
#include <fstream>
#include <sstream>

class StaticModelClass : public ModelClass
{
	public:
		StaticModelClass();
		StaticModelClass(const StaticModelClass&);
		virtual ~StaticModelClass();

		virtual bool setup(ID3D11Device*, std::string);
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*);

	private:
		virtual bool setupBuffers(ID3D11Device*);
};

#endif //_STATIC_MODEL_CLASS_H_
