#include "glfwWindow/Context.h"

#include "Renderer.h"

int main(int argc, char **argv) 
{
	Renderer renderer;

	if (renderer.Init())
		renderer.Run();

	renderer.Destroy();
}
