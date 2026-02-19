#include "Renderer.h"

int main(int argc, char **argv) 
{
	Renderer renderer;

	if (renderer.Init() == EXIT_SUCCESS)
		renderer.Run();

	renderer.Destroy();
}
