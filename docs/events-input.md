# Events & Input

## Event System

The event system follows a **source / site** pattern:

- **`EventSource`** — fires events. Objects that produce events inherit from it (or a specialised subclass).
- **`EventSite`** — receives events. Objects that want to listen inherit from the matching site subclass and register with `source->subscribe(this)`.

```
EventSource  ──subscribe(EventSite*)──►  [set of EventSite*]
                                               │
             ──fireEvent(...)──────────────────▼
                                         EventSite::onXxx(event)
```

There are three specialised source/site pairs, each covering a different area:

| Source | Site | Events fired |
|--------|------|-------------|
| `GLFWEventSource` | `GLFWEventSite` | Mouse move, mouse button, keyboard |
| `EngineEventSource` | `EngineEventSite` | Window resize |
| `LevelEventSource` | `LevelEventSite` | Chunk about-to-load, chunk unload |

`InputSystem` and `Window` are both `GLFWEventSource`. `Window` is also an `EngineEventSource`. `World` is a `LevelEventSource`.

### Subscribing to Events

```cpp
// Your class inherits from one or more *Site classes and overrides the handlers.

class MyListener
    : public engine::GLFWEventSite
    , public engine::EngineEventSite
    , public engine::LevelEventSite
{
protected:
    // GLFWEventSite
    void mouseMoveEvent(engine::MouseEvent* e) override {
        float dx = e->x, dy = e->y;
    }
    void mouseButtonEvent(engine::MouseButtonEvent* e) override {
        // e->button, e->action, e->mods  (GLFW constants)
    }
    void keyboardEvent(engine::KeyboardEvent* e) override {
        // e->key, e->action, e->mods  (GLFW constants)
    }

    // EngineEventSite
    void windowResizeEvent(engine::ResizeEvent* e) override {
        int w = e->width, h = e->height;
    }

    // LevelEventSite
    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* e) override {
        // e->chunk is the Chunk about to be populated.
        // Set e->chunk->data()->populated = true here to skip the terrain generator
        // (useful when loading a saved chunk from disk).
    }
    void chunkUnloadEvent(engine::ChunkUnloadEvent* e) override {
        // e->chunk is the chunk being unloaded.
    }
};

// Register with the sources:
m_inputSystem->subscribe(&myListener);
m_window->subscribe(&myListener);
m_world->subscribe(&myListener);
```

Multiple sites can subscribe to the same source, and a single site can subscribe to multiple sources.

---

## Input System

`InputSystem` is accessed via `engine.inputSystem()` (or `m_inputSystem` inside your `Engine` subclass). It is itself a `GLFWEventSource`, so you can subscribe event-based listeners to it.

### Polling Key State

For frame-by-frame polling (better for movement), use the templated `isKey` / `isMouse`:

```cpp
#include <input/InputSystem.h>
#include <input/InputTypes.h>
#include <GLFW/glfw3.h>

engine::InputSystem* input = engine.inputSystem();

// Exactly the frame the key went down
if (input->isKey<engine::KeyState::Pressed>(GLFW_KEY_SPACE)) { jump(); }

// Held continuously while key is down
if (input->isKey<engine::KeyState::Held>(GLFW_KEY_W)) { moveForward(); }

// True both on the pressed frame and while held
if (input->isKey<engine::KeyState::Down>(GLFW_KEY_W)) { /* ... */ }

// Exactly the frame the key was released
if (input->isKey<engine::KeyState::Released>(GLFW_KEY_E)) { interact(); }

// Mouse buttons
if (input->isMouse<engine::KeyState::Pressed>(GLFW_MOUSE_BUTTON_LEFT)) { attack(); }
```

### Axes

```cpp
float sideways = input->getAxis(engine::InputAxis::Sideways);  // A/D
float forward  = input->getAxis(engine::InputAxis::Forward);   // W/S
float vertical = input->getAxis(engine::InputAxis::Vertical);  // Space/Ctrl
float mouseX   = input->getAxis(engine::InputAxis::MouseX);
float mouseY   = input->getAxis(engine::InputAxis::MouseY);
float scroll   = input->getAxis(engine::InputAxis::MouseScroll);
```

### Calling `beginFrame`

`InputSystem::beginFrame()` must be called once per frame to advance key states (`Pressed` → `Held`, `Released` → `None`). The engine's `gameloop()` handles this internally — you only need to call it manually if you drive the loop yourself.

---

## Writing Custom EventSources

You can create your own source/site pair for game-specific events:

```cpp
// 1. Define the event struct
struct PlayerDeathEvent : public engine::Event {
    int respawnDelay;
};

// 2. Define the site interface
class PlayerEventSite : public virtual engine::EventSite {
    friend class PlayerEventSource;
protected:
    virtual void playerDeathEvent(PlayerDeathEvent* e) {}
};

// 3. Define the source
class PlayerEventSource : public engine::EventSource {
protected:
    void firePlayerDeathEvent(PlayerDeathEvent* e) {
        fireEvent(&PlayerEventSite::playerDeathEvent, e);
    }
};

// 4. Inherit the source in your class and fire away
class Player : public PlayerEventSource {
    void die() {
        PlayerDeathEvent e{ 5 };
        firePlayerDeathEvent(&e);
    }
};

// 5. Subscribe a listener
class UI : public PlayerEventSite {
protected:
    void playerDeathEvent(PlayerDeathEvent* e) override {
        showDeathScreen(e->respawnDelay);
    }
};

player.subscribe(&ui);
```
