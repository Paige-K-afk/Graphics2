#include "TerrainNode.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColour;
	XMFLOAT4    AmbientColour;
};

bool TerrainNode::Initialise()
{
	//here and below may be no good.
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	// The call to CoInitializeEx is needed as the WIC library used requires it
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	
	BuildRendererStates();
	LoadHeightMap(_heightMapFile);

	LoadTerrainTextures();
	

	BuildGeometryBuffers();//1 //GenerateBlendMap(); is in here

	BuildShaders();//shaders 

	BuildVertexLayout();//3
	BuildConstantBuffer();//4

	//BuildTexture();//textures

	return true;
}

void TerrainNode::BuildRendererStates()
{
	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _defaultRasteriserState.GetAddressOf()));
	rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _wireframeRasteriserState.GetAddressOf()));
}

bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
{
	unsigned int mapSize = _numberOfXPoints * _numberOfZPoints;
	USHORT* rawFileValues = new USHORT[mapSize];

	std::ifstream inputHeightMap;
	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}

	inputHeightMap.read((char*)rawFileValues, mapSize * 2);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	for (unsigned int i = 0; i < mapSize; i++)
	{
		_heightValues.push_back((float)rawFileValues[i] / 65536);
	}
	delete[] rawFileValues;
	return true;
}

void TerrainNode::BuildGeometryBuffers()
{
	//verticies
	//int start = -((_numberOfXPoints * _WIDTH)/2);
	int xStart = -((_numberOfXPoints * _WIDTH) / 2);
	int zStart = ((_numberOfZPoints * _WIDTH) / 2);
	//int xPixel = start;
	//int zPixel = start;

	int i = -1;//for indicies can be 0 or -1
	//float t = 0;//for textures
	//float t_increment = float(_heightValues.size())/float(_numberOfXPoints * _numberOfZPoints*4);

	for (int x = 0; x < _numberOfXPoints-1; x++)
	{

		//zPixel = start;
		for (int z = 0; z < _numberOfZPoints-1; z++)
		{
			vertexContributingCount.push_back(0);
			vertexContributingCount.push_back(0);
			vertexContributingCount.push_back(0);
			vertexContributingCount.push_back(0);
			//xPixel, 0, zPixel 


			TerrainVertex node; //== v1
			node.Position = XMFLOAT3(float(x * _WIDTH + xStart), (hmapMultiplier * (_heightValues[z * _numberOfXPoints + x])), float((-z) * _WIDTH + zStart)); //(hmapMultiplier * (_heightValues[int(t)]))
			node.Normal = XMFLOAT3(0, 0, 0);
			node.TexCoord = XMFLOAT2(0, 0);
			node.BlendMapTexCoord = XMFLOAT2(0, 0);
			_terrainMap.push_back(node);
			//t = t + t_increment;

			//make 3 more nodes for the other corners of the square
			TerrainVertex node1;// == v2
			node1.Position = XMFLOAT3(float((x * _WIDTH + xStart) + _WIDTH), (hmapMultiplier * (_heightValues[z * _numberOfXPoints + x+1])), float((-z) * _WIDTH + zStart));//(hmapMultiplier * (_heightValues[int(t)]))
			node1.Normal = XMFLOAT3(0, 0, 0);
			node1.TexCoord = XMFLOAT2(0, 0);
			node1.BlendMapTexCoord = XMFLOAT2(0, 0);
			_terrainMap.push_back(node1);
			//t = t + t_increment;

			TerrainVertex node2;// ==v3
			node2.Position = XMFLOAT3(float(x * _WIDTH + xStart), (hmapMultiplier * (_heightValues[(z + 1) * _numberOfXPoints + x])), float(((-z - 1) * _WIDTH + zStart)));//(hmapMultiplier * (_heightValues[int(t)]))
			node2.Normal = XMFLOAT3(0, 0, 0);
			node2.TexCoord = XMFLOAT2(0, 0);
			node2.BlendMapTexCoord = XMFLOAT2(0, 0);
			_terrainMap.push_back(node2);
			//t = t + t_increment;

			TerrainVertex node3;// ==v4
			node3.Position = XMFLOAT3(float((x * _WIDTH + xStart) + _WIDTH), (hmapMultiplier * (_heightValues[(z+1) * _numberOfXPoints + x + 1])), float(((-z - 1) * _WIDTH + zStart)));//(hmapMultiplier * (_heightValues[int(t)]))
			node3.Normal = XMFLOAT3(0, 0, 0);
			node3.TexCoord = XMFLOAT2(0, 0);
			node3.BlendMapTexCoord = XMFLOAT2(0, 0);
			_terrainMap.push_back(node3);
			//t = t + t_increment;

			//indicies start
			//i must start at 0 or -1:
			i = i + 1;
			_indicies.push_back(i);
			i = i + 1;
			_indicies.push_back(i);
			i = i + 1;
			_indicies.push_back(i);
			_indicies.push_back(i);
			i = i - 1;
			_indicies.push_back(i);
			i = i + 2;
			_indicies.push_back(i);
			
			//zPixel = zPixel + _WIDTH;

		}
		//xPixel = xPixel + _WIDTH;
	}
	
	SetVertexNormals();
	GenerateBlendMap();

	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(TerrainVertex) * _numberOfXPoints * _numberOfZPoints*4;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_terrainMap[0];
	// and create the vertex buffer.
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));


	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(INT) * _numberOfXPoints * _numberOfZPoints * 6; //6 for 012213
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indicies[0];

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TerrainNode::SetVertexNormals()
{
	const int mapsize = _numberOfXPoints *_numberOfZPoints;

	int i = 0;
	for (int x = 0; x < _numberOfXPoints -1; x++)
	{

		for (int z = 0; z < _numberOfZPoints -1; z++)
		{
			//access each square
			//calculate vertex normal
			//XMFLOAT3 normal;//XMFLOAT3 is the variable of normal vector.
			
			
			int index0 = _indicies[i ];//i is the for loop position.
			int index1 = _indicies[i + 1];
			int index2 = _indicies[i + 2];
			int index3 = _indicies[i + 5];
			XMVECTOR u =XMVectorSet(_terrainMap[index2].Position.x - _terrainMap[index0].Position.x,
									_terrainMap[index2].Position.y - _terrainMap[index0].Position.y,
									_terrainMap[index2].Position.z - _terrainMap[index0].Position.z,
									0.0f);
			XMVECTOR v = XMVectorSet(_terrainMap[index1].Position.x - _terrainMap[index0].Position.x,
									_terrainMap[index1].Position.y - _terrainMap[index0].Position.y,
									_terrainMap[index1].Position.z - _terrainMap[index0].Position.z,
									0.0f);
			XMVECTOR a = XMVectorSet(_terrainMap[index1].Position.x - _terrainMap[index3].Position.x,
									_terrainMap[index1].Position.y - _terrainMap[index3].Position.y,
									_terrainMap[index1].Position.z - _terrainMap[index3].Position.z,
									0.0f);
			XMVECTOR b = XMVectorSet(_terrainMap[index2].Position.x - _terrainMap[index3].Position.x,
									_terrainMap[index2].Position.y - _terrainMap[index3].Position.y,
									_terrainMap[index2].Position.z - _terrainMap[index3].Position.z,
									0.0f);

			XMVECTOR normal = XMVector3Cross(v,u); // Calculaetes the cross product (normal)
			XMVECTOR normal2 = XMVector3Cross(b,a);

			//stores normals below:
			XMStoreFloat3(&_terrainMap[index0].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index0].Normal), normal));
			vertexContributingCount[index0]++;
			XMStoreFloat3(&_terrainMap[index1].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index1].Normal), normal));
			vertexContributingCount[index1]++;
			XMStoreFloat3(&_terrainMap[index2].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index2].Normal), normal));
			vertexContributingCount[index2]++;

			XMStoreFloat3(&_terrainMap[index3].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index3].Normal), normal2));
			vertexContributingCount[_indicies[index3]]++;
			XMStoreFloat3(&_terrainMap[index1].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index1].Normal), normal2));
			vertexContributingCount[index1]++;
			XMStoreFloat3(&_terrainMap[index2].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[index2].Normal), normal2));
			vertexContributingCount[index2]++;
			
			//** apply the normal that has been calculated to connecetd corners too!!! **
			
			int cornerIndex0UD;//if i is odd or even. even or 0 is up, odd is down.
			if (i % 2 == 1){cornerIndex0UD = index0 - 6;}//down
			else{cornerIndex0UD = index0 + 6;}//up
			int cornerIndex0L = index0 -6;

			int cornerIndex1U = index1 +6;
			int cornerIndex1R = index1 + ((_numberOfZPoints * 4)-4);
			
			int cornerIndex2D = index2 - 6;
			int cornerIndex2LR;
			if (i % 2 == 1){cornerIndex2LR = index2 + ((_numberOfZPoints * 4)-4);}//right
			else {cornerIndex2LR = index2 - ((_numberOfZPoints * 4)-4);}//left
			
			int cornerIndexes[6] = { cornerIndex0UD, cornerIndex0L, cornerIndex1U, cornerIndex1R, cornerIndex2D, cornerIndex2LR };
			for (int connectedC = 0; connectedC<6;connectedC++)
			{// Check for oob
				if (cornerIndexes[connectedC] >= 0 && cornerIndexes[connectedC] < ((_numberOfXPoints -1) * ((_numberOfZPoints-1)* 4 - 4)))
				{//apply
					XMStoreFloat3(&_terrainMap[cornerIndexes[connectedC]].Normal, XMVectorAdd(XMLoadFloat3(&_terrainMap[cornerIndexes[connectedC]].Normal), normal));
					vertexContributingCount[cornerIndexes[connectedC]]++;
				}
			}//*

			i = i + 6;
		}
	}

	// Now divide the vertex normals by the contributing counts and normalise
	for (int e=0 ; e< vertexContributingCount.size();e++)
	{
		XMVECTOR vertexNormal = XMLoadFloat3(&_terrainMap[e].Normal) / (float)vertexContributingCount[e];
		//vertexNormal is the normal divided by contribution count.

		//** Normalise normals **
		//get magnitude
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, vertexNormal);
		float x = temp.x;
		float y = temp.y;
		float z = temp.z;
		float magnitude = sqrt((x * x) + (y * y) + (z * z));
		//divide each part of the normal by the magnitude

		XMStoreFloat3(&_terrainMap[e].Normal, vertexNormal /magnitude);
	}
}

void TerrainNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VShader", "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);

	// Compile pixel shader
	hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader", "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
}

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
	_deviceContext->IASetInputLayout(_layout.Get());
}

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TerrainNode::BuildTexture()
{
	ThrowIfFailed(CreateWICTextureFromFile(_device.Get(),
		_deviceContext.Get(),
		_wcsdup(_textureFile.c_str()),
		nullptr,
		_texture.GetAddressOf()
	));
}

void TerrainNode::LoadTerrainTextures()
{
	// Change the paths below as appropriate for your use
	wstring terrainTextureNames[5] = { L"Terrain\\grass.dds", L"Terrain\\darkdirt.dds", L"Terrain\\stone.dds", L"Terrain\\lightdirt.dds", L"Terrain\\snow.dds" };

	// Load the textures from the files
	ComPtr<ID3D11Resource> terrainTextures[5];
	for (int i = 0; i < 5; i++)
	{
		ThrowIfFailed(CreateDDSTextureFromFileEx(_device.Get(),
											     _deviceContext.Get(),
											     terrainTextureNames[i].c_str(),
												 0,
												 D3D11_USAGE_IMMUTABLE,
												 D3D11_BIND_SHADER_RESOURCE,
												 0,
												 0,
												 false,
											     terrainTextures[i].GetAddressOf(),
											     nullptr
											  ));
	}
	// Now create the Texture2D arrary.  We assume all textures in the
	// array have the same format and dimensions

	D3D11_TEXTURE2D_DESC textureDescription;
	ComPtr<ID3D11Texture2D> textureInterface;
	terrainTextures[0].As<ID3D11Texture2D>(&textureInterface);
	textureInterface->GetDesc(&textureDescription);

	D3D11_TEXTURE2D_DESC textureArrayDescription;
	textureArrayDescription.Width = textureDescription.Width;
	textureArrayDescription.Height = textureDescription.Height;
	textureArrayDescription.MipLevels = textureDescription.MipLevels;
	textureArrayDescription.ArraySize = 5;
	textureArrayDescription.Format = textureDescription.Format;
	textureArrayDescription.SampleDesc.Count = 1;
	textureArrayDescription.SampleDesc.Quality = 0;
	textureArrayDescription.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureArrayDescription.CPUAccessFlags = 0;
	textureArrayDescription.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> textureArray = 0;
	ThrowIfFailed(_device->CreateTexture2D(&textureArrayDescription, 0, textureArray.GetAddressOf()));

	// Copy individual texture elements into texture array.

	for (UINT i = 0; i < 5; i++)
	{
		// For each mipmap level...
		for (UINT mipLevel = 0; mipLevel < textureDescription.MipLevels; mipLevel++)
		{
			_deviceContext->CopySubresourceRegion(textureArray.Get(),
												  D3D11CalcSubresource(mipLevel, i, textureDescription.MipLevels),
												  NULL,
												  NULL,
												  NULL,
												  terrainTextures[i].Get(),
												  mipLevel,
												  nullptr
												);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = textureArrayDescription.Format;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDescription.Texture2DArray.MostDetailedMip = 0;
	viewDescription.Texture2DArray.MipLevels = textureArrayDescription.MipLevels;
	viewDescription.Texture2DArray.FirstArraySlice = 0;
	viewDescription.Texture2DArray.ArraySize = 5;

	ThrowIfFailed(_device->CreateShaderResourceView(textureArray.Get(), &viewDescription, _texturesResourceView.GetAddressOf()));
}

void TerrainNode::GenerateBlendMap()
{
	// Note that _numberOfRows and _numberOfColumns need to be setup
	// to the number of rows and columns in your grid in order for this
	// to work.
	DWORD * blendMap = new DWORD[_numberOfRows * _numberOfColumns];
	DWORD * blendMapPtr = blendMap;
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;

	DWORD index = 0;
	for (DWORD i = 0; i < _numberOfColumns; i++)
	{
		for (DWORD j = 0; j < _numberOfRows; j++)
		{
			
			// Calculate the appropriate blend colour for the 
			// current location in the blend map.  This has been
			// left as an exercise for you.  You need to calculate
			// appropriate values for the r, g, b and a values (each
			// between 0 and 255). The code below combines these
			// into a DWORD (32-bit value) and stores it in the blend map.


			//_heightValues[i * _numberOfXPoints + j+1] //_heightValues[z * _numberOfXPoints + x+1] gets the coordinate in height values.
			float currentH = _heightValues[i * _numberOfXPoints + j + 1];
			//sets to grass.
			r = 0;// dirt
			g = 0;// stone
			b = 0;// light dirt
			a = 0;// snow
			
			if (currentH > 0.2 && currentH < 0.3)
			{
				r = BYTE(318 * (1-currentH));
			}
			if (currentH < 0.2)
			{
				if (currentH < 0.1)
				{
					g = 255;
				}
				g = BYTE((1 - currentH) * 255);
			}
			if (currentH > 0.25 && currentH < 0.4)
			{
				b = BYTE(255 * (1 - currentH));
			}
			if (currentH > 0.7)
			{
				a = BYTE(255 * currentH);
			}
			
			//TEST RUN:
			//r = 250;// dirt
			//g = 250;// stone
			//b = 250;// light dirt
			//a = 250;// snow
			//DWORD mapValue = (a << 24) + (r << 16) + (g << 8) + b;


			DWORD mapValue = (a << 24) + (b << 16) + (g << 8) + r;//given code starts again comment back in later
			*blendMapPtr++ = mapValue;
		}
	}
	D3D11_TEXTURE2D_DESC blendMapDescription;
	blendMapDescription.Width = _numberOfRows;
	blendMapDescription.Height = _numberOfColumns;
	blendMapDescription.MipLevels = 1;
	blendMapDescription.ArraySize = 1;
	blendMapDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	blendMapDescription.SampleDesc.Count = 1;
	blendMapDescription.SampleDesc.Quality = 0;
	blendMapDescription.Usage = D3D11_USAGE_DEFAULT;
	blendMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDescription.CPUAccessFlags = 0;
	blendMapDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA blendMapInitialisationData;
	blendMapInitialisationData.pSysMem = blendMap;
	blendMapInitialisationData.SysMemPitch = 4 * _numberOfColumns;

	ComPtr<ID3D11Texture2D> blendMapTexture;
	ThrowIfFailed(_device->CreateTexture2D(&blendMapDescription, &blendMapInitialisationData, blendMapTexture.GetAddressOf()));

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDescription.Texture2D.MostDetailedMip = 0;
	viewDescription.Texture2D.MipLevels = 1;

	ThrowIfFailed(_device->CreateShaderResourceView(blendMapTexture.Get(), &viewDescription, _blendMapResourceView.GetAddressOf()));
	delete[] blendMap;
}


void TerrainNode::Render()
{

	// get projectionTransf and viewTransf from the directxframework

	//Get view and perspective matricies
	//solution2
	XMMATRIX view = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();
	//XMMATRIX view = DirectXFramework::GetDXFramework()->GetViewTransformation();
	XMMATRIX perspective = DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	// completetransf = combined (comes from SceneNode within the cube itself) * viewtransf * projectionTransf
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * view * perspective;

	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_combinedWorldTransformation);
	cBuffer.AmbientColour = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	cBuffer.LightVector = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	cBuffer.LightColour = XMFLOAT4(0.90f, 0.90f, 0.90f, 1.0f);

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());//

	_deviceContext->PSSetShaderResources(0, 1, _blendMapResourceView.GetAddressOf());//&&**** Blend Map
	_deviceContext->PSSetShaderResources(1, 1, _texturesResourceView.GetAddressOf());//&&**** Blend Map

	// Now render the cube
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get());//the only line that matters.

	_deviceContext->DrawIndexed((_numberOfXPoints*_numberOfZPoints*6), 0, 0);

	// below is just so i know the terrain is still in one peice.
	//0x45
	if (GetAsyncKeyState(0x45) < 0)//E
	{
		_deviceContext->RSSetState(_wireframeRasteriserState.Get());//the only line that matters.
		_deviceContext->DrawIndexed((_numberOfXPoints * _numberOfZPoints * 6), 0, 0);
	}

}

void TerrainNode::Shutdown()
{
	//Required because we called CoInitialize above
	CoUninitialize();
}