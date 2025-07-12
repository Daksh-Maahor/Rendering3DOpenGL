# 3D Cube Renderer

A modern OpenGL-based 3D renderer built with C++ that renders multiple textured cubes with lighting and camera controls.

## Features

- **3D Rendering**: Renders multiple cubes in 3D space with proper perspective projection
- **Lighting System**: Implements ambient, diffuse, and specular lighting
- **Camera Controls**: First-person camera with mouse look and WASD movement
- **Modern OpenGL**: Uses OpenGL 3.3+ with core profile
- **Shader-based Rendering**: Custom vertex and fragment shaders
- **Object-Oriented Design**: Clean separation of concerns with dedicated classes

## Controls

- **WASD**: Move camera forward/backward/left/right
- **Mouse**: Look around (camera rotation)
- **Mouse Wheel**: Zoom in/out
- **ESC**: Exit application

## Dependencies

- **OpenGL**: 3.3 or higher
- **GLFW**: Window management and input handling
- **GLEW**: OpenGL extension loading
- **GLM**: OpenGL Mathematics library for vectors and matrices
- **CMake**: Build system

## Building the Project

### Prerequisites

Make sure you have the following packages installed on your system:

#### Arch Linux / Manjaro:
```bash
sudo pacman -S cmake glfw-x11 glew glm
```

#### Ubuntu / Debian:
```bash
sudo apt-get install cmake libglfw3-dev libglew-dev libglm-dev
```

#### macOS (with Homebrew):
```bash
brew install cmake glfw glew glm
```

### Build Instructions

1. Clone or download the project
2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure and build:
   ```bash
   cmake ..
   make
   ```

4. Run the application:
   ```bash
   ./Rendering3D
   ```

## Project Structure

```
Rendering3D/
├── CMakeLists.txt          # Build configuration
├── include/                # Header files
│   ├── Shader.h           # Shader management
│   ├── Camera.h           # Camera system
│   ├── Mesh.h             # Geometry rendering
│   └── Cube.h             # Cube geometry
├── src/                   # Source files
│   ├── main.cpp           # Main application
│   ├── Shader.cpp         # Shader implementation
│   ├── Camera.cpp         # Camera implementation
│   ├── Mesh.cpp           # Mesh implementation
│   └── Cube.cpp           # Cube implementation
├── shaders/               # GLSL shader files
│   ├── vertex.glsl        # Vertex shader
│   └── fragment.glsl      # Fragment shader
└── README.md              # This file
```

## Technical Details

### Rendering Pipeline

1. **Vertex Shader**: Transforms vertices from object space to clip space
2. **Fragment Shader**: Calculates lighting and colors for each pixel
3. **Depth Testing**: Ensures proper 3D rendering order

### Lighting Model

The renderer implements a Phong lighting model with:
- **Ambient Lighting**: Base illumination level
- **Diffuse Lighting**: Directional lighting based on surface normals
- **Specular Lighting**: Highlights for shiny surfaces

### Camera System

The camera uses a first-person perspective with:
- **Position**: 3D world coordinates
- **Orientation**: Yaw and pitch angles
- **Projection**: Perspective projection matrix
- **View Matrix**: Transforms world coordinates to view space

## Customization

### Adding More Objects

To add more cubes or different objects:

1. Create new geometry classes similar to `Cube`
2. Add positions to the `cubePositions` vector in `main.cpp`
3. Render them in the main loop

### Modifying Lighting

Adjust lighting parameters in the fragment shader or modify the uniform values in `main.cpp`:
- `lightPos`: Position of the light source
- `lightColor`: Color and intensity of the light
- `objectColor`: Base color of the objects

### Changing Shaders

Modify the GLSL files in the `shaders/` directory:
- `vertex.glsl`: Vertex transformations and data passing
- `fragment.glsl`: Pixel coloring and lighting calculations

## Troubleshooting

### Common Issues

1. **Shader compilation errors**: Check that your GPU supports OpenGL 3.3+
2. **Missing dependencies**: Ensure all required libraries are installed
3. **Window creation fails**: Check that your graphics drivers are up to date

### Debug Information

The application will print error messages to the console for:
- GLFW initialization failures
- GLEW initialization failures
- Shader compilation errors
- File loading errors

## License

This project is open source and available under the MIT License. 
