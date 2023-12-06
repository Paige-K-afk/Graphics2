#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include <fstream>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

class TerrainNode : public SceneNode
{
public:
	TerrainNode() :SceneNode(L"default") { _heightMapFile = L"Terrain\\Example_HeightMap.raw"; };
	TerrainNode(wstring name, wstring filename) : SceneNode(name) { _heightMapFile = filename; _textureFile = L"white.png";};
	TerrainNode(wstring name, wstring filename, wstring texname) : SceneNode(name) { _heightMapFile = filename; _textureFile = texname; };


	bool Initialise();
	void Render();
	void Shutdown();

	void BuildGeometryBuffers();

	void SetVertexNormals();

	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();

	void BuildRendererStates();
	bool LoadHeightMap(wstring heightMapFilename);

	void GenerateBlendMap();
	void LoadTerrainTextures();

	ComPtr<ID3D11RasterizerState> _defaultRasteriserState;
	ComPtr<ID3D11RasterizerState> _wireframeRasteriserState;

private:
	wstring _heightMapFile;
	vector<TerrainVertex> _terrainMap;
	vector<UINT> _indicies;
	
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

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;

	ComPtr<ID3D11ShaderResourceView> _texturesResourceView;//for blended
	ComPtr<ID3D11ShaderResourceView> _blendMapResourceView;//for blended

	vector <float> _heightValues;
	int _WIDTH = 10;//CHANGE BACK TO 10
	int _numberOfXPoints = 1024;
	int _numberOfZPoints = 1024;
	int _numberOfRows = 1022;//_numberOfXPoints -2
	int	_numberOfColumns = 1022;//_numberOfZPoints -2

	float hmapMultiplier = 500;

	vector<int> vertexContributingCount;
};