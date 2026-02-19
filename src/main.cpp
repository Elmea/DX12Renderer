#include "glfwWindow/Context.h"

int main(int argc, char **argv) 
{
	Context context;

	context.Init(1200, 900, "DX12Renderer");

	while (!context.ShouldClose())
	{
		context.StartFrame();
	}

	context.Destroy();
}
