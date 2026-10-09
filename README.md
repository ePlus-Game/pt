# Phong Than 2 — legacy source archival / analysis

An **incomplete, unverified** historic C/C++ source snapshot for research into Phong Than 2 / 封神榜2. This repository is **not** a working offline release.

## Layout
- `Base/` — common code and Lua libraries
- `Client/` — client, engine, rendering and UI projects
- `Server/` — lord launcher project; dependent library sources are missing
- `Share/` — shared headers and prebuilt libraries
- `docs/PT_source_analysis.md` — investigation and missing dependencies

## Current build progress
- One-shot portable source build (legacy Lua + CRC32/miniLZO subset) with CMake, CTest and GitHub Actions; **not** a playable client/server.
- Combined dependency and asset inventory reports, plus a Windows VC6 multi-project build-attempt script.
- See [Offline recovery guide](docs/OFFLINE_RECOVERY.md) for the full one-pass workflow.

## Known blockers
- Requires old 32-bit Windows/Visual C++ toolchains and dependencies.
- `Game.dll`, `FSInterface.dll`, full server executables and the `fsonline2` database dump are absent.
- No confirmed working offline environment or build pipeline.
- `Server/lord/src/lord.cfg` deliberately excluded because the archived file contained connection credentials; use the redacted `lord.cfg.example` and local settings instead.

## Security / rights
- **Legacy code** may contain unsafe functions and stale dependencies; inspect and build inside an isolated VM.
- Source contains historical Kingsoft copyright notices; the archive did not include a grant to publish/relicense the proprietary game code. **Do not make a public distribution without the rights holder's permission.** This README does not confer a license.

## Import
A local PowerShell helper `push-to-github.ps1` is included for a repository whose remote has no commits. It will never force-push. Review source code and publishing rights before use.
