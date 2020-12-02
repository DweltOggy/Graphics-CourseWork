#include "Renderer.h"
#include "Artifact.h"
#include "../nclgl/CubeRobot.h"

const int POST_PASSES = 3;
const int NO_SCRAPERS = 60;
const int NO_ART = 10;
const int NO_LIGHTS = 5;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise12.png");

	building1 = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");

	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");

	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	CRTprocessShader = new Shader("TexturedVertex.glsl", "#COCRTProcess.glsl");

	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");

	//wireFrameShader = new Shader("#COwireVertex.glsl", "#COwireFrag.glsl", "#COwireGeometry.glsl");

	walltexture[0] = SOIL_load_OGL_texture(TEXTUREDIR "scraper.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	walltexture[1] = SOIL_load_OGL_texture(TEXTUREDIR "scraper2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	walltexture[2] = SOIL_load_OGL_texture(TEXTUREDIR "scraper4.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	blockadeTex = SOIL_load_OGL_texture(TEXTUREDIR "Diamond_Plate_ALBEDO.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);


	wallBump1 = SOIL_load_OGL_texture(
		TEXTUREDIR "ScraperBump2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR "Purple.jpg", SOIL_LOAD_AUTO,
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

	waterTex = SOIL_load_OGL_texture(
		TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	for (int i = 0; i < 3; i++)
	{
		SetTextureRepeating(walltexture[i], true);
	}
	
	SetTextureRepeating(blockadeTex, true);
	SetTextureRepeating(wallBump1, true);

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);

	SetTextureRepeating(roadTexture, true);
	SetTextureRepeating(roadBump, true);

	SetTextureRepeating(pavedTexture, true);

	othercubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
		TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
		TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "sunwave_west.jpg", TEXTUREDIR "sunwave_east.jpg",
		TEXTUREDIR "sunwave_up.jpg", TEXTUREDIR "sunwave_down.jpg",
		TEXTUREDIR "sunwave_south.jpg", TEXTUREDIR "sunwave_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

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

	glGenFramebuffers(1, &bufferFBO); 
	glGenFramebuffers(1, &processFBO); 

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

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
	delete camera2;
	delete quad;
	delete cube;
	delete building1;
	delete skyboxShader;
	delete lightShader;
	delete reflectShader;

	delete lights;

	delete heightMap;
	delete sceneShader;
	delete processShader;
	delete CRTprocessShader;

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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 90.0f);
	DrawSkybox();
	DrawNodes();
	DrawWater();
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
		SetShaderLight(*lights);
		UpdateShaderMatrices();
		
	}

	if (n->GetMesh())
	{
		if (n->GetShader() == reflectShader)
		{

			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
				"cameraPos"), 1, (float*)&camera->GetPosition());

			glUniform1i(glGetUniformLocation(
				n->GetShader()->GetProgram(), "diffuseTex"), 0);
			glUniform1i(glGetUniformLocation(
				n->GetShader()->GetProgram(), "cubeTex"), 2);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, othercubeMap);


			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
				"cameraPos"), 1, (float*)&camera->GetPosition());

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "modelMatrix"), 
																				1, false, model.values);
			//UpdateShaderMatrices();
			SetShaderLight(*lights);

			n->Draw(*this);
		}
		else
		{
			glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "diffuseTex"), 0);

			texture = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "modelMatrix"), 1, false, model.values);

			if (n->GetBumpTexture())
			{
				glUniform1i(glGetUniformLocation(
					n->GetShader()->GetProgram(), "bumpTex"), 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, n->GetBumpTexture());
			}

			//glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "useTexture"), texture);
			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
				"cameraPos"), 1, (float*)&camera->GetPosition());

			modelMatrix.ToIdentity();
			textureMatrix.ToIdentity();

			n->Draw(*this);

		}

	}

}

void Renderer::placeTerrain()
{

	root = new SceneNode();
	root->SetShader(NULL);
	SceneNode* Scenery = new SceneNode(heightMap, Vector4(1, 0, 1, 1));
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	
	float terrainsize = heightMap->GetHeightmapSize().Length() * 5.0f;
	Scenery->SetTexture(earthTex);
	Scenery->SetBumpTexture(earthBump);
	Scenery->SetShader(lightShader);
	Scenery->SetBoundingRadius(terrainsize);

	camera = new Camera(0, -90.f, heightmapSize * Vector3(0.05, 0.5f, 0.5));

	camera2 = new Camera(0, 90.f, heightmapSize * Vector3(0.95, 0.5f, 0.5));

	lights = new Light(Vector3(0, 0, 100.0f), Vector4(1, 0.75, 1, 1), 1000.0f);
	lights->SetPosition(heightmapSize * Vector3(0.5, 0.5, 0.5));

	//road
	for (int i = 0; i < 11; i++)
	{
		SceneNode* s = new SceneNode(quad, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(120.0f, 300.0f, 200.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 1.01, 0.1)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(heightmapSize * Vector3(-0.5, 0.07, 0 + 0.09 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(roadTexture);
		s->SetBumpTexture(roadBump);
		s->SetShader(lightShader);
		s->SetBoundingRadius(5000.0f);
		Scenery->AddChild(s);
	}
	//pavement
	for (int i = 0; i < 11; i++)
	{
		SceneNode* s = new SceneNode(quad, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(150.0f, 300.0f, 100.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0.5, 0.95, 0.1)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(heightmapSize * Vector3(-0.5, 0.069, 0 + 0.09 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(pavedTexture);
		s->SetBumpTexture(NULL);
		s->SetShader(lightShader);
		s->SetBoundingRadius(10000.0f);
		Scenery->AddChild(s);
	}

	for (int i = 0; i < 10; i++)
	{
		SceneNode* r = new SceneNode(building1, Vector4(1, 1, 1, 1));
		r->SetModelScale(Vector3(100.0f, 300.0f, 1100.0f));
		r->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0, 0, -1 + 0.34 * i)));

		r->SetTexture(walltexture[1]);
		r->SetBumpTexture(wallBump1);
		r->SetShader(lightShader);
		r->SetBoundingRadius(10000.0f);
		root->AddChild(r);
	}

	SceneNode* portal = new SceneNode(building1, Vector4(1, 1, 1, 1));
	portal->SetModelScale(Vector3(160.0f, 120.0f, 3.0f));

	portal->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.94, 0.05, 0.5)));
	portal->SetTransform(portal->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
	portal->SetTexture(waterTex);
	portal->SetBumpTexture(earthBump);
	portal->SetShader(reflectShader);
	portal->SetBoundingRadius(10000.0f);

	root->AddChild(portal);

	for (int i = 0; i < NO_ART; i++)
	{
		Artifact* s = new Artifact();
		float x = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0.1)));
		float y = 0.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (8.0 - 0.5)));
		float z = -0.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.5));
		s->SetTransform(Matrix4::Translation(heightmapSize * Vector3(x, y, z)));
		s->SetShader(NULL);
		s->setCube(building1, earthTex, reflectShader);
		Scenery->AddChild(s);
	}

	for (int i = 0; i < NO_SCRAPERS; i++)
	{
		SceneNode* r = new SceneNode(building1, Vector4(1, 1, 1, 1));
		r->SetModelScale(Vector3(100.0f, rand() % (int)500 + 300, rand() % (int)50 + 100.0f));
		r->SetTransform(Matrix4::Translation(heightmapSize * Vector3(-0.05 , 0.6, -1 + 0.05 * i )));
		
		r->SetTexture(walltexture[rand()% 3]);
		r->SetBumpTexture(wallBump1);
		r->SetShader(lightShader);
		r->SetBoundingRadius(10000.0f);
		root->AddChild(r);

	}

	for (int i = 0; i < NO_SCRAPERS; i++)
	{
		SceneNode* r = new SceneNode(building1, Vector4(1, 1, 1, 1));
		r->SetModelScale(Vector3(100.0f, rand() % (int)500 + 600, rand() % (int)50 + 100.0f));
		r->SetTransform(Matrix4::Translation(heightmapSize * Vector3(-0.2, 0.6, -1 + 0.05 * i)));

		r->SetTexture(walltexture[rand() % 3]);
		r->SetBumpTexture(wallBump1);
		r->SetShader(lightShader);
		r->SetBoundingRadius(10000.0f);
		root->AddChild(r);

	}

	for (int i = 0; i < NO_SCRAPERS; i++)
	{
		SceneNode* r = new SceneNode(building1, Vector4(1, 1, 1, 1));
		r->SetModelScale(Vector3(100.0f, rand() % (int)500 + 900, rand() % (int)50 + 100.0f));
		r->SetTransform(Matrix4::Translation(heightmapSize * Vector3(-0.4, 0.6, -1 + 0.05 * i)));

		r->SetTexture(walltexture[rand() % 3]);
		r->SetBumpTexture(wallBump1);
		r->SetShader(lightShader);
		r->SetBoundingRadius(10000.0f);
		root->AddChild(r);

	}

	root->AddChild(Scenery);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	applyCRT();
	applyBlur();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::applyCRT()
{

	BindShader(CRTprocessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(CRTprocessShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < 3; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(CRTprocessShader->GetProgram(), "isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();
		 //Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(CRTprocessShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}

}

void Renderer::applyBlur()
{
	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < 1; ++i)
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

void Renderer::DrawWater()
{
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(hSize * Vector3(1, 0.01, 0.5)) *Matrix4::Scale(hSize * 0.5f) *Matrix4::Rotation(90, Vector3(1, 0, 0));

	/*textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));*/

	UpdateShaderMatrices();
	SetShaderLight(*lights);
	quad->Draw();
}
