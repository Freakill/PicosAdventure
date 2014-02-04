#ifndef _TEXTURE_CLASS_H_
#define _TEXTURE_CLASS_H_

#include <d3d11.h>
#include <d3dx11tex.h>

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <sstream>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool setup(ID3D11Device*, const std::string& strFilename);
	void destroy();

	ID3D11ShaderResourceView* getTexture();

private:
	ID3D11ShaderResourceView* texture_;
};

#endif