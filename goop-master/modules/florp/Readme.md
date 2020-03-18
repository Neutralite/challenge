# Welcome to FLORP!
## The Framework for Letting Objects be Rendered Passably

The goal of FLORP is to provide some basic graphical and application tools to make developing games and applications easier. 


# General Design
FLORP can be seen as a basic game engine in the fact that it provides a basic game loop for you. FLORP allows users to define *Application Layers* that hook into this game loop, and can provide functionality for the game. The hooks provided to application layers are as follows:

* `Initialize()` -> For performing initialization before the game loop
* `Shutdown()` -> For cleaning up after the game loop ends
* `Update()` -> Variable rate updates called as fast as possible
* `LateUpdate()` -> Same as above, but called after all layers have been Updated
* `FixedUpdate()` -> Called at as close to a fixed rate as possible (see Timing::FixedTimeStep)
* `PreRender()` -> Called before any object rendering is performed, whenever a new frame starts
* `Render()` -> Called during a frame for the layer to perform rendering to the main screen
* `PostRender()` -> Called during a frame after all layers have rendered
* `PreGUI()` -> Called during a frame after PostRender, but before RenderGUI
* `RenderGUI()` -> Called during a frame after scene rendering, to render GUI element
* `PostGUI()` -> Called after all layers have called RenderGUI
* `OnSceneExit()` -> Called when the appplication is leaving a scene
* `OnSceneEnter()` -> Called when the application is entering a scene

In addition to the *Application Layers*, FLORP also provides an entity-component-system using ENTT. The component registries are accessed via *Scenes* and the *SceneManager*, or by getting the current scene's registry with *GetCurrentRegistry()*. 

As part of the ECS, FLORP also provides a behaviour system, implemented via the *IBehaviour* interface, and invoked by the *BehaviourLayer*. The behaviour system lets you easily add behaviours to entities, and new user behaviours can be added to an entity by calling *AddBehaviour&lt;UserBehaviourType&gt;(entity)*. All of the methods that a behaviour implements will accept an entt::entity which is the current entity that the behaviour is associated with. Due to the nature of entt, it is important to not keep references to other components. The hooks that can be used by the behaviour system are:
* `OnLoad(entt::entity)` -> Called when the scene loads, or the behaviour is added to an entity
* `OnUnload(entt::entity)` -> Called before a scene is unloaded
* `Update(entt::entity)` -> Called during the variable rate update
* `LateUpdate(entt::entity)` -> Called during the variable rate LateUpdate
* `FixedUpdate(entt::entity)` -> Called during the fixed update
* `RenderGUI(entt::entity)` -> Called during the GUI rendering phase

# Namespaces
FLORP is broken up into 4 major sections:

### `florp::app`
This namespace contains utilities and structures to ease application development. It contains the following classes:
* `Application`
* `ApplicationLayer`
* `Timing`
* `Window`

### `florp::game`
This namespace contains tools that are specific to game development, such as scene management, ECS, transformations, cameras, etc... The classes in the `florp::game` namespace are:
* `BehaviourLayer`
* `CameraComponent`
* `IBehaviour`
* `BehaviourBinding` (the component for associating IBehaviours to a entity)
* `ImGuiLayer`
* `Material`
* `RenderableComponent`
* `Scene`
* `SceneManager`
* `Transform`

### `florp::graphics`
This namespace contains utilities for rendering using the OpenGL rendering API. It contains the following classes:
 * `IGraphicsResource`
 * `ITexture`
 * `Mesh`
 * `MeshBuilder`
 * `MeshData `
 * `BufferElement`
 * `BufferLayout`
 * `Vertex`
 * `ObjLoader`
 * `Shader`
 * `Texture2D`
 * `TextureCube`
 * `TextureSampler`
 * `BlendState`
 * `RasterizerState`
 There are also multiple enums and helper classes defined as part of the graphics API

### `florp::utils`
This namespace contains some general-purpose utilities for FLORP. Currently, these are:
* `FileUtils` for reading the contents of files
* `TextureUtils` for manipulating texture data
