#pragma once

#include "Matrix4.h"
#include "Matrix3.h"

class Camera
{
public:
	Camera(void) 
	{
		yaw = 0.0f;
		pitch = 0.0f;
		velocity = 8.0f;
	};
	
	Camera(float pitch, float yaw, Vector3 position) 
	{
		this -> pitch = pitch;
		this -> yaw = yaw;
		this -> position = position;
		velocity = 8.0f;
		
	}
	
	~Camera(void) {};
	
	void UpdateCamera(float dt = 1.0f);

	void ForwardCamera(float dt);
	void UpCamera(float dt);
	
	void DownCamera(float dt);
	void BackwardCamera(float dt);
	void RotateCamera(float dt);
	
	Matrix4 BuildViewMatrix();
	
	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	
	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }
	
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }
	
 protected:
	 float yaw;
	 float pitch;
	 float velocity;
	 Vector3 position;

};

