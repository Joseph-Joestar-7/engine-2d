# 2D Game Engine

A modular 2D engine implementing an Entity–Component–System architecture, developed in C++ with SFML for rendering and input.
It’s not a full-scale engine, but it provides all the core systems needed to build small-to-medium 2D games. 

## Features

### 🎮 Entity–Component–System Architecture

Every game object is treated as an Entity composed of modular Components.

Core components include:
CTransform, CInput, CLifespan, CAnimation, CGameplayTags, CBoundingBox, CGravity, CScore

### ⚙️ Components

- **CTransform** – Tracks position, previous position, velocity, and rotation.  
- **CInput** – Marks entities as player or AI controllable.  
- **CLifespan** – Manages time-to-live or destruction timing.  
- **CAnimation** – Handles sprite or frame animations.  
- **CGameplayTags** – Assigns tags for easy filtering or logic checks.  
- **CBoundingBox** – Defines collision boundaries.  
- **CGravity** – Applies gravitational forces to the entity.  
- **CScore** – Stores and manages scoring variables.

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

- **SMovement** – Updates entity positions based on velocity and physics.  
- **SAnimation** – Updates sprite/frame animations each tick.  
- **SCollision** – Checks and resolves collisions between entities.  
- **SLifespan** – Handles entity destruction when lifespan expires.  
- **SRender** – Draws entities to the screen.  
- **SDoAction** – Processes input or AI actions using Action objects.  
- **SCamera** – Controls camera behavior based on `m_CameraType`.

### 🔧 Physics Utilities

- Collision / overlap detection
- Bounding box interactions
- Simple gravity support through CGravity
