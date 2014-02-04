#ifndef _MODEL_CLASS_H_
#define _MODEL_CLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include <Windows.h>

#include "cal3d/cal3d.h"

#include "graphicsManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

class ModelClass
{
	public:
		ModelClass();
		ModelClass(const ModelClass&);
		virtual ~ModelClass();

		virtual bool setup(ID3D11Device*, std::string) = 0;
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*) = 0;
		virtual void destroy();

		int getIndexCount();

	protected:
		virtual bool setupBuffers(ID3D11Device*) = 0;
		virtual void drawBuffers(ID3D11DeviceContext*);
		virtual void destroyBuffers();

		virtual void destroyModel();

		virtual bool parseModelConfiguration(std::string strFilename);

		ID3D11Buffer *vertexBuffer_, *indexBuffer_;
		int vertexCount_, indexCount_;

		// Cal3D Model
		CalCoreModel* cal3dCoreModel_;
		CalModel* cal3dModel_;

		// Meshes
		int modelMeshID;
};

#endif
