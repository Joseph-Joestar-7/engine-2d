
# 2D Game Engine

A modular 2D engine implementing an ECS architecture, developed in C++ with SFML for rendering and input.
It’s not a full-scale engine, but it provides all the core systems needed to build small-to-medium 2D games.

## Features

### 🎮 Entity–Component–System Architecture

Every game object is treated as an Entity composed of modular Components.

Core components include:
CTransform, CInput, CLifespan, CAnimation, CState, CBoundingBox, CGravity, CScore

### ⚡ Action System (Input Handling)

A dedicated Action class stores input actions similar to UE5’s InputAction objects.

Scenes map actions to behavior, allowing flexible and scene-specific input handling.

### 📦 Asset Management

All assets are defined in a simple text file: assets.txt

Supports multiple asset types (textures, fonts, animations) through a clean, declarative format.

### 🗺️ Level Loading

Levels are described using external text files (e.g., Level1.txt, Level2.txt).

Levels specify what entities to spawn, their components, and their initial data.

### 🧩 Systems

Update systems manage game logic, rendering, input, physics, animation, etc.

Each system operates independently on matching components.

### 🔧 Physics Utilities

Collision / overlap detection

Bounding box interactions

Simple gravity support through CGravity
