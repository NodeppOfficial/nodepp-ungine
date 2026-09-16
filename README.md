# nodepp-Ungine: The Universal Nodepp Game Engine

Ungine is a high-performance, open-source game engine designed to simplify game development across 2D, 3D, and Virtual Reality platforms. Built on the speed of C++, Ungine leverages a modern node-based architecture and an asynchronous event system to help developers create stable, scalable, and complex games with minimal boilerplate code.

## 💡 Featured Project: Duck Hunt VR (WASM Edition)

To showcase [Nodepp for Web](https://github.com/NodeppOfficial/nodepp-wasm), we ported the classic Duck Hunt to Virtual Reality, running natively in the browser via WebAssembly. This project pushes the limits of web-based VR by combining low-level C++ performance with modern Web APIs.

https://github.com/user-attachments/assets/ab26287e-bd73-4ee8-941b-d97382e203c9

Play it now: [Duck Hunt VR on Itch.io](https://edbcrepo.itch.io/duck-hunt-vr)

## 💡 Featured Project: Cursed-Luna (WASM Remake)
Cursed-Luna is a high-performance remake of the Global Game Jam 2018 classic. Originally built around the theme of Transmission, this version has been completely rewritten in C++ using Nodepp and Raylib to bring tight gameplay mechanics to the browser via WebAssembly.

https://github.com/user-attachments/assets/3647b5b6-fbfd-4281-af0f-f35f3260a319

Play it now: [Cursed-Luna on Itch.io](https://edbcrepo.itch.io/cursed-luna)

## Key Features
### Performance & Architecture

- **Built on C++:** Harness the raw speed and efficiency of C++ for demanding applications, ensuring high frame rates and quick loading times.
- **Asynchronous Core:** Utilizes an internal Event Loop for efficient task management, enabling highly concurrent and non-blocking I/O.
- **Node-Based Scene Graph:** Every game object—from players and bombs to cameras and scenes—is a node_t, allowing for clear, hierarchical organization and easy inheritance of properties.
- **Smart Memory Management:** Employs a Smart Pointer Garbage Collector system to drastically reduce memory leaks and simplify resource handling compared to traditional C++.

## Dependencies & Cmake Integration
```bash
include(FetchContent)

FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

FetchContent_Declare(
	nodepp-raylib
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-raylib
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-raylib)

#[...]

target_link_libraries( #[...]
	PUBLIC nodepp nodepp-raylib #[...]
)
```

## Developer Experience

Familiar Scripting Style: The architecture offers a syntax and structure that is easy to learn for developers coming from environments like Node.js or similar high-level languages.

- **Built-in Utility:** Includes essential tools such as a JSON parser/stringify system and a powerful RegExp engine.
- **Lifecycle Hooks:** Clear object lifecycle management through powerful callbacks:
- **onLoop(float delta):** For frame-by-frame physics and game logic updates.
- **on2DDraw():** For 2D rendering and drawing operations.
- **on3DDraw():** For 3D rendering and drawing operations.
- **onUIDraw():** For UI rendering and drawing operations.
- **onClose():** For clean-up and resource deallocation.

## Compile & Run

```bash
🐧: g++ -o main     main.cpp -L./lib -I./include -lraylib ; ./main
🪟: g++ -o main.exe main.cpp -L./lib -I./include -lraylib -lws2_32 -lwinmm -lgdi32; ./main.exe
```

## Hello World

```cpp
#include <nodepp/nodepp.h>
#include <ungine/ungine.h>

using namespace nodepp;
using namespace ungine;

global_t global;

void onMain() {

    engine::start( 800, 600, "Game" );
    engine::set_fps( 60 ); 

    window::set_min_size({ 800, 600 });

    window::mode::set( 
        ungine::window::WINDOW_ALWAYS_RUN |
        ungine::window::WINDOW_RESIZABLE
    );

    node::node_scene([=]( ref_t<node_t> self ){

        self->on2DDraw([=](){
            rl::ClearBackground( rl::BLACK );
            rl::DrawText( "hello world", 12, 12, 12, rl::WHITE );
        });

    });

}
```

## Contribution

If you want to contribute to **Nodepp**, you are welcome to do so! You can contribute in several ways:

- ☕ Buying me a Coffee
- 📢 Reporting bugs and issues
- 📝 Improving the documentation
- 📌 Adding new features or improving existing ones
- 🧪 Writing tests and ensuring compatibility with different platforms
- 🔍 Before submitting a pull request, make sure to read the contribution guidelines.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/edbc_repo)

## License
**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.