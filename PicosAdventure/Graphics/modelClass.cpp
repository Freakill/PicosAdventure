#include "modelClass.h"

ModelClass::ModelClass()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
	cal3dCoreModel_ = 0;
	cal3dModel_ = 0;
	//animationToPlay_ = "";
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

void ModelClass::destroy()
{
	// Release the vertex and index buffers.
	destroyBuffers();

	// Release the model
	destroyModel();

	return;
}

int ModelClass::getIndexCount()
{
	return indexCount_;
}

void ModelClass::drawBuffers(ID3D11DeviceContext* device_context)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(TexturedVertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device_context->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device_context->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ModelClass::destroyBuffers()
{
	// Release the index buffer.
	if(indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = 0;
	}

	// Release the vertex buffer.
	if(vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = 0;
	}

	return;
}

void ModelClass::destroyModel()
{
	if(cal3dModel_)
	{
		delete cal3dModel_;
		cal3dModel_ = 0;
	}

	if(cal3dCoreModel_)
	{
		delete cal3dCoreModel_;
		cal3dCoreModel_ = 0;
	}

	return;
}

bool ModelClass::parseModelConfiguration(std::string strFilename)
{
	//We create the model Loader
	CalLoader::setLoadingMode( LOADER_ROTATE_X_AXIS );
	//We set the basic root for getting the models
	std::string root = "./Data/models/" + strFilename + "/";

	//Loading the skeleton
	bool is_ok = cal3dCoreModel_->loadCoreSkeleton( root + strFilename + ".csf" );
	if(!is_ok)
	{
		MessageBoxA(NULL, "Could not load the skeleton!", "ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	//Loading the mesh
	modelMeshID = cal3dCoreModel_->loadCoreMesh( root + strFilename + ".cmf" );
	if(modelMeshID < 0)
	{
		MessageBoxA(NULL, "Could not load the mesh!", "ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	//Set up the mesh
	cal3dModel_ = new CalModel(cal3dCoreModel_);
	if(!cal3dModel_->attachMesh(modelMeshID))
	{
		MessageBox(NULL, L"Could not attach a Mesh to the model", L"ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	cal3dModel_->update(0.0f);

	return true;
}
