# Phong Thần 2: integrated offline recovery workflow

This repo holds a **partial** historical source snapshot, not a playable offline game. Do not claim full restoration until the original client successfully logs into a local server and enters a map.

## One-shot checks on Linux / CI

From repository root:

```sh
python -m unittest discover -s tests -v
python tools/audit_legacy_projects.py --json reports/projects.json --markdown reports/projects.md
python tools/recovery_inventory.py --json reports/dependencies.json --markdown reports/dependencies.md
cmake -S . -B build/recovery -DCMAKE_BUILD_TYPE=Release
cmake --build build/recovery --parallel
ctest --test-dir build/recovery --output-on-failure
```

This builds two isolated **portable subsets**:

- `pt_legacy_lua` — legacy Lua C sources.
- `pt_common_portable` — only CRC32 and miniLZO, **not** the complete Windows/Common module.

CRC32 is validated using the canonical `123456789` CRC32 value `CBF43926`. miniLZO is compiled but not exercised at runtime. The original `CRC32.C` is copied to `.c` **inside the build directory** because GCC interprets uppercase `.C` as C++.

CI stores `projects.json`, `projects.md`, `dependencies.json`, and `dependencies.md` as downloadable workflow artifacts.

## One-shot Visual C++ 6.0 build attempts (Windows x86 VM)

The original game source uses MSVC6 Win32/x86 and prebuilt Windows `.lib` dependencies. Neither a Linux GCC archive nor a modern MSVC archive should be represented as link-compatible with the original binary.

1. Use an isolated, snapshotted Windows x86 VM.
2. Install a licensed Visual C++ 6.0 toolchain and required SDKs locally.
3. Clone the private repository into the VM; do not expose its old server binaries directly to the Internet.
4. Open PowerShell inside the VM and run:

```powershell
.\tools\build-vc6-all.ps1 -DryRun -Configuration Release
.\tools\build-vc6-all.ps1 -DryRun -Configuration Debug
.\tools\build-vc6-all.ps1 -Configuration Release -Msdev "C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\MSDEV.EXE"
```

The script validates project configurations before invoking VC6, then attempts all eight target configurations in order: `LuaLib`, `Common`, `Core_Lib Client`, `Core_Lib Server`, `Engine`, `Represent2`, `Faith`, `lord`. It **continues after build failures** and captures per-target build logs plus a JSON summary under `build/vc6/`. Both the `Release` and `Debug` config names are validated in automated tests and Windows CI dry runs; this does not compile any VC6 code. Running the script in this environment has not been tested. It uses MSDEV's historical `/MAKE` interface and does not run built games.

## True offline blockers (independent of compiling C)

| Component | Current snapshot |
| --- | --- |
| `Game.dll` | Missing |
| `FSInterface.dll` | Missing |
| Database `fsonline2` schema/data | Missing |
| Maps, NPC, sprite/animation, scripts | Missing complete client data |
| Full game/server build | Not demonstrated |
| Windows VC6 toolchain | Must be supplied by the developer |

Do not create empty stub DLLs or invent a database schema and call that a restored game. The next engineering step is **real compiler logs from a Windows x86 VM**, resolving non-proprietary build problems first, then rebuilding or legitimately obtaining ABI-compatible runtime components.

## License and security

Source and assets contain third-party copyright notices including Kingsoft. The repository is private for investigation, not automatically licensed for redistribution. The miniLZO component carries its own GPL notice. Do not remove copyright headers, distribute game archives without authorization, or run unknown binaries outside a VM.
