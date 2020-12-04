#include "Renderer.h"
#include "Artifact.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"


Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise12.png");

	building1 = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	streetLamp = Mesh::LoadFromMeshFile("Street Lamp.msh");

	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");

	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	CRTprocessShader = new Shader("TexturedVertex.glsl", "#COCRTProcess.glsl");

	SBLprocessShader = new Shader("TexturedVertex.glsl", "#COSBLProcess.glsl");

	GreyScaleShader = new Shader("TexturedVertex.glsl", "#COGRYProcess.glsl");

	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	 
	wireFrameShader = new Shader("#COwireVertex.glsl", "#COwireFrag.glsl");

	gammaShader = new Shader("TexturedVertex.glsl", "#COGMRProcess.glsl");

	skinningShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");


	walltexture[0] = SOIL_load_OGL_texture(TEXTUREDIR "scraper.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	walltexture[1] = SOIL_load_OGL_texture(TEXTUREDIR "scraper2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	walltexture[2] = SOIL_load_OGL_texture(TEXTUREDIR "scraper4.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	blockadeTex = SOIL_load_OGL_texture(TEXTUREDIR "Diamond_Plate_ALBEDO.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);


	wallBump1 = SOIL_load_OGL_texture(
		TEXTUREDIR "ScraperBump2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR "scraper3.jpg", SOIL_LOAD_AUTO,
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
	
	scanlines = SOIL_load_OGL_texture(
		TEXTUREDIR "scanlines10.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

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
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);

	

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

	glGenFramebuffers(1, &testFBO);

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

	freeCam = false;
	CRT = false;
	SBL = false;
	grey = false;
	gamma = false;
	blur = false;

	currentFrame = 0;
	frameTime = 0.0f;

	init = true;
}

Renderer::~Renderer(void)	
{
	delete camera;
	delete camera2;

	delete quad;
	delete streetLamp;
	delete building1;

	delete skyboxShader;
	delete lightShader;
	delete reflectShader;
	delete sceneShader;
	delete processShader;
	delete CRTprocessShader;
	delete SBLprocessShader;
	delete GreyScaleShader;
	delete wireFrameShader;
	delete gammaShader;
	delete skinningShader;

	delete theSoldier;

	delete lights;

	delete heightMap;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &testFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::UpdateScene(float dt) 
{
	if (freeCam)
		currentCamera->UpdateCamera(dt);
	else
		moveCamera(dt);

	viewMatrix = currentCamera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	theSoldier->update(dt);

	root->Update(dt);
}

void Renderer::moveCamera(float dt)
{
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	if (currentCamera->GetPosition().x < heightmapSize.x * 0.8 )
	{
		currentCamera->ForwardCamera(dt);
	}	
	else if (currentCamera->GetPosition().y < heightmapSize.y * 2.0 )
	{
		currentCamera->RotateCamera(dt);
		currentCamera->UpCamera(dt);
	}

}

void Renderer::setFreeCam()
{
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	toggleFreeCam();
	if (!freeCam)
	{
		currentCamera->SetPosition(heightmapSize * Vector3(0.05, 0.5, 0.5));
		currentCamera->SetYaw(-90.0f);
		currentCamera->SetPitch(0);
	}
	
}

void Renderer::switchCam()
{
	freeCam = true;
	if (currentCamera == camera)
	{
		currentCamera = camera2;
	}
	else if (currentCamera == camera2)
	{
		currentCamera = camera;
	}
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
	DrawSoldier();
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

void  Renderer::DrawSoldier()
{
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(skinningShader);
	glUniform1i(glGetUniformLocation(skinningShader->GetProgram(), "diffuseTex"), 0);

	modelMatrix =
		Matrix4::Translation(heightmapSize * theSoldier->pos)
		* Matrix4::Scale(heightmapSize  * 100.0) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	UpdateShaderMatrices();
	vector <Matrix4> frameMatrices;

	const Matrix4* invBindPose = theSoldier->mesh->GetInverseBindPose();
	const Matrix4* frameData = theSoldier->anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < theSoldier->mesh->GetJointCount(); ++i)
	{
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(skinningShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	for (int i = 0; i < theSoldier->mesh->GetSubMeshCount(); ++i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, theSoldier->matTextures[i]);
		theSoldier->mesh->DrawSubMesh(i);
	}
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
				n->GetShader()->GetProgram(), "perfect"), true);

			glUniform1i(glGetUniformLocation(
				n->GetShader()->GetProgram(), "cubeTex"), 2);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, othercubeMap);


			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
				"cameraPos"), 1, (float*)&currentCamera->GetPosition());

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "modelMatrix"), 
																				1, false, model.values);
			//UpdateShaderMatrices();
			SetShaderLight(*lights);

			n->Draw(*this);
		}
		else if(n->GetShader() == lightShader)
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

			glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
				"cameraPos"), 1, (float*)&currentCamera->GetPosition());

			//modelMatrix.ToIdentity();
			//textureMatrix.ToIdentity();

			n->Draw(*this);

		}
		else //if (n->GetShader() == wireFrameShader)
		{
			glEnable(GL_LINE_SMOOTH);
			n->GetColour();
			n->Draw(*this);
		}

	}

}

void Renderer::placeTerrain()
{

	root = new SceneNode();
	root->SetShader(NULL);

	SceneNode* Scenery = new SceneNode(heightMap, Vector4(1, 0, 1, 0.8));
	heightmapSize = heightMap->GetHeightmapSize();
	float terrainsize = heightMap->GetHeightmapSize().Length() * 5.0f;
	Scenery->SetShader(wireFrameShader);
	Scenery->SetBoundingRadius(terrainsize);

	theSoldier = new Soldier();
	theSoldier->setPos(heightmapSize * Vector3(0.5, 0.5, 0.5));

	camera = new Camera(0, -90.0f, heightmapSize * Vector3(0.05, 0.5, 0.5));

	camera2 = new Camera(0, 65.0f, heightmapSize * Vector3(0.8, 2.0, 0.9));

	currentCamera = camera;

	lights = new Light(heightmapSize * Vector3(0.9, 1.5, 0.5), Vector4(1, 0.75, 1, 1), 10000.0f);

	SceneNode* portal = new SceneNode(building1, Vector4(1, 1, 1, 1));
	portal->SetModelScale(Vector3(160.0f, 120.0f, 3.0f));

	portal->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.94, 0.05, 0.5)));
	portal->SetTransform(portal->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
	portal->SetTexture(waterTex);
	portal->SetBumpTexture(earthBump);
	portal->SetShader(reflectShader);
	portal->SetBoundingRadius(10000.0f);

	root->AddChild(portal);

	SceneNode* tunnel = new SceneNode(building1, Vector4(1, 1, 1, 1));
	tunnel->SetModelScale(Vector3(200.0f, 175.0f, 50.0f));

	tunnel->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.01, 0.05, 0.5)));
	tunnel->SetTransform(tunnel->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
	tunnel->SetTexture(waterTex);
	tunnel->SetBumpTexture(earthBump);
	tunnel->SetShader(reflectShader);
	tunnel->SetBoundingRadius(10000.0f);

	root->AddChild(tunnel);
	//streetLamp
//road
	for (int i = 0; i < 11; i++)
	{
		SceneNode* s = new SceneNode(building1, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(200.0f, 300.0f, 10.0f));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(heightmapSize * Vector3(-0.5, 0.07, 0 + 0.09 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(roadTexture);
		s->SetBumpTexture(roadBump);
		s->SetShader(lightShader);
		s->SetBoundingRadius(5000.0f);
		Scenery->AddChild(s);
	}

	for (int i = 0; i < 11; i++)
	{
		SceneNode* s = new SceneNode(streetLamp, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(0.25f, 0.25f, 0.18f));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(heightmapSize * Vector3(-0.47, 0.07, 0 + 0.09 * i)));
		//s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(earthTex);
		s->SetBumpTexture(earthBump);
		s->SetShader(lightShader);
		s->SetBoundingRadius(5000.0f);
		Scenery->AddChild(s);
	}

	for (int i = 0; i < 11; i++)
	{
		SceneNode* s = new SceneNode(streetLamp, Vector4(1, 1, 1, 1));
		s->SetModelScale(Vector3(0.25f, 0.25f, 0.18f));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(0.0, 1.0, 0.0)));
		s->SetTransform(s->GetTransform() * Matrix4::Translation(heightmapSize * Vector3(-0.53, 0.07, 0 + 0.09 * i)));
		s->SetTransform(s->GetTransform() * Matrix4::Rotation(180.0f, Vector3(0.0, 1.0, 0.0)));
		//s->SetTransform(s->GetTransform() * Matrix4::Rotation(90.0f, Vector3(1.0, 0.0, 0.0)));
		s->SetTexture(earthTex);
		s->SetBumpTexture(earthBump);
		s->SetShader(lightShader);
		s->SetBoundingRadius(5000.0f);
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

	for (int i = 0; i < NO_ART; i++)
	{
		Artifact* s = new Artifact();
		float x = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0.1)));
		float y = 1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (8.0 - 0.5)));
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

	if (grey)
		applyGrey();
	
	if (SBL)
		applySBL();
	
	if (CRT)
	{
		applyCRT();
		applyBlur();
	}

	if (gamma)
	{
		applyGamma();
	}

	if (blur)
	{
		applyBlur();
	}

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

	glUniform1i(glGetUniformLocation(CRTprocessShader->GetProgram(), "mixTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, scanlines);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(CRTprocessShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < 4; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}

}

void Renderer::applySBL()
{

	BindShader(SBLprocessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(SBLprocessShader->GetProgram(), "sceneTex"), 0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glUniform1i(glGetUniformLocation(SBLprocessShader->GetProgram(), "isVertical"), 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	quad->Draw();
 
	glUniform1i(glGetUniformLocation(SBLprocessShader->GetProgram(), "isVertical"), 1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, bufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
	quad->Draw();

}

void Renderer::applyGrey()
{
	BindShader(GreyScaleShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(GreyScaleShader->GetProgram(), "sceneTex"), 0);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, bufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);

	quad->Draw();

}

void Renderer::applyGamma()
{
	BindShader(gammaShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(gammaShader->GetProgram(), "sceneTex"), 0);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);

	quad->Draw();

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
		"cameraPos"), 1, (float*)&currentCamera->GetPosition());

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "perfect"), false);

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(hSize * Vector3(1, 0.01, 0.5)) 
		*Matrix4::Scale(hSize * 0.5f) *Matrix4::Rotation(90, Vector3(1, 0, 0));

	UpdateShaderMatrices();
	SetShaderLight(*lights);
	quad->Draw();
}


