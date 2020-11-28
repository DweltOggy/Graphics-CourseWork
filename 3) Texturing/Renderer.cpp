#include "Renderer.h"


Renderer::Renderer(Window& parent) : OGLRenderer(parent) 
{
	triangle = Mesh::GenerateTriangle();

	camera = new Camera;
	
	texture1 = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga ",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	//texture2 = SOIL_load_OGL_texture(TEXTUREDIR "brickDOT3.tga ", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	
	if (!texture1)// || !texture2)
	{
		return;
	}
	shader = new Shader("TexturedVertex.glsl", "texturedfragment.glsl");
		
	if (!shader -> LoadSuccess()) 
	{
		return;	
	}
	filtering = true;
	repeating = false;
	init = true;
		
}

Renderer ::~Renderer(void) 
{
	delete triangle;
	delete shader;
	glDeleteTextures(1, &texture1);
	//glDeleteTextures(1, &texture2);
}

void Renderer::RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shader -> GetProgram(),"diffuseTex"), 0); // this last parameter
	glActiveTexture(GL_TEXTURE0); // should match this number !
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	triangle -> Draw();
	
}

void Renderer::UpdateTextureMatrix(float value) 
{
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}

void Renderer::ToggleRepeating()
{
	repeating = !repeating;
	SetTextureRepeating(texture1, repeating);	
}

void Renderer::ToggleFiltering()
{
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void Renderer::UpdateScene(float dt)
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}
