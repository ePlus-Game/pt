# Recovery phase 2 — targeted source import

Phase 1 was merged in PR #8 (3,605 source files). This branch extends the source gate narrowly, then imports an additional **candidate** set from the original `PT.zip` via a Colab notebook (the archive itself stays in Google Drive).

## Candidates (archive manifest; must pass actual-content audit)

- 16 top-level `Build/` source/scripts
- 42 top-level `Deploy/` scripts and metadata (not `Deploy/Config` or `ProjectContent`)
- 41 `gameserver/` `.luax` files (only if no embedded NUL bytes / suspect credentials)
- 2 `Sources/` `.inc` files
- 1 `Docs/` `.tsv` file
- 1 `gameserver/settings/npc/player/newplayerbaseattribute.ini`

Total: **103 path-based candidates**, not a promise that all are safe or valid.

## Exclusions and verification

No runtime binaries, PAK/SPR/DAT, private keys, account databases, backup files, nested Git history, or full Client/Server script trees. Basic regex-based secret detection and NUL-byte filtering apply before push; manual review is still required, especially for public distribution rights. Build validation is *not* proof that the original game runs.

## Next stage

After reviewing the Phase 2 pull request, inventory and verify runtime dependencies in Google Drive; then attempt Windows-native build and offline startup in an isolated VM. Track missing files and errors instead of importing all 13GB into Git.
