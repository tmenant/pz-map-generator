# PZ-WorldGen: Procedural Engine Specifications & Roadmap

> **A prototype for a procedural generation pipeline targeting Project Zomboid’s map format. This project is a technical exploration into procedural content generation, aiming to synthesize entire worlds—from noise-based biomes to complex urban layouts.**

## Intentions & Objectives

The primary goal of **PZ-WorldGen** is to overcome the limitations of manual map editing by defining an automated, scalable workflow for world creation.

* **Deep-Level Learning:** Utilizing this project as a sandbox to master low-level C++ (memory management, performance optimization) and complex binary I/O.
* **Algorithmic Research:** Implementation and testing of theoretical models for natural and urban growth (Grammars, Voronoi, Graph theory).
* **Tooling-First Approach:** Developing robust internal debug tools as a prerequisite to the generator, ensuring every step of the procedural process is observable and tunable.

## Technical Specifications

### 1\. Binary Data Serialization

The engine interfaces directly with proprietary game files (`.lotpack`, `.lotheader`, `.pack`, `.tiles`).

  * **High-Speed Serialization:** Implementing a custom layer to parse vanilla assets and serialize generated data.
  * **Asset Ripping:** An automated process designed to "harvest" existing game structures into a normalized database for future procedural injection.

### 2\. Debugging & Visualization Suite

Iteration speed is prioritized through custom SFML-based tools:

  * **Cell Debugger:** A real-time visualizer to monitor noise-to-biome conversion and road pathfinding logic.
  * **Sprite Inspector:** An asset management tool to validate tile properties and sprite-stacking logic outside the main game engine.

### 3\. Procedural Pipeline

The generation is designed as a multi-stage pipeline, transforming abstract noise into concrete tile data:

  * **Macro-Scale (Noise/Biomes):** Leveraging multi-octave noise to define a "Splat Map" of biomes (e.g., Urban, Forest, Industrial).

  * **Meso-Scale (Graph Topology):** Using graph-based algorithms to compute logical road networks connecting Points of Interest (POIs).

  * **Micro-Scale (Urban Grammars):** Applying recursive subdivision and L-Systems to generate coherent city blocks and residential zones.

## Technical Stack

  * **Core:** Modern C++ (Logic, binary serialization, and PCG computations).
  * **Build System:** **CMake** + **vcpkg** for dependency management.
  * **Graphics & UI:** **SFML** & **TGUI** for high-performance debug visualizers.
  * **Quality Assurance:** **doctest** (Unit testing) and **cpptrace** (Stack-trace analysis).

## Project Architecture

```text
├── data/           # Configuration, schemas, and data assets
├── helpers/        # Automation scripts (build, deploy, test)
├── lib/            # Core library: Serializers, PCG algorithms, Data services
├── main/           # Application entry points (CLI & SFML Viewers)
└── tests/          # Unit and Integration tests (targeting the lib)
```

## Roadmap & Current Progress

### Phase 1: Foundation

  - [x] Research binary map format specifications.
  - [x] Build basic file reader.
  - [ ] Build basic file writer.
  - [ ] **Milestone:** Successfully read a vanilla map cell and re-save it without corruption.

### Phase 2: Structural Data Extraction

  - [ ] Implement logic to accurately locate buildings within vanilla cells.
  - [ ] Define a custom format for structure storage (inspired by the Tiled `.tmx` format).
  - [ ] Implement the "Asset Ripper" to extract all data into the custom format.
  - [ ] **Milestone:** Successfully harvest and store structure data from vanilla maps into local storage.

### Phase 3: Debug & Tooling

  - [ ] Implement high-performance data services for real-time retrieval.
  - [ ] Build the **Tiles Browser** for texture-pack inspection.
  - [ ] **Static Cell Viewer:** High-speed visualization of game files.
  - [ ] **Algorithmic Debugger:** Visual overlays for Noise, Voronoi, and Graph nodes.

### Phase 4: Wilderness Generation

  - [ ] Implement multi-layered 2D Noise for biome distribution.
  - [ ] **Milestone:** Generate a forest cell with coherent vegetation and noise-driven density.

### Phase 5: Urban Synthesis

  - [ ] Road network generation via graph pathfinding.
  - [ ] Urban zoning algorithms (Grammars/Subdivision).
  - [ ] **Milestone:** Generate an urban grid with roads correctly connecting to building plots.

### Phase 6: To Be Defined!

## Local Setup

**Environment:** Windows (MinGW-w64), CMake 3.26+, vcpkg.

```bash
# Set environment variables for the build scripts
setx MINGW64_HOME "C:/path/to/mingw64"
setx VCPKG_HOME "C:/path/to/vcpkg"

# Build the prototype
./helpers/build.bat
```

## References
* [pzmap2dzi](https://github.com/cff29546/pzmap2dzi): used to setup files serializers (.lotpack, .lotheader, .pack, .tiles)
* [rectPack2D](https://github.com/TeamHypersomnia/rectpack2D): used to create packed textures, allowing to batch cells rendering
