#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) 
{
	cube = Mesh::LoadFromMeshFile("OffsetCube.msh");
	camera = new Camera();
	
	shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	
	if (!shader -> LoadSuccess()) 
	{
		return;
	}
	
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,(float)width / (float)height, 45.0f);
	
	camera -> SetPosition(Vector3(0, 30, 175));
	for (int i = 0; i < 10; i++)
	{
		root[i] = new SceneNode();
		root[i]-> SetTransform(Matrix4::Translation(Vector3(100.0f, 0, -300.0f + 100.0f + 100 * i)));
		//root[i]-> SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		root[i] -> AddChild(new CubeRobot(cube));
	}

	
	glEnable(GL_DEPTH_TEST);
	init = true;
	
}

Renderer ::~Renderer(void) 
{
	//delete[] root;

	for (int i = 0; i < 10; i++)
	{
		delete root[i];
	}

	delete shader;
	delete camera;
	delete cube;
}

void Renderer::UpdateScene(float dt)
{
	camera -> UpdateCamera(dt);
	viewMatrix = camera -> BuildViewMatrix();

	for (int i = 0; i < 10; i++)
	{
		root[i]->Update(dt);
	}

	
}

void Renderer::RenderScene() 
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	BindShader(shader);
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shader -> GetProgram(),"diffuseTex"), 1);
	
	//DrawNode(root);
	for (int i = 0; i < 10; i++)
	{
		DrawNode(root[i]);
	}
}

void Renderer::DrawNode(SceneNode * n) 
{
	if (n -> GetMesh()) 
	{
		Matrix4 model = n -> GetWorldTransform() *Matrix4::Scale(n -> GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader -> GetProgram(),"modelMatrix"), 1, false, model.values);
		
		glUniform4fv(glGetUniformLocation(shader -> GetProgram(),"nodeColour"), 1, (float*)&n -> GetColour());
		
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), 0); // Next tutorial ;)
		n -> Draw(*this);
				
	}
	
	for (vector < SceneNode* >::const_iterator i = n -> GetChildIteratorStart();i != n -> GetChildIteratorEnd(); ++i) 
	{
		DrawNode(*i);
	}
}
