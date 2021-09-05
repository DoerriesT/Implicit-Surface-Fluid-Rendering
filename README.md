# Implicit Surface Fluid Rendering (Portal Fluid)

![PortalFluid](screenshot_final.png?raw=false "The final effect")

This is a demonstration of an effect for rendering fluids with implicit surfaces (similar to those found in the video game ‘Portal 2’), written in C++ and OpenGL.

# Controls
- Right click + mouse rotates the camera.
- WASD moves the camera
- 1-3 to switch between different drawing modes (points, quads, spherical distance fields, final result)
- F, G, H, J to switch particle simulation speed (normal, slow, fast, freeze)
- F1-F4 to switch between different materials (water, glass, air bubbles, soap bubbles)

# How does it work?

|  |  |
| ------------- | ------------- |
| It works by first simulating particles on the CPU. | ![PortalFluid](screenshot_particles.png?raw=false "Particles") |
| Then it renders a quad for each particle on the GPU. | ![PortalFluid](screenshot_quads.png?raw=false "Quads") |
| And finally it traces a ray in the quad’s pixel shader against the implicit surface formed by the distance field formed by the set of particles. Each particle on its own defines a spherical distance field where the implicit surface is defined as being at distance X from the surface. This results in small spheres that can look like droplets. | ![PortalFluid](screenshot_spheres.png?raw=false "Spheres")  |
| By combining the distance fields of multiple particles, a more natural result can be achieved because the implicit surface now deforms according to the influences of multiple particles. | ![PortalFluid](screenshot_final.png?raw=false "The final effect") |

# Requirements
- GPU with support for OpenGL 3.3 or better

# How to build
The project comes as a Visual Studio 2017 solution and already contains all dependencies. It should be built as x64.

# Credits
- glad https://glad.dav1d.de/
- GLFW https://www.glfw.org/
- GLM https://github.com/g-truc/glm
- GLI https://github.com/g-truc/gli
