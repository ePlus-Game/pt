# PT.zip full source import — scope and workflow

The original `PT.zip` is retained in Google Drive and is **not** committed to Git.

## Canonical code layout

- `recovered/PhongThanSource/`: original text/source files from **all** `PhongThanSource` areas, including `Sources`, `Build`, `Deploy`, `Output`, and `ThirdParty`.
- `recovered/Runtime/Client/script/`, `settings/`, `Ui/`: Client scripts and text configuration.
- `recovered/Runtime/Server/script/`, `settings/`, `Ui/`: Server scripts and text configuration.

## Manifest-based estimate (prior to source-content screening)

- Source text candidates: **5,460**.
- Client/Server runtime script, settings and UI text candidates: **11,305**.
- Total: **16,765 candidates**, about **148 MiB** uncompressed.

These are manifest-based *candidates*, not a claim that all pass secret/NUL-byte checks or are legal to redistribute. Legacy encodings are preserved byte-for-byte.

## Excluded, with a per-file report in Google Drive

- Nested `.git` history, logs, backup data, generated object/build binaries, databases.
- Executables/libraries, PAK, SPR, DAT, maps, and non-code game assets.
- Files above 5 MiB, byte-NUL files, dangerous ZIP paths, potential credentials.

The Colab importer records excluded files and reasons in `MyDrive/PT_Git_Staging/PT_Full_Source_Excluded.csv`, plus totals in `pt_full_source_report.json`.

## Validation after import

1. Run the source import checker (CI) and review all exclusions, especially missing compile-time dependencies and settings.
2. Perform manual code provenance, redistribution-rights and secret review before merging into the **public** repository.
3. Use an isolated Windows VM to build the original Client and Server; document actual errors and binary dependencies.
4. Keep runtime data/third-party binaries separately; inventory hashes and versions. A source-only CI success is **not** an operational game build.

The importer pushes to `import/pt-full-source` in chunks and creates one **Draft** pull request. The branch currently only establishes import policy; the actual content will arrive when Colab runs.
