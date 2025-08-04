# 🌍 3D Space Exploration Game

![3D Scene Preview](./scenes_inf443/project/screenshot_gameplay.png)

A 3D interactive simulation featuring the Earth, atmosphere, and an orbiting airplane, built using an OpenGL library. Fly around the globe, manipulate lighting, and explore space from multiple camera perspectives.

---

## 📌 Update (August 3rd, 2025)

This repository is now **public** for YC examiners and others interested in the project.

✅ A **precompiled executable** has been added, allowing you to **launch the game directly** without compiling.  
Check the [`/scenes_inf443/project`](./scenes_inf443/project) folder or see [Quick Start](#quick-start) below.

---

## 🚀 Quick Start

1. **Download the executable** from the [`/scenes_inf443/project`](./scenes_inf443/project) folder.
2. **Launch** the game by double-clicking the executable.
3. **Explore the scene** using your keyboard and mouse (see [Controls](#controls) below).
4. *No installation or setup needed.*

> ℹ️ If you prefer to compile from source, see the [Build from Source](#build-from-source) section.

---

## 🎮 Controls

| Action                                | Control Key / Input                       |
|--------------------------------------|-------------------------------------------|
| Move airplane (forward)              | Automatic                                 |
| Turn airplane (left/right)           | Left / Right arrow keys                   |
| Adjust airplane altitude             | Up / Down arrow keys                      |
| Switch camera view                   | Use GUI to select desired camera          |
| Move the Sun (**overview** mode)     | **Shift + Left Mouse Click & Drag**       |
| Zoom camera (**overview** mode)      | Hold **right click** + drag               |
| Move camera (**overview** mode)      | Hold **left click** + drag                |

---

## ✨ Features

* 🌞 **Interactive lighting system** - The Sun serves as the main light source, casting realistic shadows and highlights on the Earth and airplane.            Users can interactively move it.
* 🌍 **Textured Earth model** - Textured sphere with continents, oceans, forming the central body around which everything is built.
* ✈️ **User-controlled flight** – An orbiting airplane that users can control with keyboard inputs. You can steer, ascend, or descend to explore the           Earth more closely.
* 🌌 **Immersive space environment** – The entire scene is wrapped in a starry skybox, adding depth and realism to the space setting.
* 🎥 **Multiple camera modes** –  
  - First-person view (when **nothing is checked** in the GUI)  
  - Top-down view (check `"vue_2_haut"`)  
  - Free orbit camera (check `"overview"`)
* 🖱️ **Intuitive GUI** – Switch views and interact with the scene in real time.

---

## 🧱 Build from Source (using `cgp` library)

> *(Only needed if you don’t use the precompiled executable)*

### 🔧 Requirements

Before you begin, make sure you have the following installed:

* [Visual Studio](https://visualstudio.microsoft.com/) (with **Desktop Development with C++** workload)(for windows)
* [CMake](https://cmake.org/download/)
* [Git](https://git-scm.com/)
* OpenGL (it is included via Windows/Visual Studio by default)

The `cgp` library already includes necessary dependencies (GLFW, GLEW, GLM, etc.) — no need to install them separately.
see [**cgp** library by David Rohmer](https://github.com/drohmer/cgp) for proper informations and clear instructions on how to compile it.

### 🔧 builsing the game executable

**Steps (on Unix-based operating systems):**

```bash
git clone https://github.com/Me3sP/computer-graphics-avion-game.git
cd computer-graphics-avion-game
mkdir build
cd build
cmake ../scenes_inf443/project/
make
```

---

**Steps (on Windows):**

   ```bash
   git clone https://github.com/Me3sP/computer-graphics-avion-game.git
   cd computer-graphics-avion-game
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

   > Replace the generator name if using a different Visual Studio version.

5. **Open and build in Visual Studio**

   * Open `project.sln` inside the `build` folder
   * Set the startup project to your executable target (e.g., `/scenes_inf443/project/src/scene.cpp`)
   * Select `Release` or `Debug` build
   * Press `Ctrl + F5` to build and run
