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

	static std::shared_ptr<Window> createWindow(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing);
	Window(const Window &) = delete;
	Window &operator= (const Window &) = delete;
	~Window();
	void init();
	void input();
	void update();
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	glm::mat4 getProjectionMatrix() const;
	void destroyWindow();
	bool shouldClose();
	void setTitle(const std::string &_title);
	void setVsync(const bool &_vsync);
	void setShouldClose(const bool &_shouldClose);
	bool isKeyPressed(const int &_key);
	bool isMouseButtonPressed(const int &_button);
	glm::vec2 getMousePosition();
	glm::vec2 getMousePositionDelta();

private:
	std::string title;
	std::pair<unsigned int, unsigned int> resolution;
	bool vsync;
	unsigned int antialiasing;
	GLFWwindow *window = nullptr;
	glm::mat4 projectionMatrix;
	glm::vec2 previousMousePos;
	glm::vec2 currentMousePos;
	glm::vec2 mousePosDelta;
	std::set<int> pressedKeys;
	std::set<int> pressedMouseButtons;
	std::set<int> pressedChars;


	explicit Window(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing);
};