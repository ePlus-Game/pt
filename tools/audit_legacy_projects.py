#!/usr/bin/env python3
"""Statically audit Visual C++ 6.0 project file references. Never run game code."""
import argparse
import json
import re
from pathlib import Path

PROJECT_RE = re.compile(r'^# Microsoft Developer Studio Project File - Name="([^"]+)"', re.M)
TARGET_RE = re.compile(r'^# TARGTYPE "([^"]+)"', re.M)
SOURCE_RE = re.compile(r'^SOURCE\s*=\s*(.+)$', re.M)
ABSOLUTE_RE = re.compile(r'^(?:[A-Za-z]:[\\/]|[\\/]{2})')


def inspect(project: Path, root: Path) -> dict:
    contents = project.read_bytes().decode("latin-1")
    project_name = PROJECT_RE.search(contents)
    target = TARGET_RE.search(contents)
    result = {
        "project": project.relative_to(root).as_posix(),
        "name": project_name.group(1) if project_name else project.stem,
        "target": target.group(1) if target else "unknown",
        "references": 0,
        "present": 0,
        "missing": [],
        "external_or_dynamic": [],
    }
    for entry in SOURCE_RE.findall(contents):
        raw = entry.strip().strip('"').replace("\\", "/")
        result["references"] += 1
        if not raw or "$(" in raw or "%" in raw or ABSOLUTE_RE.match(raw):
            result["external_or_dynamic"].append(entry.strip())
            continue
        candidate = project.parent
        missing = False
        for segment in raw.split("/"):
            if segment in ("", "."):
                continue
            if segment == "..":
                candidate = candidate.parent
                continue
            if not candidate.is_dir():
                missing = True
                break
            match = next((p for p in candidate.iterdir() if p.name.casefold() == segment.casefold()), None)
            if match is None:
                missing = True
                break
            candidate = match
        if missing or not candidate.is_file():
            result["missing"].append(entry.strip())
        else:
            result["present"] += 1
    return result


def audit(root: Path) -> dict:
    root = root.resolve()
    projects = [inspect(p, root) for p in sorted(root.rglob("*.dsp"))]
    return {
        "project_count": len(projects),
        "source_references": sum(p["references"] for p in projects),
        "present_references": sum(p["present"] for p in projects),
        "missing_references": sum(len(p["missing"]) for p in projects),
        "dynamic_references": sum(len(p["external_or_dynamic"]) for p in projects),
        "projects": projects,
    }


def to_markdown(report: dict) -> str:
    lines = [
        "# Legacy VC6 project reference audit", "",
        "> Static path inspection only; this does not establish that the project builds.", "",
        "Projects: " + str(report["project_count"]),
        "References: " + str(report["source_references"]),
        "Present: " + str(report["present_references"]),
        "Missing: " + str(report["missing_references"]),
        "External/dynamic: " + str(report["dynamic_references"]), "",
        "| Project | Target | Present | Missing | External |",
        "| --- | --- | ---: | ---: | ---: |",
    ]
    for project in report["projects"]:
        lines.append("| {} | {} | {} | {} | {} |".format(
            project["project"], project["target"], project["present"],
            len(project["missing"]), len(project["external_or_dynamic"])))
    lines.extend(["", "## Missing references", ""])
    for project in report["projects"]:
        if project["missing"]:
            lines.append("### " + project["project"])
            lines.extend("- " + item for item in project["missing"])
            lines.append("")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--json", type=Path)
    parser.add_argument("--markdown", type=Path)
    parser.add_argument("--fail-on-missing", action="store_true")
    args = parser.parse_args()
    report = audit(args.root)
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    if args.markdown:
        args.markdown.parent.mkdir(parents=True, exist_ok=True)
        args.markdown.write_text(to_markdown(report), encoding="utf-8")
    print("Projects: {}; references: {}; present: {}; missing: {}; external/dynamic: {}".format(
        report["project_count"], report["source_references"], report["present_references"],
        report["missing_references"], report["dynamic_references"]))
    return int(args.fail_on_missing and report["missing_references"] > 0)


if __name__ == "__main__":
    raise SystemExit(main())
