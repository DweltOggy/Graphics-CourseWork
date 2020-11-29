#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../NCLGL/Camera.h"
#include "../NCLGL/SceneNode.h"
#include "../NCLGL/Frustum.h"
#include "../nclgl/HeightMap.h"

class Renderer : public OGLRenderer	
{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;
	


protected:
	void placeTerrain();
	void DrawSkybox(); 

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();

	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	Shader* lightShader;
	Shader* sceneShader;
	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* processShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* building1;
	Mesh* building2;
	Mesh* building3;
	Mesh* building4;
	Mesh* building5;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	GLuint earthTex;
	GLuint waterTex;
	GLuint earthBump;

	GLuint roadTexture;
	GLuint roadBump;

	GLuint pavedTexture;

	GLuint walltexture1;
	GLuint walltexture2;
	GLuint walltexture3;
	GLuint walltexture4;
	GLuint walltexture5;

	GLuint wallBump1;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	GLuint texture;

	SceneNode* root;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

};
