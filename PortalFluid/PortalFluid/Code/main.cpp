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

const size_t MAX_PARTICLES = 20;
std::shared_ptr<Window> window;
Camera camera(glm::vec3(0.0f, 50.0f, 50.0f), glm::quat());
ParticleEmitter particleEmitter(MAX_PARTICLES, glm::vec3(-25.0f, 25.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -3.0f, 0.0f), glm::radians(5.0f), 1.0f);
GLuint particleVAO;
GLuint particleVBO;
GLuint floorVAO;
GLuint floorVBO;
GLuint floorEBO;
std::shared_ptr<ShaderProgram> particlePointsShader;
std::shared_ptr<ShaderProgram> particleQuadsShader;
std::shared_ptr<ShaderProgram> floorShader;

// point particle shader uniforms
GLint uViewProjectionParticlePoints;
GLint uViewPortSizeParticlePoints;
GLint uPositionParticlePoints;
GLint uViewParticlePoints;
GLint uProjectionParticlePoints;
GLint uModeParticlePoints;

// quad particle shader uniforms
GLint uViewProjectionParticleQuads;
GLint uViewPortSizeParticleQuads;
GLint uPositionParticleQuads;
GLint uViewParticleQuads;
GLint uProjectionParticleQuads;
GLint uModeParticleQuads;

// floor shader uniform
GLint uModelViewProjectionFloor;

// mode
int mode = 1;


int main()
{
	camera.rotate(glm::vec3(glm::radians(45.0f), 0.0f, 0.0f));
	window = Window::createWindow("Portal Fluid", 800, 600, true, 0);
	window->init();
	initializeOpenGL();

	particlePointsShader = ShaderProgram::createShaderProgram("Resources/Shaders/particle.vert", "Resources/Shaders/particle.frag");
	particleQuadsShader = ShaderProgram::createShaderProgram("Resources/Shaders/particle.vert", "Resources/Shaders/particle.frag", "Resources/Shaders/particle.geom");
	floorShader = ShaderProgram::createShaderProgram("Resources/Shaders/floor.vert", "Resources/Shaders/floor.frag");

	uViewPortSizeParticlePoints = particlePointsShader->createUniform("uViewPortSize");
	uPositionParticlePoints = particlePointsShader->createUniform("uPosition");
	uViewParticlePoints = particlePointsShader->createUniform("uView");
	uProjectionParticlePoints = particlePointsShader->createUniform("uProjection");
	uModeParticlePoints = particlePointsShader->createUniform("uMode");

	uViewPortSizeParticleQuads = particleQuadsShader->createUniform("uViewPortSize");
	uPositionParticleQuads = particleQuadsShader->createUniform("uPosition");
	uViewParticleQuads = particleQuadsShader->createUniform("uView");
	uProjectionParticleQuads = particleQuadsShader->createUniform("uProjection");
	uModeParticleQuads = particleQuadsShader->createUniform("uMode");

	uModelViewProjectionFloor = floorShader->createUniform("uModelViewProjection");

	// particle VAO/VBO
	{
		{
			// create buffers/arrays
			glGenVertexArrays(1, &particleVAO);
			glGenBuffers(1, &particleVBO);

			glBindVertexArray(particleVAO);

			glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
			glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * 4, NULL, GL_DYNAMIC_DRAW);

			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3, (void*)0);
		}
	}
	
	// floor VAO/VBO
	{
		{
			float vertices[] = {
				0.5f, 0.0f,  0.5f,  // top right
				0.5f, 0.0f, -0.5f,  // bottom right
				-0.5f, 0.0f, -0.5f,  // bottom left
				-0.5f, 0.0f,  0.5f   // top left 
			};
			unsigned int indices[] = {  // note that we start from 0!
				0, 1, 3,  // first Triangle
				1, 2, 3   // second Triangle
			};

			glGenVertexArrays(1, &floorVAO);
			glGenBuffers(1, &floorVBO);
			glGenBuffers(1, &floorEBO);
			// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
			glBindVertexArray(floorVAO);

			glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0);
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

	currentTime = lastFrame = glfwGetTime();

	while (!window->shouldClose())
	{
		glErrorCheck("loop begin");
		currentTime = glfwGetTime();
		double delta = currentTime - lastFrame;

		input(delta);
		update(currentTime, delta);
		render();

		lastFrame = currentTime;
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
		cameraTranslation.z = -10.0f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_S))
	{
		cameraTranslation.z = 10.0f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_A))
	{
		cameraTranslation.x = -10.0f * _deltaTime;
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_D))
	{
		cameraTranslation.x = 10.0f * _deltaTime;
		pressed = true;
	}
	if (pressed)
	{
		camera.translate(cameraTranslation);
	}

	if (window->isKeyPressed(GLFW_KEY_1))
	{
		mode = 0;
	}
	else if (window->isKeyPressed(GLFW_KEY_2))
	{
		mode = 1;
	}
	else if (window->isKeyPressed(GLFW_KEY_3))
	{
		mode = 2;
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
	floorShader->setUniform(uModelViewProjectionFloor, window->getProjectionMatrix() * camera.getViewMatrix() * glm::scale(glm::vec3(50.0f)));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	// render particles
	std::vector<Particle *> &particles = particleEmitter.getParticles();
	if (!particles.empty())
	{
		/*std::vector<glm::vec3> positions;
		for (Particle *particle : particles)
		{
			positions.push_back(particle->position);
		}

		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), &positions[0]);
		glErrorCheck("glBufferSubData");*/

		glBindVertexArray(particleVAO);
		glEnableVertexAttribArray(0);

		if (mode == 0)
		{
			particlePointsShader->bind();
			particlePointsShader->setUniform(uViewPortSizeParticlePoints, glm::vec2(window->getWidth(), window->getHeight()));
			particlePointsShader->setUniform(uModeParticlePoints, 0);
			particlePointsShader->setUniform(uViewParticlePoints, camera.getViewMatrix());
			particlePointsShader->setUniform(uProjectionParticlePoints, window->getProjectionMatrix());
			for (Particle *particle : particleEmitter.getParticles())
			{
				particlePointsShader->setUniform(uPositionParticlePoints, glm::vec4(particle->position, 1.0));

				glDrawArrays(GL_POINTS, 0, 1);
				glErrorCheck("glDrawArrays");
			}
		}
		else
		{
			particleQuadsShader->bind();
			particleQuadsShader->setUniform(uViewPortSizeParticleQuads, glm::vec2(window->getWidth(), window->getHeight()));
			particleQuadsShader->setUniform(uModeParticleQuads, mode);
			particleQuadsShader->setUniform(uViewParticleQuads, camera.getViewMatrix());
			particleQuadsShader->setUniform(uProjectionParticleQuads, window->getProjectionMatrix());
			for (Particle *particle : particleEmitter.getParticles())
			{
				particleQuadsShader->setUniform(uPositionParticleQuads, glm::vec4(particle->position, 1.0));

				glDrawArrays(GL_POINTS, 0, 1);
				glErrorCheck("glDrawArrays");
			}
		}
		
		//glDrawArrays(GL_POINTS, 0, positions.size());
		glErrorCheck("glDrawArrays");
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