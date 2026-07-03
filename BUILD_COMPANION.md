# Build CTExt with companion live export

The companion export patch lives under `ctext/`. It writes
`Documents/My Games/Chrono Trigger/companion_state.json` while the game runs.

## Recommended: GitHub Actions (no Windows PC)

From the mod repo root:

```bash
gh auth login
gh repo create chrono-trigger-companion --private --source=. --push   # first time only
./scripts/build-ctext-github.sh
./scripts/install-ctext-companion.sh
```

This uses a free `windows-latest` runner to compile Release Win32 `ctext.dll` and
downloads it to `ctext-build/release/`.

VM feasibility on your Linux box: see `docs/VM_FEASIBILITY.md` (RAM too tight right now).

## Manual build (Windows + VS2022)

- Visual Studio 2022 (Desktop development with C++)
- Windows SDK
- Git submodules: `git submodule update --init --recursive`

1. Open `ctext.sln` in Visual Studio 2022.
2. Configuration: **Release**, platform: **Win32** (x86).
3. Build **CTExt**.
4. Output: `build/Release/ctext.dll` (platform **x86** / Win32).

## Install on Linux (Proton)

Copy the built DLL into the project, then from the mod repo:

```bash
mkdir -p ctext-build/release
cp /path/from/windows/ctext.dll ctext-build/release/
./scripts/install-ctext-companion.sh
./scripts/launch-companion.sh
```

Launch Chrono Trigger from Steam. The companion **Game link** panel should show
**map scene** ID (unique per map). `fieldId` is also exported for debug but
**repeats across different places on PC** — do not use it alone for location.

Export JSON (v3) includes:

| Field | Meaning |
|-------|---------|
| `mapSceneId` | Parsed from `mapinfo_N.dat` on map load (CTViewer scene index) — **use for location mapping** |
| `fieldId` | `ChronoCanvas::currentFieldId` — **not unique** on PC |
| `sceneId` | Scene mode (16 = world map, 17 = field mode) |
| `posX`, `posY` | Player tile position (from `FieldImpl`, polled each export) |
| `locationProbe` | Debug: dwordBA0/BBC/BB8/BC0 if map id wrong |

## Config (`ctext.json`)

```json
"companion": {
  "enabled": true,
  "poll_interval_ms": 1000,
  "export_path": "",
  "storyline_rva": 0
}
```

- `export_path`: leave empty for default Documents path (works under Proton).
- `storyline_rva`: optional RVA for storyline index (0 = omit until RE'd).

## Verify export file

Proton save folder:

`~/.local/share/Steam/steamapps/compatdata/613830/pfx/drive_c/users/steamuser/My Documents/My Games/Chrono Trigger/companion_state.json`

Without the game, test the bridge path:

```bash
python3 scripts/mock-companion-state.py --map-scene-id 42 --field-id 3 --scene-id 17
```
