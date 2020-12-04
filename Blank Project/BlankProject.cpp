#include "../NCLGL/window.h"
#include "Renderer.h"
#include <time.h>

int main()	{
	Window w("Make your own project!", 1920, 1080, false);
	srand(time(NULL));
	if(!w.HasInitialised()) 
	{
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) 
	{
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) 
		{
			Shader::ReloadAllShaders();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
		{
			renderer.setCRT();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
		{
			renderer.setSBL();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
		{
			renderer.setGrey();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_U))
		{
			renderer.setGamma();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
		{
			renderer.setBlur();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_N))
		{
			renderer.setFreeCam();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_M))
		{
			renderer.switchCam();
		}
	}
	return 0;
}