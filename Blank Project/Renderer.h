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
	void DrawWater();

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
	Shader* shadowShader;
	//Shader* wireFrameShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* cube;

	Mesh* building1;


	/*Mesh* building2;
	Mesh* building3;
	Mesh* building4;
	Mesh* building5;*/

	Light* lights;
	Camera* camera;
	//Camera* camera2;
	//Camera* camera3;
	//Camera* camera4;

	GLuint cubeMap;

	GLuint earthTex;
	GLuint waterTex;
	GLuint earthBump;

	GLuint roadTexture;
	GLuint roadBump;

	GLuint pavedTexture;

	GLuint walltexture[3];
	GLuint blockadeTex;

	GLuint wallBump1;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint sceneDiffuse;
	GLuint sceneBump;
	float sceneTime;



	GLuint texture;

	SceneNode* root;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

};
