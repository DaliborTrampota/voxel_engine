# Rendering

## Overview

The rendering pipeline is built around three concepts:

1. **`Renderable`** — anything that can be drawn implements this interface and pushes `RenderContext` objects into the engine's queue.
2. **`RenderContext`** / **`GroupRenderContext`** — a lightweight description of a single draw call (buffer, material, matrices, pass mask).
3. **`RenderPass`** — a stage in the pipeline (shadow map, scene, transparency, composite). Passes iterate the queue and draw every context whose `passMask` matches.

---

## Renderable

Implement `Renderable` on any class that needs to draw itself:

```cpp
#include <render/Renderable.h>

class MyObject : public engine::Renderable {
public:
    void render(engine::Engine& engine, const engine::Camera* camera, int pass) override {
        engine::RenderContext ctx;
        ctx.attributes  = m_buffer.get();
        ctx.material    = &m_material;
        ctx.camera      = camera;
        ctx.passMask    = engine::RenderPass::Scene;
        ctx.setModelMatrix(m_position);

        engine.submitRender(std::move(ctx));
    }
};
```

The `pass` parameter lets you handle multiple render passes in a single `render()` call if needed (e.g. `Chunk` uses `pass == 1` for opaque and `pass == 2` for transparent).

---

## RenderContext

```cpp
struct RenderContext {
    gl::IBuffer*      attributes;          // vertex buffer
    const Material*   material;            // shader + uniforms
    const Camera*     camera = nullptr;    // leave null to inherit from pass
    const gl::FBO*    fbo = nullptr;       // leave null to use the pass's FBO
    RenderPass::ID    passMask = RenderPass::Scene; // which pass(es) draw this

    struct {
        std::optional<glm::mat4> projection; // override camera projection
        std::optional<glm::mat4> view;       // override camera view
        glm::mat4 model;                     // model matrix (required)
    } matrices;

    // Helpers
    void setModelMatrix(const glm::vec3& position);
    void setModelMatrix(const glm::vec3& pos, float angle, const glm::vec3& axis);
};
```

Submit a single draw call:

```cpp
engine.submitRender(std::move(ctx));
// or skip the queue and draw immediately:
engine.submitRender(std::move(ctx), /*immediate=*/true);
```

### GroupRenderContext

For batching many draw calls that share a material and camera (e.g. instanced world objects), use `GroupRenderContext` to avoid per-call overhead:

```cpp
engine::GroupRenderContext group;
group.material  = &m_material;
group.camera    = camera;
group.passMask  = engine::RenderPass::Scene;
group.reserve(objects.size());

for (auto& obj : objects) {
    group.addDrawCall(obj.buffer.get(), obj.modelMatrix);
}

engine.submitRender(std::move(group));
```

---

## Render Passes

### Built-in Passes

The engine registers these passes by default (in order):

| Pass | ID constant | Purpose |
|------|-------------|---------|
| `DirectionalShadowPass` | `RenderPass::DirectionalShadow` | Renders depth map for directional light |
| `ScenePass` | `RenderPass::Scene` | Main opaque scene |
| `TransparentPass` | `RenderPass::SceneTransparent` | Transparent / alpha-blended geometry |
| `CompositePass` | `RenderPass::Composite` | Full-screen post-processing |

Set the `passMask` on your `RenderContext` to control which pass draws it:

```cpp
ctx.passMask = engine::RenderPass::Scene;                          // opaque only
ctx.passMask = engine::RenderPass::SceneTransparent;               // transparent
ctx.passMask = engine::RenderPass::Scene | engine::RenderPass::DirectionalShadow; // both
```

### Adding a Custom Pass

Subclass `RenderPass` and register it via `RenderPassRegistry`:

```cpp
#include <render/RenderPass.h>
#include <render/RenderPassRegistry.h>

class MyOutlinePass : public engine::RenderPass {
public:
    MyOutlinePass(glm::ivec2 resolution)
        : RenderPass(resolution) {}  // auto-assigns the next available ID

    void beforeRender(engine::Engine& engine, uint8_t pass) override {
        // bind FBO, set GL state
    }

    void afterRender(engine::Engine& engine, uint8_t pass) override {
        // blit / resolve
    }
};

// Register after the scene pass (position 0 = append):
engine::RenderPassRegistry::Get().registerPass(
    std::make_unique<MyOutlinePass>(windowSize)
);
```

Your pass ID is automatically a power-of-two bit flag — use it as a `passMask` on any `RenderContext` you want this pass to process:

```cpp
ctx.passMask = myOutlinePass->id();
```

### Accessing a Pass

```cpp
auto* scenePass = engine::RenderPassRegistry::Get().getPass<engine::ScenePass>();
```

### Removing a Pass

```cpp
engine::RenderPassRegistry::Get().deletePass<MyOutlinePass>();
```

---

## Material

`Material` extends `gl::ShaderProgram` with two engine-level flags:

```cpp
#include <render/Material.h>

engine::Material mat("path/to/vert.glsl", "path/to/frag.glsl");
mat.setShadowSupport(true); // engine will supply shadow-map uniforms
mat.mvpSupport(true);       // engine will supply MVP matrices (default: true)
```

When `supportsMVP()` is true, the engine automatically uploads `u_projection`, `u_view`, and `u_model` uniforms from the `RenderContext` matrices before each draw call.

When `supportsShadows()` is true, the `DirectionalShadowPass` will supply `u_lightSpaceMatrix` and the shadow map sampler.

---

## Camera

Pass a `Camera*` into `RenderContext::camera` or into `World::render` / your own `Renderable::render`. The engine uses it to derive `view` and `projection` matrices unless you override them manually in the context.

```cpp
#include <scene/Camera.h>

auto camera = std::make_shared<engine::Camera>();
camera->setFov(90.0f);
camera->setPosition(glm::vec3(0, 64, 0));
camera->setDirection(glm::vec3(0, 0, -1));

// Subscribe to window resize events to keep aspect ratio correct:
m_window->subscribe(camera.get());  // Camera implements EngineEventSite
```

---

## Frame Lifecycle (Summary)

```
gameloop()
 ├─ beforeRender()           ← optional override
 ├─ render(dt)               ← required override
 │    ├─ fireUpdate(dt)
 │    └─ world->render(engine, camera)
 │         └─ chunk->render(engine, camera, pass)
 │              └─ engine.submitRender(RenderContext)  // pushed to queue
 ├─ flush()                  ← iterates passes, draws queue
 │    for each RenderPass:
 │      pass.beforeRender()
 │      for each RenderContext where (ctx.passMask & pass.id()):
 │        draw(ctx)
 │      pass.afterRender()
 └─ afterRender()            ← optional override (UI, audio)
```

---

**See also:** [Getting Started](getting-started.md) for `Engine` subclassing and the directional light setup.
