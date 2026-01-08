# Voxel Engine

- There are mostly two types of voxel engines
    - Raycasting - Small voxels are usually done by raycasting; traversting accelerated datastructures like SVO's (Sparse Voxel Octree) directly on GPU
    - Meshing - Bigger voxels like those in Minecraft use vertices; mesh is built on CPU and GPU renders them using render pipeline

- This one is the latter

## Dependencies
- GLAD - OpenGL function mapper/extension loader
- GLM - maths/vector library
- GLFW - platform independent window and input management library
- Single header libs
    - stb_image - image loading
    - PerlinNoise
    - ogt_vox - loading magica voxel models
- SoLoud - basic audio support 
- LWGL - Low Level OpenGL Library
- SEUIL - Simple Enough UI Library
    - LWGL
    - FreeType - font loading
    - MSDF - library implementing [Valve's interesting paper](https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf)

## Features
- Input system
- Directional audio system
- AABB world collisions
- Skybox
- Custom chunks and its dimensions
- DI terrain generators
- Registry for blocks and geometries and user defined objects
- Variant blocks - blocks that change shape/model based on its neighbours
- Multiblocks - multiple models in one block position
- Extensible render pass system (default are: DirectionalPass, TransparentPass, ScenePass) 
- Directional lights with shadows

## Missing features (listed by priority)
- Omnidirectional lights and shadows
- Particle system
- Engine graphical settings
- Instanced rendering (WIP)
- Translucent block support (WIP)
- Cascaded shadow maps
- Hard shadows support
- Support custom input axis registration
- Support input keybinds
- PBR
