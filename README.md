# Shard Engine

A custom 3D game engine built from scratch in C++ on top of DirectX 11 — covering everything from the rendering pipeline and shader framework to physics simulation and in-engine tooling.

---

## Core Systems

### Rendering Pipeline
Built on **DirectX 11**, the renderer handles the full pipeline from scene submission to final output. Geometry, materials, lighting, and draw call batching are all managed internally with a clean separation between the engine's scene representation and the underlying D3D11 API calls.

### Shader Framework
Rather than writing raw HLSL and manually managing constant buffers and shader reflection, Shard Engine includes a **custom shader framework** that abstracts authoring and binding of vertex, pixel, and compute shaders. The framework handles:
- Shader compilation and hot-reloading
- Constant buffer management and per-draw vs per-frame update semantics
- A clean API so higher-level engine code never touches D3D11 shader objects directly

### Physics — NVIDIA PhysX
Rigid body simulation, collision detection, and scene queries are powered by **NVIDIA PhysX**, integrated directly into the engine's update loop. The integration includes spatial acceleration structures — dynamic **quadtrees** for 2D broad-phase queries and **octrees** for 3D scene partitioning — giving fine-grained control over which objects enter the PhysX simulation pipeline.

### In-Engine UI — ImGui
**Dear ImGui** is integrated as the engine's immediate-mode UI layer, used for runtime debugging panels, scene inspection, and performance overlays. This allows iterating on engine internals without recompiling — tweak physics parameters, inspect draw call counts, or visualize spatial structures live.

---

## Technical Highlights

- **Language:** C++ (91%), C (8%), HLSL (1%)
- **Graphics API:** DirectX 11
- **Physics:** NVIDIA PhysX
- **UI:** Dear ImGui
- **Platform:** Windows (Visual Studio / MSVC)
- **Architecture:** Modular — core, graphics, physics, input, and shared utilities are cleanly separated into independent subsystems

---


## Build

**Requirements:**
- Windows 10 / 11
- Visual Studio 2022 with C++17 support
- DirectX 11 SDK (included with Windows SDK)
- NVIDIA PhysX SDK

Open `graphic.sln` in Visual Studio and build in Release or Debug configuration.
