#include "Renderer.h"


 Renderer::Renderer(Window& parent):OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();
	basicShader = new Shader("basicVertex.glsl", "colourFragment.glsl");
	 
	if (!basicShader -> LoadSuccess()) 
	{
		return;	 
	}
	init = true;
}

Renderer::~Renderer(void)
{
	delete triangle;
	delete basicShader;
}

void Renderer::RenderScene()
{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	

	BindShader(basicShader);
	//glPointSize(10);

	triangle->Draw();
	
	glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);

	//colour[0] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//colour[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	//colour[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	glUnmapBuffer(GL_ARRAY_BUFFER);
}