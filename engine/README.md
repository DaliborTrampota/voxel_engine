[![CMake Build (Win, Linux) + Release binaries](https://github.com/DaliborTrampota/voxel_engine/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/DaliborTrampota/voxel_engine/actions/workflows/cmake-multi-platform.yml)

# VoxelEngine


## Notes

#### Coordinate System
- Follows OpenGL's standard, RH coordinate system
- +Y is up and -Z is forward



## Usage

#### Setup

In your game/project's CMake file after linking VoxelEngine, call `setup_voxelengine_resource_copy(target)` and `setup_seuil_resource_copy(target)` with the target name of your project.

#### Registering blocks
| Class/File                | Usage Description                                                                 |
|---------------------------|----------------------------------------------------------------------------------|
| `<data/Registry.h>`       | Provides the `Registry` class for registering and managing anything.           |
| `<data/RegistryManager.h>`| Offers the `RegistryManager` class to handle multiple registries. By default, blocks and geometries |
| `<data/TextureLoader.h>`  | Loads a directory into a OpenGL texture array or sampler2DArray and adds them to TextureManager                   |
| `<data/TextureManager.h>` | Holds loaded textures to obtain texture index by its name.         |

```cpp
Registry<Geometry>& geometries = RegistryManager::Geometries();

geometries.add(Geometry::Cube(), "cube");
geometries.add(Geometry::Cylinder(), "cylinder");

Registry<Block>& blocks = RegistryManager::Blocks();
TextureManager& texMgr = TextureManager::Get();

auto DIRT = Block(1, &geometries.get("cube"))
    .isSolid(true)
    .material(Material()
        .add(FaceTag::All, texMgr.texture("dirt"))
    );

auto GRASS = Block(2, &geometries.get("cube"))
    .isSolid(true)
    .material(Material()
        .add(FaceTag::Top, texMgr.texture("grass_block_top"))
        .add(FaceTag::Bottom, texMgr.texture("dirt"))
        .add(FaceTag::Side, texMgr.texture("dirt"))
    );
```
