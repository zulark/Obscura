# Obscura - Project Summary

## 🎮 Project Overview

**Obscura** is a top-down action RPG game developed in C using the Raylib graphics library. The project draws inspiration from popular games like Dark Souls and Vampire Survivors, featuring wave-based combat, character progression, and boss battles. Originally created as a project for an Advanced Programming course, Obscura demonstrates solid software engineering principles with modular architecture and well-organized codebase.

## 🛠️ Technical Specifications

### **Technology Stack**
- **Language**: C (ISO C standard)
- **Graphics Engine**: Raylib 4.x
- **Build System**: Makefile (MinGW/GCC compatible)
- **Platform**: Windows (with potential for cross-platform support)
- **Code Scale**: ~8,400 lines of code across 12 source files and 20+ header files

### **Architecture Overview**
The project follows a modular architecture with clear separation of concerns:

```
📁 Project Structure
├── src/           # Core game logic (12 C files)
├── include/       # Header files and interfaces (20+ H files)
├── assets/        # Game resources (sprites, sounds, icons)
├── bin/          # Compiled executable and DLLs
├── obj/          # Temporary build objects
└── lib/          # External libraries
```

### **Core Systems**
- **Game Loop**: Main game loop with state management
- **Rendering**: 2D graphics with camera system and viewport management
- **Physics**: Collision detection and response
- **Audio**: Modular sound system with volume/pitch control
- **Input**: Mouse and keyboard handling
- **Memory Management**: Proper resource allocation and cleanup

## 🎯 Gameplay Features

### **Core Mechanics**
- **Movement**: WASD character control with mouse-aim
- **Combat**: 
  - Basic projectile attacks (left-click)
  - Area damage magic (right-click)
  - Dash ability (Spacebar)
  - Barrier magic (E key)
- **Enemy System**: Multiple enemy types with distinct behaviors
- **Boss Battles**: Multi-phase boss encounters with unique attack patterns
- **Wave System**: Progressive enemy waves with increasing difficulty

### **Progression System**
- **Experience Points**: Gained by defeating enemies
- **Level Up**: Character advancement with attribute bonuses
- **Skill Points**: Currency for character upgrades
- **Magic System**: Hotkey-based spell casting with cooldowns
- **Health/Mana**: Resource management mechanics

### **User Interface**
- **HUD Elements**: Health bar, XP bar, level display, skill points, FPS counter
- **Menus**: Pause menu, game over screen, upgrade menu
- **Visual Feedback**: Floating damage text, particle effects, screen effects
- **Custom Cursor**: Crosshair-style targeting system

## 🎨 Visual & Audio Design

### **Graphics**
- **Art Style**: Pixel art sprites with smooth animations
- **Particle System**: Visual effects for spells, level-ups, and impacts
- **Animation**: Frame-based sprite animation system
- **Camera**: Smooth following camera with deadzone and boundaries

### **Audio**
- **Sound Effects**: Combat sounds, UI feedback, environmental audio
- **Music**: Background music and contextual audio
- **Audio Control**: Volume and pitch manipulation for dynamic effects

## 🏗️ Development Status

### **Current State**
- ✅ **Core Gameplay Loop**: Fully functional
- ✅ **Combat System**: Complete with projectiles and magic
- ✅ **Enemy AI**: Multiple enemy types with pathfinding
- ✅ **Boss System**: Multi-phase boss with various attack patterns
- ✅ **Progression**: XP, leveling, and skill point systems
- ✅ **Audio/Visual**: Complete feedback systems
- ✅ **UI/UX**: Functional menus and HUD

### **Code Quality**
- **Modularity**: Well-separated systems (player, enemy, boss, UI, audio, etc.)
- **Documentation**: Comprehensive README with build instructions
- **Build System**: Automated compilation with debug/release modes
- **Resource Management**: Proper asset loading and cleanup
- **Error Handling**: Defensive programming practices

## 🚀 Future Development (Roadmap)

### **Planned Features**
- **Enhanced Magic System**: More spells and drag-&-drop hotkey customization
- **Item System**: Equipment, consumables, and power-ups
- **Save/Load**: Game progress persistence
- **Advanced Boss Mechanics**: More complex boss behaviors
- **Difficulty Scaling**: Dynamic or selectable difficulty modes
- **Enhanced AI**: Improved enemy intelligence and behaviors

### **Technical Improvements**
- **Performance Optimization**: Frame rate improvements and memory optimization
- **Cross-Platform Support**: Linux and macOS compatibility
- **Graphics Enhancements**: Advanced visual effects and shaders
- **Audio Expansion**: More sophisticated audio mixing and effects

## 🎯 Educational Value

This project demonstrates several advanced programming concepts:

- **Game Development Patterns**: Entity-Component systems, state machines, object pooling
- **Graphics Programming**: 2D rendering, animation systems, particle effects
- **Software Architecture**: Modular design, separation of concerns, clean interfaces
- **Resource Management**: Asset loading, memory management, cleanup
- **Real-time Systems**: Game loops, timing, performance considerations
- **User Experience Design**: UI/UX principles, accessibility, feedback systems

## 📊 Project Metrics

- **Total Lines of Code**: ~8,400
- **Source Files**: 12 C files, 20+ header files
- **Asset Files**: 50+ sprites, sounds, and icons
- **Build Time**: <30 seconds on modern hardware
- **Executable Size**: ~2MB (excluding assets)
- **Development Time**: Academic semester project

## 🏆 Achievements

Obscura successfully demonstrates:
- **Complete Game Loop**: From menu to gameplay to game over
- **Professional Code Organization**: Industry-standard modular architecture
- **Feature Completeness**: All major systems implemented and functional
- **User Experience**: Polished UI with proper feedback and effects
- **Technical Competency**: Solid C programming and graphics programming skills

---

*Obscura represents a comprehensive example of game development in C, showcasing both technical programming skills and game design principles. The project serves as an excellent foundation for further game development learning and could be extended into a commercial-quality game with additional features and polish.*