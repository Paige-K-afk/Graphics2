#include "SceneGraph.h"

//Initialise
bool SceneGraph::Initialise()
{
	//Call the Initialise method on each child node.
	//If any node returns false, then Initialise should return false.
	//If all child nodes return true, then Initialise should return true.

	for (int i = 0; i != _children.size(); i++)
	{
		if (!_children[i].get()->Initialise())
		{
			return false;
		}
	}
	return true;
}


//Update 
void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	//Update the combined world transformation for itself

	SceneNode::Update(currentWorldTransformation);
	
	FXMMATRIX combined = XMLoadFloat4x4(&_combinedWorldTransformation);

	//and then call the Update method for each child node, 
	//passing the combined world transformation to those nodes.

	//for (int i = 0; i != _children.size(); i++)
	for each(SceneNodePointer child in _children)
	{

		child->Update(combined);
		
		//pass transformation from parent to child.
	}
}

//Render 
void SceneGraph::Render()
{
	//Call the Render method on each child node.
	for (int i = 0; i != _children.size(); i++)
	{
		_children[i].get()->Render();
	}
}


//Shutdown 
void SceneGraph::Shutdown()
{

	//Call the Shutdown method on each child node
	for (int i = 0; i != _children.size(); i++)
	{
		_children[i].get()->Shutdown();
	}
}


//Add 
void SceneGraph::Add(SceneNodePointer node)
{
	//Add the specified node to the list of child nodes.
	node.get()->Add(node);

	//pushes the pointer into its own list:
	_children.push_back(node);//check

}

//Remove 
void SceneGraph::Remove(SceneNodePointer node)
{
	//Call Remove on all child nodes.
	//If the child node is the one to remove, 
	//remove it fromthe list of child nodes.

	for (int i = 0 ; i != _children.size(); i++)
	{
		if (_children[i] == node)
		{
			_children[i].get()->Remove(node);//check
			_children.erase(_children.begin() + i);//check
		}
	}
}

//Find 
SceneNodePointer SceneGraph::Find(wstring name)
{
	//If we are the node being searched for, return a pointer to ourselves.
	//If not, call Find on all child nodes.
	//If the call to Find succeeds, return the pointer to the found node
	//otherwise return nullptr.


	for (int i = 0; i != _children.size(); i++)
	{
		
		if (_children[i].get()->Find(name) != nullptr)
		{
			return _children[i];
		}
		
	}

	return nullptr;
}