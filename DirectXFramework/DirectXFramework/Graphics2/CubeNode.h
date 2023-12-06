#pragma once
#include "SceneNode.h"
#include <vector>
#include "DirectXFramework.h"
#include "WICTextureLoader.h"

using namespace std;

class CubeNode : public SceneNode
{
public:

	// use DirectXFramework::GetDXFramework() to get GetDevice and GetDeviceContext.
	CubeNode() :SceneNode(L"default") {  };
	CubeNode(wstring name) : SceneNode(name) { _textureFile = L"Wood.bmp"; };
	CubeNode(wstring name, wstring filename) : SceneNode(name) { _textureFile = filename; };

	bool Initialise(void) override;
	void BuildGeometryBuffers();
	void Render(void) override;
	void Shutdown(void)override;

	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	//void BuildTexture(wchar_t filename[]);//input L"Filename.ext"
	void BuildTexture();

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout; //vertex input layout. This tells DirectX the format of each of the vertices we are sending to it.
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> _texture;
	wstring _textureFile;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;

};

