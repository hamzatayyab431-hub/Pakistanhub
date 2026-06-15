# PakistanHub 🇵🇰

A feature-rich social media desktop application built with **C++ (OOP)** and **SFML**, styled with a **glassmorphism** design language using green and white accents — inspired by Pakistan's national colors.

---

## 📌 Project Overview

PakistanHub is a local social media clone where users can register, post, follow others, and interact through a fully graphical SFML-powered interface. All data is persisted to local files. The codebase is structured using core Object-Oriented Programming principles: encapsulation, inheritance, polymorphism, and abstraction.

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| Language | C++17 |
| Graphics | SFML 2.5+ |
| Paradigm | Object-Oriented Programming |
| Persistence | File I/O (CSV / plaintext) |
| Build | g++ / Makefile |

---

## 🎨 Design Language

- **Theme:** Glassmorphism — frosted glass panels, translucency, blur-like layering via SFML draw order
- **Primary Accent:** `#00A651` (Pakistan Green)
- **Secondary Accent:** `#FFFFFF` (White)
- **Background:** Deep dark base (`#0D1117`) with semi-transparent card overlays
- **Typography:** Clean sans-serif via SFML fonts

---

## 🗂️ Project Phases

| Phase | Description | Status |
|---|---|---|
| Phase 1 | Project setup, folder structure, base classes, Makefile | ✅ |
| Phase 2 | User system — register, login, profiles, file persistence | ✅ |
| Phase 3 | Core feed — create posts, display feed, like posts | ✅ |
| Phase 4 | Social graph — follow/unfollow, follower feed filtering | ✅ |
| Phase 5 | UI polish — glassmorphism panels, animations, transitions | ✅ |
| Phase 6 | Comments, notifications, search | ✅ |
| Phase 7 | Final integration, edge-case handling, cleanup | ✅ |

---

## 📁 Folder Structure

```
PakistanHub/
├── assets/
│   ├── fonts/
│   └── icons/
├── data/
│   ├── users.txt
│   ├── posts.txt
│   └── follows.txt
├── include/
│   ├── User.h
│   ├── Post.h
│   ├── Feed.h
│   ├── SocialGraph.h
│   ├── UIComponent.h
│   └── App.h
├── src/
│   ├── User.cpp
│   ├── Post.cpp
│   ├── Feed.cpp
│   ├── SocialGraph.cpp
│   ├── UIComponent.cpp
│   ├── App.cpp
│   └── main.cpp
├── Makefile
└── README.md
```

---
## 👤 Author

**Hamza** —FAST LAHORE 
Project: OOP C++ Social Media Clone 
