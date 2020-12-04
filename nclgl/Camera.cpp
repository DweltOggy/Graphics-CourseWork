#include "Camera.h"
#include "Window.h"



void Camera::UpdateCamera(float dt)
{
	pitch -= Window::GetMouse() -> GetRelativePosition().y;
	yaw -= Window::GetMouse() -> GetRelativePosition().x;
	
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
	
	if (yaw < 0) 
		yaw += 360.0f;
	

	if (yaw > 360.0f) 
		yaw -= 360.0f;
	

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	

	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_W)) 
		 position += forward * velocity;
	
	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_S)) 
		 position -= forward * velocity;

	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_A)) 
		 position -= right * velocity;
	
	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_D)) 
		 position += right * velocity;

	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_SHIFT)) 
		 position.y += velocity;
		
	if (Window::GetKeyboard() -> KeyDown(KEYBOARD_SPACE)) 
		 position.y -= velocity;
		
	
}

void Camera::ForwardCamera(float dt)
{
	velocity = 3.0;
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	position += forward * velocity;

	velocity = 8.0;
}

void Camera::BackwardCamera(float dt)
{
	velocity = 3.0;
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 backward = rotation * Vector3(0, 0, 1);
	position += backward * velocity;

	velocity = 8.0;
}

void Camera::UpCamera(float dt)
{
	velocity = 1.1;
	position.y += velocity;
	velocity = 8.0;
}

void Camera::DownCamera(float dt)
{
	velocity = 1.15;
	position.y -= velocity;
	velocity = 8.0;
}

void Camera::RotateCamera(float dt)
{
	yaw -= 0.52;
	//Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	//Vector3 forward = rotation * Vector3(0, 0, -1);
}
	


Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * 
		   Matrix4::Rotation(-yaw,   Vector3(0, 1, 0)) * 
		   Matrix4::Translation(-position);
}