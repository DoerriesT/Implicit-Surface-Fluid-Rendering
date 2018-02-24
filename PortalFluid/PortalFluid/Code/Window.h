#pragma once
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>
#include <memory>
#include <glm\mat4x4.hpp>
#include <glm\vec2.hpp>
#include <set>

class Window
{
	friend void windowSizeCallback(GLFWwindow *window, int width, int height);
	friend void curserPosCallback(GLFWwindow *window, double xPos, double yPos);
	friend void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	friend void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	friend void charCallback(GLFWwindow *window, unsigned int codepoint);

public:
	static const float DEFAULT_FOV;
	static const float NEAR_PLANE;
	static const float FAR_PLANE;

	/*
	 * Returns a shared_ptr to a new Window instance
	 */
	static std::shared_ptr<Window> createWindow(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing);

	/*
	 *	copy constructor and copy assignment are deleted functions;
	 *	new instances of Window my only be created through createWindow
	 */
	Window(const Window &) = delete;
	Window &operator= (const Window &) = delete;

	/*
	 * Destructor
	 */
	~Window();

	/*
	 * Initializes and creates window/glfw library
	 */
	void init();

	/*
	 * Updates user input state. Should be called every frame
	 */
	void input();

	/*
	 * Swaps buffers and polls user input
	 */
	void update();

	/*
	 * Returns window width
	 */
	unsigned int getWidth() const;

	/*
	 * Returns window height
	 */
	unsigned int getHeight() const;

	/*
	 * Returns the projection matrix
	 */
	glm::mat4 getProjectionMatrix() const;

	/*
	 * Destroys the window
	 */
	void destroyWindow();

	/*
	 * Returns a bool indicating wether the user tried to close the window
	 */
	bool shouldClose();

	/*
	 * Sets the window title to the given string
	 */
	void setTitle(const std::string &_title);

	/*
	 * Enables or disables v-sync
	 */
	void setVsync(const bool &_vsync);

	/*
	 * Sets the "should close" flag, indicating that the window should be closed
	 */
	void setShouldClose(const bool &_shouldClose);

	/*
	 * Returns a bool indicating wether the given key is currently pressed
	 */
	bool isKeyPressed(const int &_key);

	/*
	 * Returns a bool indicating wether the given mouse button is currently pressed
	 */
	bool isMouseButtonPressed(const int &_button);

	/*
	 * Returns the current mouse position
	 */
	glm::vec2 getMousePosition();

	/*
	 * Returns the mouse position delta to the last frame
	 */
	glm::vec2 getMousePositionDelta();

private:
	// window title
	std::string title;
	// current window resolution
	std::pair<unsigned int, unsigned int> resolution;
	// wether vsync is enabled or not
	bool vsync;
	// antialiasing of the default framebuffer
	unsigned int antialiasing;
	// pointer to glfw window instance
	GLFWwindow *window = nullptr;
	// projection matrix
	glm::mat4 projectionMatrix;
	// mouse position of the last frame
	glm::vec2 previousMousePos;
	// current mouse position
	glm::vec2 currentMousePos;
	// mouse position delta to the last frame
	glm::vec2 mousePosDelta;
	// set of currently pressed keys (these are added and removed by callbacks to the underlying glfw api)
	std::set<int> pressedKeys;
	// set of currently pressed mouse buttons (these are added and removed by callbacks to the underlying glfw api)
	std::set<int> pressedMouseButtons;
	// set of currently pressed character keys (these are added and removed every frame)
	std::set<int> pressedChars;

	/*
	 * Constructs a new Window instance with the given title, resolution, vertical sync and backbuffer antialiasing
	 */
	explicit Window(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing);
};