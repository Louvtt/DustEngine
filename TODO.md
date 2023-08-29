# TODOs

### Core

- [x] Application Entry
- [~] Application
    - [x] Attached renderer and window
    - [ ] Layers
- [x] Window Creation
    - [x] Os Window Creation (glfw)
    - [x] Gpu api loading (OpenGL via glad2)
    - [x] ImGui loading and config \w docking
- [ ] Reflection
    - [ ] Type reflection
    - [ ] Members
    - [ ] Functions
    - [ ] Constructors
    - [ ] Methods

### IO

- [x] InputManager
    - [x] Mouse button states
    - [x] Key states
    - [x] Mouse Pos
    - Mouse Scroll ?
- [x] Assets Manager
    - [x] Sync Loaders 
        - [x] Raw File Loader
        - [x] Model/Mesh loader
        - [x] Texture loader 
    - [ ] Async loaders (Load desc and construct Render data on main thread (because OpenGL))
    - [x] Listing
    - [x] Auto loading
        - [ ] UUID
        - [ ] .meta to load faster and store settings for files (filters, preload, lazyloading...)

### Render

- [x] Basic Renderer
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
- [x] FrameBuffers
- [ ] **Shadow mapping**
    - [ ] **Simple shadow mapping**
    - [ ] CSM
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
- [~] Engine Panel/Editor via ImGui (see sponza -> *convert into full editor*)
- [ ] ImGuizmo
- [ ] Node Editor for dialogue tool
- [ ] Interface
    - [ ] Menu / Panel
    - [ ] Buttons
    - [ ] Custom Text/Number Inputs
    - [ ] Tooltip
    - [ ] Simple Images

### Addons

- [ ] Addons/Mods support (using dll)

------

## FIXES

- Flatten classes and structures and avoid nested
- Model materials slots instead of one mat per mesh to be able to batch a lot.
- Finish PBR Model