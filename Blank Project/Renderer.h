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
	
	void setCRT() { CRT = !CRT; };
	void setFreeCam();

protected:
	void placeTerrain();
	void DrawSkybox(); 
	void DrawWater();

	void setTextures();
	void loadShaders();

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	void applyCRT();
	void applyBlur();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();

	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void moveCamera(float dt);

	bool freeCam;
	bool CRT;

	Shader* lightShader;
	Shader* sceneShader;
	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* processShader;	
	Shader* CRTprocessShader;
	//Shader* wireFrameShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* cube;

	Mesh* building1;

	Light* lights;

	Camera* camera;
	
	GLuint cubeMap;
	GLuint othercubeMap;

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
	//GLuint testFBO;

	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint texture;

	SceneNode* root;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

};
