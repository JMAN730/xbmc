# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is Kodi (formerly XBMC), an open source (GPLv2) cross-platform media center. The codebase is primarily C++ (targeting **C++20** — do not use C++23 features), built with CMake, and runs on Linux, Windows, macOS, iOS, tvOS, Android, webOS and FreeBSD.

## Build Commands

Kodi uses out-of-source CMake builds. Platform-specific guides live in `docs/README.<Platform>.md`; the Linux flow is:

```sh
mkdir ../kodi-build && cd ../kodi-build

# Configure (pick platform/render system as needed)
cmake ../xbmc -DCMAKE_INSTALL_PREFIX=/usr/local -DCORE_PLATFORM_NAME=x11 -DAPP_RENDER_SYSTEM=gl
# CORE_PLATFORM_NAME can be any combination of: x11 wayland gbm
# APP_RENDER_SYSTEM: gl or gles

# Build
cmake --build . -- -j$(getconf _NPROCESSORS_ONLN)
```

Missing dependencies can be built in-tree by appending `-DENABLE_INTERNAL_<DEP>=ON` (e.g. `ENABLE_INTERNAL_FMT`, `ENABLE_INTERNAL_FLATBUFFERS`, `ENABLE_INTERNAL_SPDLOG`) or via `tools/depends/target/<dep>`.

Binary add-ons (in-tree): `make -C tools/depends/target/binary-addons PREFIX=/usr/local ADDONS="pvr.*"`

## Tests

Tests use Google Test and are compiled into a single `kodi-test` binary. Test sources live in `test/` subdirectories next to the code they test (e.g. `xbmc/utils/test/TestStringUtils.cpp`) and are registered in the module's `test/CMakeLists.txt` via `core_add_test_library`.

From the build directory:

```sh
make kodi-test          # build the test suite without running
make check              # build and run all tests (ctest)
./kodi-test --gtest_filter=TestStringUtils.*        # run one suite
./kodi-test --gtest_filter=TestURIUtils.GetFileName # run a single test
```

## Code Style

`.clang-format` in the repo root is authoritative; the PR CI job enforces it **per commit** on changed code. Full rules in `docs/CODE_GUIDELINES.md`. Key points:

- 2-space indentation, no tabs; braces always on their own line; 100-column limit.
- Namespaces are not indented; wrap `.cpp` contents rather than indenting.
- Naming: classes `CFoo`, interfaces `IFoo`, structs `Foo` (no prefix), methods `PascalCase`, member variables `m_fooBar`, global variables `g_fooBar`, constants `UPPER_CASE`, namespaces `UPPER_CASE` (`KODI::UTILS`).
- Header include order: own header first, then system, then other Kodi headers (clang-format handles this).
- Use `#pragma once`, `nullptr`, `static_cast` (never C-style casts), `override` on virtuals.
- Every source file carries the Team Kodi GPL-2.0-or-later SPDX license header.
- Logging via `CLog::Log(LOGDEBUG, ...)` / spdlog-style formatting; strings via `StringUtils`.
- Don't mix functional changes and unrelated cosmetic reformatting in the same commit.

## Commit / PR Conventions

- Titles follow `[Component(s)] Short description` or `Component(s): Short description` (e.g. `[Python][xbmcgui] Fix ...`).
- All development targets the `master` branch; use topic branches, keep commits squashed into logical chunks.

## Architecture

All application source is under `xbmc/`. `cmake/` holds the build system (`cmake/scripts/common/Macros.cmake` defines `core_add_library`/`core_add_test_library` — each source directory is a static "core library" listed in its own `CMakeLists.txt`, all linked into the final binary). `tools/depends/` cross-compiles dependencies for non-desktop platforms. `addons/` contains bundled XML/Python add-ons and skins; `system/` and `userdata/` hold runtime configuration shipped with the app.

### Service access: CServiceBroker / CServiceManager

Kodi is largely a set of singleton-like services. `xbmc/ServiceManager.{h,cpp}` owns service lifetime (staged init in `InitStageOne/Two/Three`), and **`xbmc/ServiceBroker.h` is the global service locator** used throughout the codebase (e.g. `CServiceBroker::GetSettingsComponent()`, `CServiceBroker::GetGUI()`, `CServiceBroker::GetPVRManager()`). New cross-cutting services get registered in CServiceManager and exposed via CServiceBroker.

### Application layer

`xbmc/application/Application.cpp` (`CApplication`, alias `g_application`) is the central application object: main loop, player lifecycle, event routing. Its functionality is split into components (`CApplicationPlayer`, `CApplicationPowerHandling`, `CApplicationVolumeHandling`, etc.) retrieved through `GetComponent<T>()`. Platform entry points live under `xbmc/platform/*/main*`.

Cross-thread communication goes through `KODI::MESSAGING::CApplicationMessenger` (`SendMsg`/`PostMsg` with `TMSG_*` ids). GUI messages use `CGUIMessage`. Event notifications to interested subsystems (JSON-RPC clients, Python add-ons, etc.) go through `CAnnouncementManager` (`xbmc/interfaces/AnnouncementManager.h`).

### Major subsystems (under `xbmc/`)

- `guilib/` — the GUI toolkit and skinning engine: `CGUIWindowManager`, `CGUIWindow`, `CGUIControl` hierarchy, `CGUIControlFactory` parses skin XML. Concrete windows/dialogs live in `windows/`, `dialogs/`, and per-feature dirs (e.g. `video/windows/`, `music/dialogs/`, `settings/windows/`). `GUIInfoManager` + `interfaces/info/` resolve skin "info labels/booleans".
- `windowing/` — platform window-system/display abstraction (`CWinSystemBase`; X11, wayland, gbm, win32, osx, android, ...); `rendering/` is the GL/GLES/DirectX render system.
- `cores/` — playback engines: `VideoPlayer/` (FFmpeg-based demux/decode/render, the main player), `paplayer/` (audio), `RetroPlayer/` (game/emulator playback), `AudioEngine/` (audio output/mixing, `IAE`), `playercorefactory/` (player selection).
- `filesystem/` — the virtual file system. Everything is accessed via `CURL`-style URLs through `XFILE::CFile`/`CDirectory` with protocol implementations (smb, nfs, http, zip, addons, ...). `CFileItem`/`CFileItemList` (in `xbmc/`) are the universal "media item" currency passed around the entire app.
- `addons/` — add-on management (`CAddonMgr`, database, installer, repositories). `addons/kodi-dev-kit/` defines the stable binary add-on C API; `addons/interfaces/` bridges it into Kodi internals.
- `interfaces/` — external scripting/control surfaces: `python/` + `legacy/` + `swig/` (Python API generated via SWIG in `tools/codegenerator`), `json-rpc/` (remote control API), `builtins/` (skin-invocable built-in commands).
- `pvr/`, `music/`, `video/`, `pictures/`, `games/` — feature verticals; music/video library scanning/scraping backed by databases in `*/Database*` using `dbwrappers/` (SQLite/MySQL).
- `settings/` — XML-driven settings system (`CSettingsComponent`, definitions in `system/settings/settings.xml`).
- `platform/` — OS-specific implementations (posix, linux, win32, darwin, android, ...); code here is selected by CMake per-platform. Platform-independent code must not include from it directly.
- `peripherals/`, `input/` — device (joystick/CEC/keyboard) handling and keymapping; `network/` — networking services (web server, UPnP, airplay, zeroconf).
- `utils/` — shared helpers (`StringUtils`, `URIUtils`, `CJob`/`CJobManager` for async work, `CVariant`, logging); `threads/` — threading primitives (`CThread`, `CEvent`, `CCriticalSection`).
