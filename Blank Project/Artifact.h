#pragma once
#include "..\nclgl\scenenode.h"

class Artifact : public SceneNode
{
public:
	Artifact();
	~Artifact(void) {};
	void Update(float dt) override;
	void setCube(Mesh* mesh, GLuint texture, Shader* shader);
protected:
	Vector3 rotation;
	SceneNode* cube;
};

