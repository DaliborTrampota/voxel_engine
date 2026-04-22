# Getting Started

## CMake Setup

After linking VoxelEngine in your `CMakeLists.txt`, call the resource copy helper so engine shaders are copied to your output directory:

```cmake
setup_voxelengine_resource_copy(<your_target>)
```

## Coordinate System

VoxelEngine follows OpenGL's right-handed coordinate system:

- **+Y** is up
- **−Z** is forward (into the screen)
- **+X** is right

## Subclassing Engine

Your game's main class must inherit from `engine::Engine` and implement the `render` method:

```cpp
#include <render/Engine.h>
#include <render/Window.h>

class MyGame : public engine::Engine {
public:
    MyGame(std::unique_ptr<engine::Window> window)
        : Engine(std::move(window)) {}

    void render(double dt) override {
        // called every frame — drive your game logic and rendering here
    }
};
```

## Game Loop

Call `gameloop()` to start the main loop. It never returns until the window is closed.

```cpp
int main() {
    auto window = std::make_unique<engine::Window>();
    window->init(1280, 720, "My Game");

    MyGame game(std::move(window));
    game.gameloop();
}
```

Internally, `gameloop()` calls three virtual hooks each frame in order:

| Hook | When it runs |
|------|--------------|
| `beforeRender()` | Before any rendering (optional override) |
| `render(dt)` | **Required override.** Drive updates and submit renderable objects here. |
| `afterRender()` | After `flush()` completes (optional override, good for UI/audio) |

In the middle of that sequence, `flush()` iterates all registered render passes and draws every submitted `RenderContext`. You can override `flush()` if you need full control.

## Update and Tick Subscribers

Two separate update rhythms are available:

- **`subscribeUpdate(shared_ptr<Updateable>)`** — called every frame with the elapsed `dt` in seconds.
- **`subscribeTick(shared_ptr<Tickable>)`** — called at a fixed rate (`Engine::TickRate`, default 1/30 s) regardless of frame rate. Good for game logic that must be deterministic.

Both `Updateable` and `Tickable` are simple interfaces:

```cpp
// Updateable — every frame
class MySystem : public engine::Updateable {
    void update(float dt) override { /* ... */ }
};

// Tickable — fixed rate
class MySystem : public engine::Tickable {
    void tick(float dt) override { /* ... */ }
};
```

## Window and Input

The `Engine` base class holds:
- `m_window` — the `Window` instance (also accessible via `window()`)
- `m_inputSystem` — the `InputSystem` (also accessible via `inputSystem()`)

To wire raw GLFW callbacks, fill a `GLFWUserPointer` struct and call `window->setUserPointer`:

```cpp
GLFWUserPointer pointer{ m_inputSystem.get(), m_window.get() };
m_window->setUserPointer(&pointer);
```

## Directional Light

To enable shadows, create a `Sun` and register it before the game loop starts:
Note: Right now its required for the engine to work.

```cpp
setDirectionalLightSource(
    std::make_shared<engine::Sun>(
        glm::ivec2(4096, 4096),   // shadow map resolution
        camera,                    // camera for shadow frustum
        glm::vec3(0.5f, -1.0f, 0.2f) // light direction
    )
);
```

## Minimal Example

```cpp
#include <render/Engine.h>
#include <render/Window.h>
#include <level/World.h>

class MyGame : public engine::Engine {
public:
    MyGame(std::unique_ptr<engine::Window> window)
        : Engine(std::move(window)) {
        // register blocks, load textures, etc.
    }

    void render(double dt) override {
        fireUpdate(static_cast<float>(dt)); // drives subscribed Updateables
        m_world->render(*this, m_camera.get());
    }

private:
    std::shared_ptr<engine::World> m_world;
    std::shared_ptr<engine::Camera> m_camera;
};
```

---

**Next steps:**
- [Blocks](blocks.md) — registering geometries, blocks, materials
- [World & Terrain](world-terrain.md) — chunks, terrain generation, block access
- [Rendering](rendering.md) — render passes, `Renderable`, `RenderContext`
- [Events & Input](events-input.md) — event system, input handling
