# Animated 3D Bar Chart Visualisation Tool

A C++ application for visualising time-evolving data as animated 3D bar charts, optionally overlaid on a Mercator-projected map.

Developed for the course of Computer Graphics (Digital Logic Design) y. 2023 at Politecnico di Milano.

**Features**  
- Animated 3D rendering with Vulkan
- Visual effects via GLSL shaders
- 3D transformations using GLM

## Build Instructions

To build the program:
1. download or clone this repository;
2. enter repository main directory;
3. run `make build`;
4. executable will be in `bin/exec.out`.


## Input files

Without map overlay:
- **data**: csv file containing time evolving data to render:
  - rows: time steps;
  - columns: entries (e.g., categories, items, etc.);

With map overlay:
- **data**: csv file containing time evolving data to render:
  - rows: time steps;
  - columns: entries (e.g., categories, items, etc.);
- **coordinates**: csv file with coordinates where to display each entry;
- latitude column: index of column contain the latitude in the coordinates csv file;
- longitute column: index of column contain the longitude in the coordinates csv file;
- **map**: mercator-projected image of the map (e.g. a screenshot from Google Maps or OpenStreetMap);
- coordinates of each border of the map image;
- map scale: parameter controlling the dimension of the rendered map.


## Controls

| Action               | Key / GUI       |
| -------------------- | --------------- |
| Play / Pause time    | `Space` / ▶ ⏸  |
| Toggle auto-rotation | `Q` / ⟳         |
| Zoom in / out        | `W` `S`       |
| Manual rotation      | `←` `→`         |
| Change inclination   | `↑` `↓`         |

Controls can also be performed using mouse, trackpad, or a joystick.

## Examples

[data](data) and [textures](textures) folders contain respectively examples of input csv and map image.
