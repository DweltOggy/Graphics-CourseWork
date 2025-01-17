#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../NCLGL/Camera.h"
#include "../NCLGL/SceneNode.h"
#include "../NCLGL/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "Soldier.h"

const int POST_PASSES = 3;
const int NO_SCRAPERS = 60;
const int NO_ART = 25;
const int LIGHTS_NUM = 5;

class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer	
{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	void RenderScene()				override;
	void UpdateScene(float msec)	override;
	
	void setCRT() { CRT = !CRT; };
	void setGrey() { grey = !grey; };
	void setSBL() { SBL = !SBL; };
	void setGamma() { gamma = !gamma; };
	void setBlur() { blur = !blur; };
	void toggleFreeCam() { freeCam = !freeCam; };
	void switchCam();
	void setFreeCam();

protected:

	void DrawSoldier();
	void placeTerrain();
	void DrawSkybox(); 
	void DrawWater();
	
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	void applyCRT();
	void applySBL();
	void applyBlur();
	void applyGrey();
	void applyGamma();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();

	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void moveCamera(float dt);

	bool freeCam;
	bool CRT;
	bool SBL;
	bool grey;
	bool gamma;
	bool blur;

	int currentFrame;
	float frameTime;

	Vector3 heightmapSize;

	Shader* lightShader;
	Shader* bumpShader;
	Shader* sceneShader;
	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* processShader;	
	Shader* CRTprocessShader;
	Shader* SBLprocessShader;
	Shader* GreyScaleShader;
	Shader* wireFrameShader;
	Shader* gammaShader;
	Shader* skinningShader;

	Soldier* theSoldier;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* streetLamp;
	Mesh* building1;

	Light* lights;

	Camera*  currentCamera;

	Camera* camera;
	Camera* camera2;
	
	GLuint cubeMap;
	GLuint othercubeMap;

	GLuint earthTex;
	GLuint waterTex;
	GLuint earthBump;

	GLuint roadTexture;
	GLuint roadBump;

	GLuint walltexture[3];

	GLuint wallBump1;
	GLuint scanlines;

	GLuint bufferFBO;
	
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	
	GLuint testFBO;
	GLuint testColourTex[2];
	GLuint testDepthTex;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint texture;

	SceneNode* root;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

};
