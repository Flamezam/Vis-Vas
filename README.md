# MyApp

Minimal C++20 project scaffold that vendors, via CMake `FetchContent`:

- [raylib](https://github.com/raysan5/raylib) — windowing / rendering
- [Dear ImGui](https://github.com/ocornut/imgui) — UI, using the
  [rlImGui](https://github.com/raylib-extras/rlImGui) raylib backend
- [Lua](https://github.com/lua/lua) — lua scripting
- [nlohmann/json](https://github.com/nlohmann/json) — JSON

All sources under `src/` are picked up automatically and compiled into
one executable. `src/main.cpp` is currently a minimal program that uses all
four libraries at once.

## Prerequisites

- CMake 3.20+
- Git (CMake shells out to it to fetch dependencies)
- A C++20 compiler:
  - Windows: Visual Studio 2022 (or the Build Tools for it)
  - Linux: GCC 11+ or Clang 14+
  - macOS: Xcode command line tools
- **Linux only** — raylib needs some system dev packages to build:
  ```bash
  sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev \
                    libxinerama-dev libxcursor-dev libxi-dev libwayland-dev libxkbcommon-dev
  ```
- An internet connection the *first* time you configure (dependencies
  are cloned into `build/_deps/` and cached there — later configures
  are offline).

## Windows — generate and use the .sln

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

This clones and builds raylib/imgui/rlImGui/lua/json, then generates
`build/MyApp.sln` (inside `build` folder). Open it in Visual Studio and hit Run — `MyApp` is
already set as the startup project. Or build from the command line:

```bat
cmake --build build --config Release
```

The executable ends up in `build/bin/Release/MyApp.exe`.

## Linux / macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/MyApp
```

(Pass `-G Ninja` to the first command if you have Ninja installed, for
faster builds.)

## Project layout

(To be made when we have scaffold)

Add more `.cpp`/`.c` files anywhere under `src/` and they're picked up
automatically on the next CMake configure (re-run `cmake -S . -B build`
if you add a *new* file so it's detected).

