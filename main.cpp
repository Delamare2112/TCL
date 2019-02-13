#include <iostream>
#include <cstring>
#include <GLFW/glfw3.h>

struct Pixel {
	unsigned int r,g,b;
};

struct Settings {
	int screen_width = 1920;
	int screen_height = 1080;
	int pixel_size = 10;
	bool full_screen = false;
};

void error_callback(int error, const char* description)
{
    puts(description);
}

Settings parse_arguments(int argc, char** argv) {
	Settings ret;
	for (int i = 0; i < argc; i++) {
		const auto arg = argv[i];
		if (strcmp(arg, "--height") == 0) {
			ret.screen_height = atoi(argv[i+1]);
		}
		else if (strcmp(arg, "--width") == 0) {
			ret.screen_width = atoi(argv[i+1]);
		}
		else if (strcmp(arg, "--px") == 0) {
			ret.pixel_size = atoi(argv[i+1]);
		}
		else if (strcmp(arg, "--fs")) {
			ret.full_screen = true;
		}
	}
	return ret;
}

void main_loop(const Settings& settings, GLFWwindow*const window) {
	const auto canvas_width = settings.screen_width / settings.pixel_size;
	const auto canvas_height = settings.screen_height / settings.pixel_size;

	glPixelZoom(settings.pixel_size,settings.pixel_size);
	
	while (!glfwWindowShouldClose(window)) {
		glClearColor( 0, 0, 0, 1 );
		glClear(GL_COLOR_BUFFER_BIT);

		Pixel data[canvas_height][canvas_width];
		for (auto& _p : data) for (Pixel& p : _p) {
			p.r = ( rand() % 256 ) * 256 * 256 * 256;
			p.g = ( rand() % 256 ) * 256 * 256 * 256;
			p.b = ( rand() % 256 ) * 256 * 256 * 256;
		}

	    glDrawPixels( canvas_width, canvas_height, GL_RGB, GL_UNSIGNED_INT, data );
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char** argv) {
	Settings settings = parse_arguments(argc, argv);

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		std::cerr << "Failed to init GLFW!\n";
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(settings.screen_width, settings.screen_height, "Conway's Game Of Life", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to glfwCreateWindow!\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	main_loop(settings, window);

	glfwTerminate();
	return 0;
}
