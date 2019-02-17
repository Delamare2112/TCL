#include <iostream>
#include <cstring>
#include <array>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <GLFW/glfw3.h>

struct Pixel {
	unsigned int r,g,b;
	void Birth() {
		r = -1;
		g = -1;
		b = -1;
	}
	void Die() {
		r = 0;
		g = 0;
		b = 0;
	}
};

struct Settings {
	int screen_width = 1920;
	int screen_height = 1080;
	int pixel_size = 10;
	int step_time = 500;
	bool full_screen = false;
	char* seed = nullptr;
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
		else if (strcmp(arg, "--fs") == 0) {
			ret.full_screen = true;
		}
		else if (strcmp(arg, "--milli") == 0) {
			ret.step_time = atoi(argv[i+1]);
		}
		else if (strcmp(arg, "--seed") == 0) {
			ret.seed = argv[i+1];
		}
	}
	return ret;
}

class Canvas {
public:
	size_t width, height;
	Pixel* buffer1;
	Pixel* buffer2;
	Pixel* current;

	Canvas(size_t width, size_t height) {
		this->width = width;
		this->height = height;

		buffer1 = new Pixel[width * height];
		buffer2 = new Pixel[width * height];
		memset(buffer1, 0, width * height * sizeof(Pixel));
		memset(buffer2, 0, width * height * sizeof(Pixel));
		current = buffer1;

		for (size_t x = 0; x < width; x++)
		for (size_t y = 0; y < height; y++) {
			if (rand() % 5 == 0) {
				get(x,y).Birth();
			}
		}
	}

	~Canvas() {
		delete[] buffer1;
		delete[] buffer2;
	}

	Pixel& get(size_t x, size_t y, Pixel* data) const {
		return data[(y * width) + x];
	}
	Pixel& get(size_t x, size_t y) const {
		return get(x,y, current);
	}

	u_int8_t count_neightbors(size_t x, size_t y) {
		u_int8_t ret = 0;
		if (x + 1 < width && get(x+1,y).r == -1) {
			ret++;
		}
		if (x + 1 < width && y + 1 < height && get(x+1,y+1).r == -1) {
			ret++;
		}
		if (y + 1 < height && get(x,y+1).r == -1) {
			ret++;
		}
		if (y + 1 < height && x - 1 >= 0 && get(x-1,y+1).r == -1) {
			ret++;
		}
		if (x - 1 >= 0 && get(x-1,y).r == -1) {
			ret++;
		}
		if (x - 1 >= 0 && y - 1 >= 0 && get(x-1,y-1).r == -1) {
			ret++;
		}
		if (y - 1 >= 0 && get(x,y-1).r == -1) {
			ret++;
		}
		if (y - 1 >= 0 && x + 1 < width && get(x+1,y-1).r == -1) {
			ret++;
		}
		return ret;
	}

	void Step() {
		Pixel* other = current == buffer1 ? buffer2 : buffer1;
		memset(other, 0, width * height * sizeof(Pixel));

		for (auto x = 0; x < width; x++)
		for (auto y = 0; y < height; y++) {
			Pixel& cell = get(x,y);
			Pixel& new_cell = get(x,y,other);
			const bool alive = cell.r == -1;
			const u_int8_t neightbors = count_neightbors(x,y);
			if (alive) {
				if (neightbors == 3 || neightbors == 2) {
					new_cell = cell;
					// TOOD: new_cell.Age();
				}
			} else {
				if (neightbors == 3) {
					new_cell.Birth();
				}
			}
		}
		current = other;
	}
};

void main_loop(const Settings& settings, GLFWwindow*const window) {
	glPixelZoom(settings.pixel_size,settings.pixel_size);
	Canvas canvas(settings.screen_width / settings.pixel_size, settings.screen_height / settings.pixel_size);
	
	while (!glfwWindowShouldClose(window)) {
		glClearColor( 0, 0, 0, 1 );
		glClear(GL_COLOR_BUFFER_BIT);

		canvas.Step();

	    glDrawPixels( canvas.width, canvas.height, GL_RGB, GL_UNSIGNED_INT, canvas.current );
		glfwSwapBuffers(window);
		glfwPollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(settings.step_time));
	}
}

int main(int argc, char** argv) {
	Settings settings = parse_arguments(argc, argv);

	auto seed = 0;
	if (settings.seed == nullptr) {
		seed = time(0);
	} else {
		auto haba_hasher = std::unordered_map<std::string, unsigned int>().hash_function();
		seed = haba_hasher(settings.seed);
	}
	std::cout << "Seeds duncan: " << seed << '\n';
	srand(seed);

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
