#pragma once
#include "../nclgl/OGLRenderer.h"
class Mesh;
class Camera;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	void UpdateScene(float dt) override;
	
protected:
	Mesh * heightMap;
	Shader * shader;
	Camera * camera;
	GLuint terrainTex;
};
