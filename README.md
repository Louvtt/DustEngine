<div align="center">

# DustEngine

OpenGL Game Engine made for DustInTime <br/>
By Louvtt.

</div>

## Features

### Core

- [x] Application Entry
- [x] Window Creation
    - [x] Os Window Creation (glfw)
    - [x] Gpu api loading (OpenGL via glad2)

### IO

- [x] InputManager
    - [x] Mouse button states
    - [x] Key states
    - [x] Mouse Pos
    - Mouse Scroll ?
- [x] File loader
    - [ ] Mesh loader
    - [ ] Texture loader 

### Render

- [~] Basic Renderer
    - [x] Clear color
    - [x] Depth Mask/Write
    - [x] Wireframe toggle
- [~] Mesh
    - [x] Basic Mesh Data
    - [x] Custom Mesh attributes
    - [ ] Mesh loading (assimp)
    - [ ] Mesh Instancing
- [x] Shader
    - [x] Basic Shader
    - [x] Loading Shaders
    - [x] Uniforms
    - [x] Reloading (must be called explictly)
- [ ] Texture
    - [ ] Texture loading (stb_image)
    - Name + ID
- [ ] Text rendering :(
    - [ ] Loading Font (stb_truetype)
    - [ ] Rendering Simple text
    - [ ] Custom colored rendering text (with effects like bold, waves ...)
- [ ] Animation System
    - [ ] Basic proprety animation
    - [ ] Skeletal animation
    - [ ] Animation Loading ? (assimp)

### Entity

- [ ] Entity System
    - Transform 
    - Scripts / Components
- [ ] Scene System
    - [ ] Loading Scene
    - [ ] Switching Scene
    - [ ] Transform inheritance transfer
- [ ] Components/Scripts

### UI

- [ ] Engine Panel/Editor via ImGui (please)
- [ ] Interface
    - [ ] Menu / Panel
    - [ ] Buttons
    - [ ] Custom Text/Number Inputs
    - [ ] Tooltip
    - [ ] Simple Images