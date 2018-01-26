#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <chrono>
#include <cassert>
#include <glm\detail\func_trigonometric.hpp>
#include "Window.h"
#include "Particle.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include <glm\gtx\string_cast.hpp>
#include <glm\gtx\transform.hpp>

void glErrorCheck(const std::string &_message);
void gameLoop();
void input(const double &_deltaTime);
void update(const double &_currentTime, const double &_deltaTime);
void render();
bool initializeOpenGL();

typedef std::chrono::duration<double> DoubleDuration;

const size_t MAX_PARTICLES = 10;
std::shared_ptr<Window> window;
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat());
ParticleEmitter particleEmitter(MAX_PARTICLES, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -0.1f, 0.0f), glm::radians(10.0f));
GLuint particleVAO;
GLuint particleVBO;
GLuint floorVAO;
GLuint floorVBO;
GLuint floorEBO;
std::shared_ptr<ShaderProgram> particleShader;
std::shared_ptr<ShaderProgram> floorShader;
GLint uViewProjection;
GLint uViewPortSize;
GLint uModelViewProjection;


int main()
{
	camera.rotate(glm::vec3(glm::radians(0.0f), 0.0f, 0.0f));
	window = Window::createWindow("Portal Fluid", 800, 600, true, 0);
	window->init();
	initializeOpenGL();

	particleShader = ShaderProgram::createShaderProgram("Resources/Shaders/particle.vert", "Resources/Shaders/particle.frag");
	floorShader = ShaderProgram::createShaderProgram("Resources/Shaders/floor.vert", "Resources/Shaders/floor.frag");
	uViewProjection = particleShader->createUniform("uViewProjection");
	uViewPortSize = particleShader->createUniform("uViewPortSize");
	uModelViewProjection = floorShader->createUniform("uModelViewProjection");

	// particle VAO/VBO
	{
		{
			// create buffers/arrays
			glGenVertexArrays(1, &particleVAO);
			glGenBuffers(1, &particleVBO);

			glBindVertexArray(particleVAO);

			glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
			glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3, NULL, GL_DYNAMIC_DRAW);

			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3, (void*)0);
		}
	}
	
	// floor VAO/VBO
	{
		{
			float positions[] =
			{
				-1.0f, 0.0f, -1.0f,
				-1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, -1.0f,
				1.0f, 0.0f, -1.0f,
				-1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f,
			};

			// create buffers/arrays
			glGenVertexArrays(1, &floorVAO);
			glGenBuffers(1, &floorVBO);
			glGenBuffers(1, &floorEBO);

			glBindVertexArray(floorVAO);

			glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);

			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3, (void*)0);
		}
	}

	

	gameLoop();
	return 0;
}

void gameLoop()
{
	//make sure we can use this clock
	assert(std::chrono::high_resolution_clock::is_steady);
	static double currentTime;
	static double lastFrame;

	currentTime = lastFrame = std::chrono::duration_cast<DoubleDuration>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	while (!window->shouldClose())
	{
		glErrorCheck("loop begin");
		currentTime = std::chrono::duration_cast<DoubleDuration>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		double delta = currentTime - lastFrame;

		input(delta);
		update(delta, currentTime);
		render();
	}
}

void input(const double &_deltaTime)
{
	window->input();
	if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		const glm::vec2 mouseDelta = window->getMousePositionDelta();
		camera.rotate(glm::vec3(mouseDelta.y * 0.002f, mouseDelta.x * 0.002f, 0.0));
	}

	glm::vec3 cameraTranslation;
	bool pressed = false;
	if (window->isKeyPressed(GLFW_KEY_W))
	{
		cameraTranslation.z = -0.1f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_S))
	{
		cameraTranslation.z = 0.1f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_A))
	{
		cameraTranslation.x = -0.1f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_D))
	{
		cameraTranslation.x = 0.1f * _deltaTime;
		pressed = true;
	}
	if (pressed)
	{
		camera.translate(cameraTranslation);
	}
}

void update(const double &_currentTime, const double &_deltaTime)
{
	particleEmitter.update(_currentTime, _deltaTime);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	// render floor
	glBindVertexArray(floorVAO);
	glEnableVertexAttribArray(0);

	floorShader->bind();
	floorShader->setUniform(uModelViewProjection, window->getProjectionMatrix() * camera.getViewMatrix() * glm::scale(glm::vec3(100.0f)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// render particles
	std::vector<Particle> &particles = particleEmitter.getParticles();
	if (!particles.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * 3, &particles[0]);

		glBindVertexArray(particleVAO);
		glEnableVertexAttribArray(0);

		particleShader->bind();
		particleShader->setUniform(uViewProjection, window->getProjectionMatrix() * camera.getViewMatrix());
		particleShader->setUniform(uViewPortSize, glm::vec2(window->getWidth(), window->getHeight()));

		glDrawArrays(GL_POINTS, 0, particles.size());
	}
	window->update();
}

bool initializeOpenGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// seamless cubemap interpolation
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDisable(GL_CULL_FACE);

	//glEnable(GL_MULTISAMPLE);

	glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.2f, 0.5f, 1.0f);

	glPointSize(10.0f);

	return true;
}



void glErrorCheck(const std::string &_message)
{
	switch (glGetError())
	{
	case (GL_INVALID_ENUM):
		std::cout << _message << std::endl;
		std::cout << "GL_INVALID_ENUM" << std::endl;
		break;
	case (GL_INVALID_VALUE):
		std::cout << _message << std::endl;
		std::cout << "GL_INVALID_VALUE" << std::endl;
		break;
	case (GL_INVALID_OPERATION):
		std::cout << _message << std::endl;
		std::cout << "GL_INVALID_OPERATION" << std::endl;
		break;
	case (GL_OUT_OF_MEMORY):
		std::cout << _message << std::endl;
		std::cout << "GL_OUT_OF_MEMORY" << std::endl;
		break;
	case (GL_INVALID_FRAMEBUFFER_OPERATION):
		std::cout << _message << std::endl;
		std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
		break;
	}
}