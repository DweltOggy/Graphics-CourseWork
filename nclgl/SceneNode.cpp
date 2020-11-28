#include "SceneNode.h"


SceneNode::SceneNode(Mesh* mesh, Vector4 colour)
{
	this -> mesh = mesh;
	this -> colour = colour;
	parent = NULL;
	shader = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	
}

SceneNode ::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
	
}

void SceneNode::AddChild(SceneNode* s)
{
	if (s != this)
	{
		children.push_back(s);
		s -> parent = this;
	}
	
}

void SceneNode::RemoveChild(SceneNode* s)
{
	//if (s && s != this)
	//{
	//	delete s;
	//	s = NULL;
	//}
}

void SceneNode::Draw(const OGLRenderer& r) 
{
	if (mesh) { mesh->Draw(); }
}

void SceneNode::Update(float dt)
{
	if (parent) 
	{ // This node has a parent ...
		worldTransform = parent -> worldTransform * transform;
	}
	else
	{ // Root node , world transform is local transform !
		 worldTransform = transform;
	}
	for (vector <SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) 
	{
		(*i) -> Update(dt);
	}
	
}