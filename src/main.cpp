#include "Renderer.h"

int main(int argc, char **argv) 
{
	Renderer renderer;

	if (renderer.Init() == 0)
		renderer.Run();

	renderer.Destroy();
}
