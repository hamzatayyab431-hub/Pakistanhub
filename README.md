# PakistanHub 🇵🇰


*A high-performance, fully graphical social media desktop application built from scratch in C++ using SFML.*

PakistanHub is an interactive, localized social network clone offering a modern user experience with **glassmorphism** design principles, smooth animations, and a rich dark-mode interface. Built entirely as an Object-Oriented C++ application, it brings web-like UI aesthetics to a native desktop environment.

---

## ✨ Features

- **Modern UI/UX:** Stunning "glassmorphism" panels, frosted glass overlays, custom rounded-corner rendering, and dynamic hover animations.
- **Complete User System:** Register, login, and customize your profile (Bio, City, and beautiful Avatar & Cover integrations).
- **Social Graph:** Follow/unfollow users and see follower/following counts update in real-time.
- **Interactive Feed:** Create posts, like posts, and view a timeline populated by the people you follow.
- **Engagement:** Full commenting system on individual posts.
- **Data Persistence:** All users, posts, comments, and social graphs are saved locally via structured File I/O.
- **Search & Explore:** Discover other users on the platform.

---

## 🛠️ Technology Stack

| Component | Technology / Concept |
|---|---|
| **Core Language** | `C++17` |
| **Graphics Engine** | `SFML 2.5+` (Simple and Fast Multimedia Library) |
| **Architecture** | Object-Oriented Programming (Polymorphism, Inheritance, Encapsulation) |
| **Data Storage** | Local File System (Text/CSV based) |
| **Build System** | GNU Make (`Makefile`) & MinGW `g++` |

---

## 🎨 Design Language

PakistanHub breaks away from traditional native UI limits by implementing a customized rendering engine on top of SFML:
*   **Theming:** Deep navy/gray background (`#0D1117`) paired with a vibrant Pakistan Green accent (`#00A651`).
*   **Advanced Rendering:** Uses `sf::ConvexShape` algorithms to eliminate alpha-blending artifacts on translucent rounded rectangles.
*   **Typography:** Clean sans-serif integration with full UTF-8 support for emojis (like ♥ and ♡).

---

## 🚀 How to Run (Windows)

### Prerequisites
*   **MinGW-w64** installed and added to your system PATH.
*   **SFML 2.5.1** (GCC MinGW version) installed and properly configured, or its binaries placed in the working directory/PATH.

### Build and Execute
1. Clone or download the repository.
2. Open a terminal (PowerShell or CMD) in the project root directory.
3. Compile the project using the included Makefile:
   ```bash
   mingw32-make
   ```
4. Run the compiled executable:
   ```bash
   ./PakistanHub.exe
   ```

*(Note: To perform a clean rebuild, run `mingw32-make clean` first).*

---

## 📁 Directory Structure

```text
PakistanHub/
├── assets/         # Fonts, icons, generated cover & avatar images
├── data/           # Persistent storage (users.txt, posts.txt, etc.)
├── include/        # C++ Header files (.h)
├── src/            # C++ Source files (.cpp)
├── Makefile        # Build instructions
└── README.md       # Project documentation
```

---

## 👤 Author

**Hamza** — FAST NUCES, Lahore  
*Project: OOP C++ Social Media Clone*
