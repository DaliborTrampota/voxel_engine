[![CMake Build (Win, Linux) + Release binaries](https://github.com/DaliborTrampota/voxel_engine/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/DaliborTrampota/voxel_engine/actions/workflows/cmake-multi-platform.yml)

# VoxelEngine

## Documentation

| Topic | File |
|-------|------|
| Getting started, Engine subclass, game loop | [docs/getting-started.md](docs/getting-started.md) |
| Blocks, geometries, materials, variant blocks | [docs/blocks.md](docs/blocks.md) |
| World, chunks, terrain generation | [docs/world-terrain.md](docs/world-terrain.md) |
| Render passes, RenderContext, Renderable | [docs/rendering.md](docs/rendering.md) |
| Event system, input | [docs/events-input.md](docs/events-input.md) |

Online Doxygen reference: https://dalibortrampota.github.io/voxel_engine/

## Notes

#### Coordinate System
- Follows OpenGL's standard, RH coordinate system
- +Y is up and −Z is forward

#### Namespaces
- All engine classes are under `engine::`
- Layer constants are under `engine::Layers::` (e.g. `engine::Layers::Opaque`)
- LWGL (OpenGL abstraction) classes are under `gl::`

## Quick Setup

In your `CMakeLists.txt` after linking VoxelEngine:

```cmake
setup_voxelengine_resource_copy(<your_target>)
```

See [docs/getting-started.md](docs/getting-started.md) for the full walkthrough.
