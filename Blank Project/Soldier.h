#pragma once
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/OGLRenderer.h"

class Soldier
{
public:
	Soldier();
	~Soldier();

	void renderSoldier();
	void update(float dt);
	void setPos(Vector3 x) {pos = x;};

	//Shader* shader;

	Mesh* mesh;
	MeshAnimation* anim;
	MeshMaterial* material;
	vector <GLuint> matTextures;

	Vector3 pos;

	int currentFrame;
	float frameTime;
};

