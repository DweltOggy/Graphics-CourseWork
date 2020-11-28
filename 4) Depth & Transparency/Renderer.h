#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer :public OGLRenderer
{
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	
	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

	virtual void UpdateScene(float dt);
	
 protected:
	GLuint textures[2];
	Mesh * meshes[2];
	Shader * shader;
	Vector3 positions[2];

	Camera* camera;
	
	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;

};

