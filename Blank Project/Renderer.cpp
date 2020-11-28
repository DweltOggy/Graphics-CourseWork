#include "Renderer.h"
#include "../nclgl/CubeRobot.h"

const int POST_PASSES = 1;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise9.png");

	building1 = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	building2 = Mesh::LoadFromMeshFile("cylinder.msh");
	
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");

	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");

	walltexture1 = SOIL_load_OGL_texture(TEXTUREDIR "scraper2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	wallBump1 = SOIL_load_OGL_texture(
		TEXTUREDIR "ScraperBump2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR "Brushed_Metal2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(
		TEXTUREDIR "BrushedBump.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	roadTexture = SOIL_load_OGL_texture(
		TEXTUREDIR "asphalt.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	roadBump = SOIL_load_OGL_texture(
		TEXTUREDIR "ScraperBump2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	pavedTexture = SOIL_load_OGL_texture(
		TEXTUREDIR "sidewalk.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(walltexture1, true);
	SetTextureRepeating(wallBump1, true);

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);

	SetTextureRepeating(roadTexture, true);
	SetTextureRepeating(roadBump, true);

	SetTextureRepeating(pavedTexture, true);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "sunwave_west.jpg", TEXTUREDIR "sunwave_east.jpg",
		TEXTUREDIR "sunwave_up.jpg", TEXTUREDIR "sunwave_down.jpg",
		TEXTUREDIR "sunwave_south.jpg", TEXTUREDIR "sunwave_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	camera = new Camera(0, -90.f, Vector3(0, 25.0f, 0));//-45.0f, 0.0f,heightmapSize * Vector3(0.5f, 5.0f, 0.5f)
	light = new Light(Vector3(3000.0f,1000.0f, 0.0f), Vector4(1, 0.75, 1, 1), 5000.0f);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,(float)width / (float)height, 90.0f);

	placeTerrain();

	// Generate our scene depth texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	// And our colour texture ...
	for (int i = 0; i < 2; ++i)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	}

	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post processing in this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);
	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}
Renderer::~Renderer(void)	
{
	delete camera;
	delete quad;
	delete building1;
	delete skyboxShader;
	delete lightShader;
	delete reflectShader;
	delete light;
	delete heightMap;
	delete sceneShader;
	delete processShader;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);

		}
		else
		{
			nodeList.push_back(from);
		}

	}

	for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}

}

void Renderer::DrawNodes()
{
	for (const auto& i : nodeList)
	{
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList)
	{
		DrawNode(i);
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::RenderScene()	
{
	DrawScene();
	DrawPostProcess();
	PresentScene();
}

void Renderer::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	
	BuildNodeLists(root);
	SortNodeLists();
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 90.0f);
	DrawSkybox();
	DrawNodes();


	ClearNodeLists();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);

}

void Renderer::DrawNode(SceneNode* n)
{	
	if (n->GetShader())
	{	
		BindShader(n->GetShader());
		SetShaderLight(*light);
		UpdateShaderMatrices();
		
	}

	if (n->GetMesh())
	{
	
		glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "diffuseTex"), 0);

		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		if (n->GetBumpTexture())
		{
			glUniform1i(glGetUniformLocation(
				n->GetShader()->GetProgram(), "bumpTex"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, n->GetBumpTexture());
		}

		glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "useTexture"), texture);

		n->Draw(*this);

	}

}

void Renderer::placeTerrain()
{

	root = new SceneNode();
	root->SetShader(NULL);
	root->SetBoundingRadius(10000.0f);

	SceneNode* Scenery = new SceneNode(heightMap, Vector4(1, 0, 1, 1));

	float terrainsize = heightMap->GetHeightmapSize().Length() * 15.0f;
	//R->SetTransform(Matrix4::Rotation(90.0f,Vector3(0, 1, 0)));

	Scenery->SetModelScale(Vector3(15.0f, 15.0f, 15.0f));

	Scenery->SetTransform(Scenery->GetTransform() * Matrix4::Translation(Vector3(-150.0f, 0.0f, -4500.0f)));
	Scenery->SetTexture(earthTex);
	Scenery->SetBumpTexture(earthBump);
	Scenery->SetShader(lightShader);
	Scenery->SetBoundingRadius(terrainsize);

	root->AddChild(Scenery);

	//road
	for (int i = 0; i < 15; i++)
	{
		SceneNode* s = new SceneNode(quad, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(120.0f, 300.0f, 200.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0.5, 1.01, 0.1)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(Vector3(0, 0, 100.0f + 600 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(roadTexture);
		s->SetBumpTexture(roadBump);
		s->SetShader(lightShader);
		s->SetBoundingRadius(10000.0f);
		root->AddChild(s);
	}

	//pavement
	for (int i = 0; i < 15; i++)
	{
		SceneNode* s = new SceneNode(quad, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(150.0f, 300.0f, 100.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0.5, 0.95, 0.1)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(Vector3(0, -0.2f, 100.0f + 600 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(pavedTexture);
		s->SetBumpTexture(NULL);
		s->SetShader(lightShader);
		s->SetBoundingRadius(10000.0f);
		root->AddChild(s);
	}

	//SceneNode* r = new SceneNode(building1, Vector4(1, 1, 1, 1));
	//r->SetModelScale(Vector3(100.0f, 300.0f, 100.0f));
	//r->SetTransform(Matrix4::Translation(Vector3(0, 0, 250.0f)));
	//r->SetTexture(walltexture1);
	//r->SetBumpTexture(wallBump1);
	//r->SetShader(lightShader);
	//r->SetBoundingRadius(10000.0f);
	//root->AddChild(r);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < POST_PASSES; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
}