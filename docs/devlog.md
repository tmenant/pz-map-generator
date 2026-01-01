# Devlog

This devlog documents my journey creating a **Project Zomboid map generator**. It details my technical choices, the difficulties I encountered, how I overcame them, and the lessons learned along the way.

Beyond chronological progress, this document aims to provide a high-level understanding of the project architecture and its evolution over time.

## November 2025

### Understanding the Ecosystem

My first step was to understand the workflow of existing map modders. I started by identifying the standard tools (**TileZed**, **WorldEd**) and downloading them to analyze their input and output.

I followed a comprehensive [YouTube tutorial series](https://www.youtube.com/playlist?list=PLc7_sQ-Tb6e3siC4Cwu4JfxuqJZkGO6sy) that explains every step: from creating vegetation in Gimp to manually detailing cells and assembling the world. Since my goal was not to master manual editing, but to get a global understanding of the process, I didn't dig deeper into the UI specifics.

I also discovered that the official tools were forked from the open-source [Tiled editor](https://github.com/mapeditor/tiled). Even though I didn't study its codebase extensively, it gave me a good overview of the tools' capabilities.

### Analyzing the Render Engine

Browsing vanilla map files using the [PZ_Vanilla_Map-B41 repository](https://github.com/Unjammer/PZ_Vanilla_Map-B41-) was a gold mine. It helped me understand the game's render engine, specifically:

* How tiles are spread over a cell.
* How layers create a fake 3D impression (isometric perspective).
* How tiles are superimposed to add details like erosion or grime.

I then analyzed the `.tbx` and `.tmx` files to understand how they could be generated automatically. I grasped the cell storage system and how sprites are referenced centrally. Crucially, I discovered structured **building data**. This reminded me of the *7 Days to Die* Random World Generation (RWG) and confirmed I could scrape building data to inject them into procedural cell templates.

### First Pipeline Draft

I designed a preliminary procedural generation pipeline:

1. Creating noise-based wilderness.
2. Creating cell placeholders to welcome buildings from a custom database.
3. Connecting cells to create cities.
4. Placing wilderness prefabs (farms, campsites, etc.).
5. Connecting cities with highways and POIs with dirt roads.

The plan was to use custom file formats for core generation to optimize performance, and then serialize them into a format accepted by the official tools.

### The Binary Breakthrough

While searching for documentation on the binary map format, I found [pzmap2dzi](https://github.com/cff29546/pzmap2dzi), an open-source tool used to generate web map previews. Its source code contained vital information about the game's binary file structure. This allowed me to aim for a more ambitious pipeline: **generating game-ready files directly**, skipping the official mapping tools entirely.

### From Python to C#

I started **porting** the file parsers from Python to C#. While I am very comfortable with Python, I anticipated the scale of the project and opted for C# for its performance and strong typing.

Rewriting the parsers was the best way to deeply understand the file structure and how the engine generates the world at runtime. I broke down the hierarchy: Regions (Cells)  Chunks  Squares  Sprites.

The main files I focused on were:

* **lotheader:** Metadata for one cell (tile mapping, building info, and zombie spawn heatmaps compressed in a bit array).
* **lotpack:** The heavy map data using a custom RLE (Run-Length Encoding) array.
* **texturepack:** Tilesheet information and sprite coordinates.
* **tiledefinition:** Metadata defining player interactions (collisions, etc.).

**Validation Strategy:** To ensure my parsers were correct, I implemented a test: read a game file, rewrite it, and compare the **MD5 signature** of the output against the original. This gave me great confidence in the code.

### The Switch to Low-Level Languages (Rust & C++)

Once the logic was validated, I felt it was time to switch to a lower-level language, as the project would likely require high performance.

I was initially skeptical about C++ due to memory management complexity, so I gave **Rust** a try. I rewrote my parsers and enjoyed the experience: great memory safety and excellent tooling with Cargo (testing, linting, etc.).

However, for personal reasons, I ultimately decided to switch to **C++**. It is a "must-have" skill in game programming, and I wanted to improve my low-level skills. I knew it would make development harder, but I saw the benefit of learning to optimize memory manually and writing modern C++.

## December 2025

### Build System & Workflow

My first priority was to define a robust build pipeline capable of easily integrating third-party libraries. Initially, I considered using CMake with MSVC, as I primarily develop on Windows. However, I quickly pivoted to **MinGW64**. This decision was driven by the desire to anticipate a potential future Linux port and aligns with my preference for a lightweight VS Code environment over a heavy Visual Studio installation.

For dependency management, **vcpkg** appeared to be the industry standard. I paired it with CMake’s `FetchContent` command for libraries unavailable in the vcpkg registry. However, I am not entirely satisfied with this mixed management style. I plan to transition exclusively to `FetchContent` in the future to ensure cleaner, more reproducible builds and to reduce the number of required tools.

To speed up iteration and reduce friction during development, I also set up simple `.bat` scripts to automate common build tasks (build, clean-rebuild, build-and-run, etc.).

### Porting to C++

With the build pipeline ready, I performed the final port of the file parsers into C++. This was a good opportunity to refresh my C++ knowledge, integrate quality-of-life libraries such as `cpptrace` and `fmt`, and write my first C++ unit tests using `doctest`.

As the codebase grew, a cleanup became necessary. I implemented a basic architecture to support testing and separate core logic from entry points (GUI or CLI):

```text
├── data/           # Configuration, schemas, and data assets
├── helpers/        # Automation scripts (build, deploy, test)
├── lib/            # Core library: Serializers, PCG algorithms, Data services
├── main/           # Application entry points (CLI & SFML Viewers)
|   ├── cli/        # CLI commands
|   ├── gui/        # GUI definition
|   └── main.cpp    # CLI / GUI entry point
└── tests/          # Unit and Integration tests (targeting the lib)
```

### Tooling: Visualizing the Data

My first concrete need was a tool to visualize tile sheets through a user-friendly interface. This tool needed to allow navigation through tile attributes, such as positions and properties defined in `texturepack` and `tiledefinition` files. Such a viewer would make it much easier to explore game files, identify useful content, and gain a clearer understanding of how tiles are assembled in the game engine.

To implement these visual debugging tools, I evaluated three options: OpenGL, SDL2, and SFML. Since the goal was limited to rendering static 2D content, **SFML** appeared to be the most suitable choice. It is designed for 2D rendering, offers a high-level API friendly for C++ development, and is mature enough to provide good performance while leaving the door open to raw OpenGL if needed later.

For the UI layer, I compared the two major libraries available for SFML: **ImGui** and **TGUI**.

* **ImGui:** I tested this first. While I appreciated its simplicity for prototyping, I realized that managing complex interfaces within maintainable code would be difficult due to its Immediate Mode nature (no internal state retention). It is powerful for debugging but wasn't adapted to the persistent UI I needed.
* **TGUI:** I switched to TGUI, which integrates seamlessly with SFML and offers a traditional Object-Oriented API. This allows for a cleaner architecture when building complex interfaces.

With the workflow defined, I drafted the first version of the **Tilesheet Browser**. It served as a perfect exercise to learn TGUI and allowed me to deeply debug my file parsers, while extending my understanding of the game's tilesheets architecture.

### Rendering: The Naive Approach

The next milestone was displaying a single game cell. My first approach was naive: building an isometric grid directly from tiles stored in the texture pack files.

The process involved:

1. Browsing a `lotpack` file.
2. Retrieving the tilesheet based on the ID stored in the file.
3. Cutting the specific sprite from the tilesheet.
4. Drawing the sprite at the correct screen coordinates using standard isometric rules.

**The result:** I could display a full cell... at **6 FPS**.

### Optimization: Batching & Dynamic Atlases

The bottleneck was clearly the excessive amount of draw calls as every sprite was drawn independently. To solve this, I needed to use batching extensively.

However, the isometric perspective requires a strict drawing order for depth correctness. Simple batching by original game tilesheets was impossible; it would require complex logic to maintain the draw order (switching textures constantly) and would still result in excessive draw calls. I considered using a Z-Buffer, but it introduced complexity regarding 2D transparency sorting that I wanted to avoid at this stage.

**The Solution:** Create a single texture containing all sprites used in a specific cell, then use SFML Vertex Arrays to render the entire cell in a **single draw call**.

This introduced new challenges:

* Dynamically creating an atlas with thousands of sprites.
* Reducing the VRAM footprint of this massive atlas.
* Decompressing PNGs quickly to minimize cell load times.

To solve the decompression speed issue, I sacrificed initial application startup time (approx. 30s) to minimize the loading time of individual cells during runtime. All PNG data is decompressed into byte arrays and stored in RAM as `sf::Image` objects. This consumes a significant but acceptable amount of RAM (about **1.2 GB** for all used tilesheets), but makes the data instantly available for manipulation.

For the atlas generation, I used `rectpack2D`, a library implementing a fast rectangle-packing algorithm.

This decoupled the sprite representation from the image stored in RAM, allowing me to quickly construct a custom atlas for the cell. This enables drawing the entire cell in one draw call (or technically, one draw call per layer, to allow dynamic layer toggling).

**Performance Impact:**
This technique took me from **~20 FPS with 96k+ draw calls** to **700+ FPS with ~10 draw calls**.
*Disclaimer: These performance stats were recorded on an RTX 5070 rendering a single cell. The ultimate goal is to maintain at least 60 FPS while rendering many cells simultaneously.*

### Graph-Based Deduplication

Now that single-cell rendering is fast and functional, the next target is to draw multiple cells around the camera to allow for zooming out and previewing generated worlds as far as possible.

To optimize this, I am working on grouping dynamic cell atlases. The logic is simple: If Cell A contains all the sprites needed for Cell B, there is no need to reconstruct an atlas for Cell B. This should save significant VRAM when zoomed out.

I implemented a graph-based algorithm, pre-computed at map load:

1. Analyze each `lotheader` file.
2. Register every unique sprite ID used.
3. Check if another cell already contains this set of sprites.
4. If so, the "bigger" atlas becomes the parent.

By extending this recursively, I create a dependency graph where only the root nodes generate dynamic atlases, while leaf nodes reuse parent atlases to render their sprites.

## January 2026

### Refactoring CLI architecture 
- need to manage multiple windows within the same executable
- need to create small entry points quickly for debugging
- need an extensible architecture
- anticipating future dual mode: cli vs gui

### Roadmap
- [x] improving separation between cli and gui: allow to easily show tiles browser without separate executable
- [ ] create a main gui architecture, allowing to create multiple windows from the main gui
- [ ] implementing dynamic cells viewers
- [ ] implementing automated extraction of buildings
- [ ] implementing buildings viewer
- [ ] drafting pois injector architecture
- [ ] improving tiles browser
