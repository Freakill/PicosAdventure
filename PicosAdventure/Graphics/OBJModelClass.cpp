#include "OBJModelClass.h"

OBJModelClass::OBJModelClass()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
}

OBJModelClass::OBJModelClass(const OBJModelClass& other)
{
}

OBJModelClass::~OBJModelClass()
{
}

bool OBJModelClass::setup(ID3D11Device* device, std::string modelName)
{
	std::ifstream fin;
	char input;
	bool result;

	modelName_ = modelName;

	std::string root = "./Data/models/" + modelName + "/" + modelName + ".obj";

	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(root);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while(!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') { vertexCount++; }
			if(input == 't') { textureCount++; }
			if(input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ') { faceCount++; }
		}
		
		// Otherwise read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	result = parseModelConfiguration(root);
	if(!result)
	{
		MessageBox(NULL, L"Could not parse configuration file.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = setupBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void OBJModelClass::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	drawBuffers(deviceContext);
}

void OBJModelClass::destroy()
{
	// Release the vertex and index buffers.
	destroyBuffers();

	// Release the model
	// Release the four data structures.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}
	if(texcoords)
	{
		delete [] texcoords;
		texcoords = 0;
	}
	if(normals)
	{
		delete [] normals;
		normals = 0;
	}
	if(faces)
	{
		delete [] faces;
		faces = 0;
	}

	return;
}

bool OBJModelClass::setupBuffers(ID3D11Device* device)
{
	TexturedVertexType *verticesArray = 0;
	int *indicesArray = 0;
	D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
	D3D11_SUBRESOURCE_DATA vertex_data, index_data;
	HRESULT result;
	int vIndex, tIndex, nIndex;

	vertexCount_ = faceCount*3;
	indexCount_ = faceCount*3;

	verticesArray = new TexturedVertexType[vertexCount_];
	indicesArray = new int[indexCount_];

	int facesNum = 0;

	for(int i = 0; i < faceCount; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		verticesArray[i*3].position = XMFLOAT3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
		verticesArray[i*3].normal = XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
		verticesArray[i*3].tu = texcoords[tIndex].x;
		verticesArray[i*3].tv = texcoords[tIndex].y;
		indicesArray[i*3] = i*3;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		verticesArray[i*3+1].position = XMFLOAT3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
		verticesArray[i*3+1].normal = XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
		verticesArray[i*3+1].tu = texcoords[tIndex].x;
		verticesArray[i*3+1].tv = texcoords[tIndex].y;
		indicesArray[i*3+1] = i*3+1;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		verticesArray[i*3+2].position = XMFLOAT3(vertices[vIndex].x, vertices[vIndex].y, vertices[vIndex].z);
		verticesArray[i*3+2].normal = XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z);
		verticesArray[i*3+2].tu = texcoords[tIndex].x;
		verticesArray[i*3+2].tv = texcoords[tIndex].y;
		indicesArray[i*3+2] = i*3+2;

		facesNum++;
	}

	std::stringstream lol;
	lol << "Vertex count = " << vertexCount_ << " index count = " << indexCount_ << " faces " << facesNum;
	MessageBoxA(NULL, lol.str().c_str(), "Hola", MB_OK);

	// Set up the description of the static vertex buffer.
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.ByteWidth = sizeof(TexturedVertexType) * vertexCount_;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertex_data.pSysMem = verticesArray;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertexBuffer_);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed creating vertex buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Set up the description of the static index buffer.
	index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	index_buffer_desc.ByteWidth = sizeof(int) * indexCount_;
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	index_data.pSysMem = indicesArray;
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&index_buffer_desc, &index_data, &indexBuffer_);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed creating index buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

bool OBJModelClass::parseModelConfiguration(std::string modelName)
{
	std::ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	char input, input2;

	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		MessageBox(NULL, L"Failed creating vertices model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	texcoords = new VertexType[textureCount];
	if(!texcoords)
	{
		MessageBox(NULL, L"Failed creating texcoords model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	normals = new VertexType[normalCount];
	if(!normals)
	{
		MessageBox(NULL, L"Failed creating normals model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	faces = new FaceType[faceCount];
	if(!faces)
	{
		MessageBox(NULL, L"Failed creating faces model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(modelName);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
				    >> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
				    >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}
