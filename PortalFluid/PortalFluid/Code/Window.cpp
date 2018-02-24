#include <algorithm>
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>
#include "Window.h"

void windowSizeCallback(GLFWwindow *window, int width, int height);

void curserPosCallback(GLFWwindow *window, double xPos, double yPos);

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void charCallback(GLFWwindow *window, unsigned int codepoint);

const float Window::DEFAULT_FOV = 60.0f;
const float Window::NEAR_PLANE = 0.1f;
const float Window::FAR_PLANE = 500.0f;


std::shared_ptr<Window> Window::createWindow(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing)
{
	return std::shared_ptr<Window>(new Window(_title, _width, _height, _vsync, _antialiasing));
}

Window::~Window()
{
	glfwTerminate();
}

void Window::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(resolution.first, resolution.second, title.c_str(), NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(static_cast<int>(vsync));

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, windowSizeCallback);
	glfwSetCursorPosCallback(window, curserPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);

	glfwSetWindowUserPointer(window, this);
}

void Window::input()
{
	pressedChars.clear();

	mousePosDelta = (currentMousePos - previousMousePos);
	previousMousePos = currentMousePos;
}

void Window::update()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

unsigned int Window::getWidth() const
{
	return resolution.first;
}

unsigned int Window::getHeight() const
{
	return resolution.second;
}

glm::mat4 Window::getProjectionMatrix() const
{
	return projectionMatrix;
}

void Window::destroyWindow()
{
	glfwDestroyWindow(window);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::setTitle(const std::string &_title)
{
	title = _title;
	glfwSetWindowTitle(window, title.c_str());
}

void Window::setVsync(const bool & _vsync)
{
	if (vsync != _vsync)
	{
		vsync = _vsync;
		glfwSwapInterval(vsync);
	}
}

void Window::setShouldClose(const bool &_shouldClose)
{
	glfwSetWindowShouldClose(window, _shouldClose);
}

bool Window::isKeyPressed(const int &_key)
{
	return pressedKeys.find(_key) != pressedKeys.end();
}

bool Window::isMouseButtonPressed(const int &_button)
{
	return pressedMouseButtons.find(_button) != pressedMouseButtons.end();
}

glm::vec2 Window::getMousePosition()
{
	return currentMousePos;
}

glm::vec2 Window::getMousePositionDelta()
{
	return mousePosDelta;
}

Window::Window(const std::string &_title, const unsigned int &_width, const unsigned int &_height, const bool &_vsync, const unsigned int &_antialiasing)
	:title(_title), resolution(std::make_pair(_width, _height)), vsync(_vsync), antialiasing(_antialiasing)
{
	projectionMatrix = glm::perspective(glm::radians(DEFAULT_FOV), static_cast<float>(getWidth()) / static_cast<float>(getHeight()), NEAR_PLANE, FAR_PLANE);
}

// callback functions

void windowSizeCallback(GLFWwindow *_window, int _width, int _height)
{
	Window *window = static_cast<Window *>(glfwGetWindowUserPointer(_window));
	window->resolution.first = _width;
	window->resolution.second = _height;
}

void curserPosCallback(GLFWwindow *_window, double _xPos, double _yPos)
{
	Window *window = static_cast<Window *>(glfwGetWindowUserPointer(_window));
	window->currentMousePos.x = static_cast<float>(_xPos);
	window->currentMousePos.y = static_cast<float>(_yPos);
}

void mouseButtonCallback(GLFWwindow *_window, int _button, int _action, int _mods)
{
	Window *window = static_cast<Window *>(glfwGetWindowUserPointer(_window));
	if (_action == GLFW_RELEASE)
	{
		window->pressedMouseButtons.erase(_button);
	}
	else if (_action == GLFW_PRESS)
	{
		window->pressedMouseButtons.insert(_button);
	}
}

void keyCallback(GLFWwindow *_window, int _key, int _scancode, int _action, int _mods)
{
	Window *window = static_cast<Window *>(glfwGetWindowUserPointer(_window));
	if (_action == GLFW_RELEASE)
	{
		window->pressedKeys.erase(_key);
	}
	else if (_action == GLFW_PRESS)
	{
		window->pressedKeys.insert(_key);
	}
}

void charCallback(GLFWwindow *_window, unsigned int _codepoint)
{
	Window *window = static_cast<Window *>(glfwGetWindowUserPointer(_window));
	window->pressedKeys.insert(_codepoint);
}
