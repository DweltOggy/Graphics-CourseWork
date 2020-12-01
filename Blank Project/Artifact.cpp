#include "Artifact.h"
Artifact::Artifact()
{
	//SetMesh (mesh); 

}

void Artifact::setCube(Mesh* mesh, GLuint texture, Shader* shader)
{
	cube = new SceneNode(mesh, Vector4(1, 1, 1, 1));
	cube->SetModelScale(Vector3(50, 50, 50));
	//cube->SetTransform(GetTransform() * Matrix4::Translation(Vector3(0, 0, 0)));
	cube->SetTexture(texture);
	cube->SetShader(shader);
	cube->SetBoundingRadius(10000.0f);

	rotation.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	rotation.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	rotation.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	AddChild(cube);

}

void Artifact::Update(float dt)
{
	
	//transform = transform * Matrix4::Rotation((rand() % 30)* dt, Vector3(1, 0, 0));

	cube->SetTransform(cube->GetTransform() * Matrix4::Rotation((rand() % 30) * dt, rotation));
	SceneNode::Update(dt);
}