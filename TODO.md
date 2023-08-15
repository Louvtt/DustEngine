# TODOs

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
- [x] Loaders
    - [x] Raw File Loader
    - [x] Model/Mesh loader
    - [x] Texture loader 

### Render

- [~] Basic Renderer
    - [x] Clear color
    - [x] Depth Mask/Write
    - [x] Wireframe toggle
- [~] Mesh
    - [x] Basic Mesh Data
    - [x] Custom Mesh attributes
    - [x] Mesh loading (assimp) 
    - [x] Optimized Mesh batching by Material
    - [ ] Mesh Instancing
- [x] Material
    - [x] TextureMaterial
    - [~] PhongMaterial (Named PBRMaterial because will be upgraded to that) 
- [x] Shader
    - [x] Basic Shader
    - [x] Loading Shaders
    - [x] Uniforms
    - [x] Reloading (must be called explictly)
- [x] Texture
    - [x] Texture loading (stb_image)
    - Name + ID
- [x] Skybox
- [~] **FrameBuffers**
- [ ] Shadow mapping (ScreenSpace ?)
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

- [x] Engine DebugLayer
- [ ] Engine Panel/Editor via ImGui (please)
- [ ] Interface
    - [ ] Menu / Panel
    - [ ] Buttons
    - [ ] Custom Text/Number Inputs
    - [ ] Tooltip
    - [ ] Simple Images
