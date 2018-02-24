#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <cassert>
#include <glm\detail\func_trigonometric.hpp>
#include "Window.h"
#include "Particle.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include <glm\gtx\string_cast.hpp>
#include <glm\gtx\transform.hpp>
#include "Texture.h"

enum class RenderMode
{
	POINTS, UV, LINEAR, EXPONENTIAL
};

enum class SubstanceMode
{
	WATER, GLASS, AIR_BUBBLES, SOAP_BUBBLES
};

enum class SimulationSpeed
{
	FREEZE, SLOW, NORMAL, FAST
};

void glErrorCheck(const std::string &_message);
void gameLoop();
void input(const double &_deltaTime);
void update(const double &_currentTime, const double &_deltaTime);
void render();
bool initializeOpenGL();

const size_t MAX_PARTICLES = 20;

std::shared_ptr<Window> window;

Camera camera(glm::vec3(0.0f, 50.0f, 50.0f), glm::vec3(glm::radians(45.0f), 0.0f, 0.0f));

// particle emitter
ParticleEmitter particleEmitter(MAX_PARTICLES, glm::vec3(-25.0f, 25.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -3.0f, 0.0f), glm::radians(15.0f), 1.0f);

// particles array/buffer
GLuint particleVAO;
GLuint particleVBO;

// shaders
std::shared_ptr<ShaderProgram> particlePointsShader;
std::shared_ptr<ShaderProgram> particleQuadsShader;
std::shared_ptr<ShaderProgram> skyboxShader;

// environment texture
std::shared_ptr<Texture> environmentTexture;

// point particle shader uniforms
GLint uViewPortSizePoints;
GLint uViewPoints;
GLint uProjectionPoints;
GLint uModePoints;

// quad particle shader uniforms
GLint uViewPortSizeQuads;
GLint uViewQuads;
GLint uProjectionQuads;
GLint uModeQuads;
GLint uParticlesQuads[MAX_PARTICLES];
GLint uNumParticlesQuads;
GLint uEnvironmentMapQuads;
GLint uInverseViewQuads;
GLint uSubstanceModeQuads;

// skybox shader uniforms
GLint uInverseModelViewProjectionSkybox;
GLint uEnvironmentMapSkybox;

// modes
RenderMode mode = RenderMode::EXPONENTIAL;
SubstanceMode substanceMode = SubstanceMode::WATER;
SimulationSpeed simSpeed = SimulationSpeed::NORMAL;


int main()
{
	window = Window::createWindow("Portal Fluid", 1280, 720, false, 0);
	window->init();
	initializeOpenGL();
	gameLoop();
	return 0;
}

/*
 * Runs the game loop until the window is closed
 */
void gameLoop()
{
	double currentTime;
	double previousTime;

	currentTime = previousTime = glfwGetTime();

	while (!window->shouldClose())
	{
		currentTime = glfwGetTime();
		double delta = currentTime - previousTime;

		input(delta);
		update(currentTime, delta);
		render();

		previousTime = currentTime;
	}
}

/*
 * Queries and processes user input 
 */
void input(const double &_deltaTime)
{
	// update user input state
	window->input();

	// rotate camera
	if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		const glm::vec2 mouseDelta = window->getMousePositionDelta();
		camera.rotate(glm::vec3(mouseDelta.y * 0.002f, mouseDelta.x * 0.002f, 0.0));
	}

	// translate camera
	glm::vec3 cameraTranslation;
	bool pressed = false;
	if (window->isKeyPressed(GLFW_KEY_W))
	{
		cameraTranslation.z = -15.0f * static_cast<float>(_deltaTime);
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_S))
	{
		cameraTranslation.z = 15.0f * static_cast<float>(_deltaTime);
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_A))
	{
		cameraTranslation.x = -15.0f * static_cast<float>(_deltaTime);
		pressed = true;
	}
	if (window->isKeyPressed(GLFW_KEY_D))
	{
		cameraTranslation.x = 15.0f * static_cast<float>(_deltaTime);
		pressed = true;
	}
	if (pressed)
	{
		camera.translate(cameraTranslation);
	}

	// set rendering mode
	if (window->isKeyPressed(GLFW_KEY_1))
	{
		mode = RenderMode::POINTS;
	}
	else if (window->isKeyPressed(GLFW_KEY_2))
	{
		mode = RenderMode::UV;
	}
	else if (window->isKeyPressed(GLFW_KEY_3))
	{
		mode = RenderMode::LINEAR;
	}
	else if (window->isKeyPressed(GLFW_KEY_4))
	{
		mode = RenderMode::EXPONENTIAL;
	}

	// set material/substance mode
	if (window->isKeyPressed(GLFW_KEY_F1))
	{
		substanceMode = SubstanceMode::WATER;
	}
	else if (window->isKeyPressed(GLFW_KEY_F2))
	{
		substanceMode = SubstanceMode::GLASS;
	}
	else if (window->isKeyPressed(GLFW_KEY_F3))
	{
		substanceMode = SubstanceMode::AIR_BUBBLES;
	}
	else if (window->isKeyPressed(GLFW_KEY_F4))
	{
		substanceMode = SubstanceMode::SOAP_BUBBLES;
	}

	// set simulation speed
	if (window->isKeyPressed(GLFW_KEY_F))
	{
		simSpeed = SimulationSpeed::NORMAL;
	}
	else if (window->isKeyPressed(GLFW_KEY_G))
	{
		simSpeed = SimulationSpeed::SLOW;
	}
	else if (window->isKeyPressed(GLFW_KEY_H))
	{
		simSpeed = SimulationSpeed::FAST;
	}
	else if (window->isKeyPressed(GLFW_KEY_J))
	{
		simSpeed = SimulationSpeed::FREEZE;
	}
}

/*
 * Updates particle "simulation" state
 */
void update(const double &_currentTime, const double &_deltaTime)
{
	double delta = _deltaTime;
	switch (simSpeed)
	{
	case SimulationSpeed::NORMAL:
	{
		break;
	}
	case SimulationSpeed::FAST:
	{
		delta *= 2.0;
		break;
	}
	case SimulationSpeed::SLOW:
	{
		delta *= 0.5;
		break;
	}
	case SimulationSpeed::FREEZE:
	{
		delta *= 0.0;
		break;
	}
	default:
		assert(false);
		break;
	}
	particleEmitter.update(_currentTime, delta);
}

/*
 * Clears the backbuffer and renders skybox and particles
 */
void render()
{
	glViewport(0, 0, window->getWidth(), window->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// draw skybox
	{
		skyboxShader->bind();
		// we use the inverse MVP matrix to project the skybox cubemap on a single screensized triangle
		skyboxShader->setUniform(uInverseModelViewProjectionSkybox, glm::inverse(window->getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix()))));

		// glDrawArrays needs to have a VAO bound (on AMD), even if all geometry is generated by the vertex shader
		glBindVertexArray(particleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	
	
	// render particles
	{
		std::vector<Particle *> &particles = particleEmitter.getParticles();
		if (!particles.empty())
		{
			glm::mat4 viewMatrix = camera.getViewMatrix();

			// sort particles by view space depth (we are using transparency and need to render back to front)
			std::sort(particles.begin(), particles.end(), [&viewMatrix](const Particle *a, const Particle *b)
			{
				glm::vec3 aPos = glm::vec3(viewMatrix * glm::vec4(a->position, 1.0));
				glm::vec3 bPos = glm::vec3(viewMatrix * glm::vec4(b->position, 1.0));
				return aPos.z < bPos.z;
			});

			// create a vector containing all particle positions
			std::vector<glm::vec3> positions;
			for (Particle *particle : particles)
			{
				positions.push_back(particle->position);
			}

			// update vertex buffer object with new particle positions
			glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 3 * 4, &positions[0]);
			glEnableVertexAttribArray(0);

			if (mode == RenderMode::POINTS)
			{	
				// set uniforms for rendering points
				particlePointsShader->bind();
				particlePointsShader->setUniform(uViewPortSizePoints, glm::vec2(window->getWidth(), window->getHeight()));
				particlePointsShader->setUniform(uModePoints, 0);
				particlePointsShader->setUniform(uViewPoints, camera.getViewMatrix());
				particlePointsShader->setUniform(uProjectionPoints, window->getProjectionMatrix());
			}
			else
			{
				// set uniforms for rendering quads
				particleQuadsShader->bind();
				particleQuadsShader->setUniform(uViewPortSizeQuads, glm::vec2(window->getWidth(), window->getHeight()));
				particleQuadsShader->setUniform(uModeQuads, static_cast<int>(mode));
				particleQuadsShader->setUniform(uViewQuads, camera.getViewMatrix());
				particleQuadsShader->setUniform(uProjectionQuads, window->getProjectionMatrix());
				particleQuadsShader->setUniform(uNumParticlesQuads, static_cast<int>(positions.size()));
				particleQuadsShader->setUniform(uSubstanceModeQuads, static_cast<int>(substanceMode));
				particleQuadsShader->setUniform(uInverseViewQuads, glm::inverse(viewMatrix));

				for (std::size_t i = 0; i < particles.size(); ++i)
				{
					particleQuadsShader->setUniform(uParticlesQuads[i], glm::vec3(camera.getViewMatrix() * glm::vec4(particles[i]->position, 1.0)));
				}
			}

			// draw the particles
			glDrawArrays(GL_POINTS, 0, positions.size());
		}
	}

	window->update();
}

/*
 * Initializes OpenGL, sets up the required global state and loads the shaders
 */
bool initializeOpenGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// seamless cubemap interpolation
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// we need depth testing
	glEnable(GL_DEPTH_TEST);
	// the skybox has maximum depth so we need less/equal
	glDepthFunc(GL_LEQUAL);

	// no need for stencil test
	glDisable(GL_STENCIL_TEST);
	// there is no performance to be saved by face culling in this case
	glDisable(GL_CULL_FACE);

	// we use transparency so blending needs to be enabled
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(10.0f);

	// load shaders
	particlePointsShader = ShaderProgram::createShaderProgram("Resources/Shaders/particle.vert", "Resources/Shaders/particle.frag");
	particleQuadsShader = ShaderProgram::createShaderProgram("Resources/Shaders/particle.vert", "Resources/Shaders/particle.frag", "Resources/Shaders/particle.geom");
	skyboxShader = ShaderProgram::createShaderProgram("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");

	// point uniforms
	uViewPortSizePoints = particlePointsShader->createUniform("uViewPortSize");
	uViewPoints = particlePointsShader->createUniform("uView");
	uProjectionPoints = particlePointsShader->createUniform("uProjection");
	uModePoints = particlePointsShader->createUniform("uMode");

	// quad uniforms
	uViewPortSizeQuads = particleQuadsShader->createUniform("uViewPortSize");
	uViewQuads = particleQuadsShader->createUniform("uView");
	uProjectionQuads = particleQuadsShader->createUniform("uProjection");
	uModeQuads = particleQuadsShader->createUniform("uMode");
	for (size_t i = 0; i < MAX_PARTICLES; ++i)
	{
		uParticlesQuads[i] = particleQuadsShader->createUniform("uParticles[" + std::to_string(i) + "]");
	}
	uNumParticlesQuads = particleQuadsShader->createUniform("uNumParticles");
	uEnvironmentMapQuads = particleQuadsShader->createUniform("uEnvironmentMap");
	uInverseViewQuads = particleQuadsShader->createUniform("uInverseView");
	uSubstanceModeQuads = particleQuadsShader->createUniform("uSubstanceMode");

	// skybox uniforms
	uInverseModelViewProjectionSkybox = skyboxShader->createUniform("uInverseModelViewProjection");
	uEnvironmentMapSkybox = skyboxShader->createUniform("uEnvironmentMap");

	// set "static" uniforms here to avoid setting them every frame
	skyboxShader->bind();
	skyboxShader->setUniform(uEnvironmentMapSkybox, 0);
	particleQuadsShader->bind();
	particleQuadsShader->setUniform(uEnvironmentMapQuads, 0);

	// load environment texture
	environmentTexture = Texture::createTexture("Resources/Textures/environment.dds");

	// bind environment texture to texture unit 0. since we use only one texture we can set it here once instead of every frame
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(environmentTexture->getType(), environmentTexture->getId());

	// create particle VAO/VBO
	{
		{
			// create buffer/array
			glGenVertexArrays(1, &particleVAO);
			glGenBuffers(1, &particleVBO);

			glBindVertexArray(particleVAO);

			glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
			// allocate memory and signal OpenGL that we intend to change the memory frequently
			glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * 4, NULL, GL_DYNAMIC_DRAW);

			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		}
	}

	return true;
}


/*
 * Debug function to test if an OpenGL api call raised an error
 */
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