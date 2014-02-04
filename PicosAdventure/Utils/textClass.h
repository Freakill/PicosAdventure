#ifndef _TEXT_CLASS_H_
#define _TEXT_CLASS_H_

#include "../Graphics/fontClass.h"
#include "../Graphics/shader2DClass.h"

#include "../Math/pointClass.h"

class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
		int width;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool setup(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int posX, int posY, std::string sentenceText,
			   XMFLOAT4X4 baseViewMatrix);
	bool draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 orthoMatrix);
	void destroy();

	bool setText(std::string text, ID3D11DeviceContext*);

	Point getTextPosition();
	int getTextWidth();
	int getTextHeight();

private:
	bool setupSentence(SentenceType**, int, ID3D11Device*);
	bool updateSentence(SentenceType*, std::string, int, int, float, float, float, ID3D11DeviceContext*);
	bool drawSentence(ID3D11DeviceContext*, SentenceType*, XMFLOAT4X4, XMFLOAT4X4);
	void destroySentence(SentenceType**);

	FontClass* font_;
	Shader2DClass* fontShader_;
	int screenWidth_, screenHeight_;
	int positionX_, positionY_;
	XMFLOAT4X4 baseViewMatrix_;
	SentenceType* sentence_;
};

#endif