# Project Maximus (V3)
> Qt 6 + QML in-car diagnostics dashboard for a 2021 Mazda CX-5 Turbo.

---

## Hardware Target

| Component | Detail |
|-----------|--------|
| Vehicle | 2021 Mazda CX-5 Turbo |
| Compute | Raspberry Pi (in-cabin) |
| Display | Corsair Xeneon Edge 14.5" / 2560×720 — replaces the factory rearview mirror |

## Tech Stack

- **Qt 6.5+** — core framework
- **QML** — declarative UI
- **CMake 3.21+** — build system
- **C++20** — application logic and data model

---

## Build & Run (macOS)

```bash
brew install qt@6 cmake
cmake -B build -S . -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
cmake --build build -j
./build/maximus
```

Qt is detected automatically from `/opt/homebrew/opt/qt@6` on Apple Silicon.
Pass `-DCMAKE_PREFIX_PATH=<your-qt-path>` if your Qt installation is elsewhere.

---

## Notes

Design docs, architecture decisions, and planning notes live in the Obsidian vault, not this repo.
