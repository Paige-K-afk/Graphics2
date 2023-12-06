#include "Graphics2.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This is where you add nodes to the scene graph

	shared_ptr<CubeNode> head = make_shared<CubeNode>(L"head");
	head->SetWorldTransform(XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(0.0f, 34.0f + 150.0f, 310.0f));
	sceneGraph->Add(head);
	shared_ptr<CubeNode> body = make_shared<CubeNode>(L"body", L"Bricks.bmp");
	body->SetWorldTransform(XMMatrixScaling(5.0f, 8.0f, 2.5f) * XMMatrixTranslation(0.0f, 23.0f + 150.0f, 310.0f));
	sceneGraph->Add(body);
	shared_ptr<CubeNode> leftleg = make_shared<CubeNode>(L"leftleg", L"Concrete.bmp");
	leftleg->SetWorldTransform(XMMatrixScaling(1.0f, 7.5f, 1.0f) * XMMatrixTranslation(-4.0f, 7.5f + 150.0f, 310.0f));
	sceneGraph->Add(leftleg);
	shared_ptr<CubeNode> rightleg = make_shared<CubeNode>(L"rightleg", L"Concrete.bmp");
	rightleg->SetWorldTransform(XMMatrixScaling(1.0f, 7.5f, 1.0f) * XMMatrixTranslation(4.0f, 7.5f + 150.0f, 310.0f));
	sceneGraph->Add(rightleg);
	shared_ptr<CubeNode> leftarm = make_shared<CubeNode>(L"leftarm");
	leftarm->SetWorldTransform(XMMatrixScaling(1.0f, 8.5f, 1.0f) * XMMatrixTranslation(-6.0f, 22.0f + 150.0f, 310.0f));
	sceneGraph->Add(leftarm);
	shared_ptr<CubeNode> rightarm = make_shared<CubeNode>(L"rightarm");
	rightarm->SetWorldTransform(XMMatrixScaling(1.0f, 8.5f, 1.0f) * XMMatrixTranslation(6.0f, 22.0f + 150.0f, 310.0f));
	sceneGraph->Add(rightarm);


	// This is where you add nodes to the scene graph
	shared_ptr<TerrainNode> terrain = make_shared<TerrainNode>(L"Terrain1", L"Terrain\\Example_HeightMap.raw");//HeightMap4096.bmp");//
	sceneGraph->Add(terrain);

	
	shared_ptr<MeshNode> plane = make_shared<MeshNode>(L"Plane1", L"Plane\\Bonanza.3DS");
	plane->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(1.0f, 0.1f, 0.0f, 0.0f), 90 * XM_PI / 180.0f) * XMMatrixTranslation(0.0f,400.0f,0.0f));
	sceneGraph->Add(plane);

	GetCamera()->SetCameraPosition(0.0f, 550.0f, -500.0f);
	SetBackgroundColour(XMFLOAT4(0.392156899f, 0.584313750f, 0.929411829f, 1.0f));
	//SetBackgroundColour(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph

	
	_rotationAngle += 0.05f;
	if (_rotationAngle > 359.9f)
	{_rotationAngle = 0.0f;}
	FXMMATRIX rotation = FXMMATRIX(cos(_rotationAngle),0.0f, -sin(_rotationAngle),0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sin(_rotationAngle), 0.0f, cos(_rotationAngle), 0, 0.0f, 0.0f, 0.0f, 1.0f);
	sceneGraph.get()->Find(L"Plane1")->SetWorldTransform((XMMatrixRotationAxis(XMVectorSet(1.0f, 0.1f, 0.0f, 0.0f), 90 * XM_PI / 180.0f) * XMMatrixTranslation(0.0f, 0.0f, 0.0f))* XMMatrixTranslation(50.0f, 400.0f, 0)* rotation);

	if (GetAsyncKeyState(0x57) < 0)//W
	{
		GetCamera()->SetForwardBack(1);
	}
	if (GetAsyncKeyState(0x53) < 0)//S
	{
		GetCamera()->SetForwardBack(-1);
	}

	if (GetAsyncKeyState(0x44) < 0)//d
	{
		GetCamera()->SetYaw(1);
	}
	if (GetAsyncKeyState(0x41) < 0)//a
	{
		GetCamera()->SetYaw(-1);
	}

	if (GetAsyncKeyState(VK_SHIFT) < 0)//shift
	{
		GetCamera()->SetPitch(-1);
	}
	if (GetAsyncKeyState(VK_CONTROL) < 0)//ctrl
	{
		GetCamera()->SetPitch(1);
	}
}

