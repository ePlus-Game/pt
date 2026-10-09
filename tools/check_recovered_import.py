#!/usr/bin/env python3
"""Read-only safety gate for source imported into recovered/PhongThanSource/.

Conservative static inspection only; not a license clearance or full secret scan.
Never executes any imported code or binaries.
"""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path, PurePosixPath
import re
import subprocess

PREFIX = 'recovered/PhongThanSource/'
ALLOWED_SUFFIXES = {
    '.cpp', '.c', '.h', '.hpp', '.inl', '.dsp', '.dsw', '.sln', '.vcproj',
    '.idl', '.odl', '.def', '.rc', '.lua', '.py', '.ps1', '.md', '.txt',
    '.json', '.yml', '.yaml', '.cmake', '.mak', '.bat', '.cmd', '.cs',
    '.xml', '.sh', '.ts', '.tsx', '.jsx', '.js',
}
BLOCKED_PARTS = {
    '.git', '.vs', 'debug', 'release', 'output', 'outputmodern',
    'thirdparty', 'lib', 'deploy', 'build', 'bin', 'obj', 'logs',
    'backup', '_backup', 'node_modules', 'win32release', 'win32debug',
    'win32serverrelease', 'win32serverdebug', 'win32clientrelease',
    'win32clientdebug',
}
MAX_BYTES = 3 * 1024 * 1024
SUSPICIOUS_NAME = re.compile(
    r'(?:^\.env(?:\.|$)|secret|credential|password|private[_-]?key|'
    r'id_rsa|\.pem$|\.pfx$|\.p12$|\.key$|token)', re.I
)
# Findings never reveal matched credential values in reports or logs.
SENSITIVE_CONTENT = (
    re.compile(rb'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----'),
    re.compile(rb'github_pat_[A-Za-z0-9_]{18,}'),
    re.compile(rb'gh[pousr]_[A-Za-z0-9_]{20,}'),
    re.compile(rb'AKIA[0-9A-Z]{16}'),
    re.compile(rb'sk-[A-Za-z0-9_-]{30,}'),
    re.compile(rb'(?i)(?:password|secret|api[_-]?key|access[_-]?token)\s*[:=]\s*[\'\"][^\'\"\r\n]{7,}[\'\"]'),
)


def check_file(root: Path, relative: str) -> list[str]:
    if not relative.startswith(PREFIX):
        return ['outside_recovered_source']
    parts = relative[len(PREFIX):].split('/')
    if (not parts or any(not p or p in ('.', '..') for p in parts)
            or '\\' in relative or '\x00' in relative
            or ':' in relative or relative.startswith('/')):
        return ['unsafe_path']
    if any(p.casefold() in BLOCKED_PARTS for p in parts[:-1]):
        return ['blocked_directory']
    name = parts[-1]
    if SUSPICIOUS_NAME.search(name):
        return ['suspicious_filename']
    if PurePosixPath(name).suffix.lower() not in ALLOWED_SUFFIXES:
        return ['unapproved_extension']
    target = root.joinpath(*relative.split('/'))
    if target.is_symlink() or not target.is_file():
        return ['symlink_or_missing_file']
    if target.stat().st_size > MAX_BYTES:
        return ['oversized_source_file']
    content = target.read_bytes()
    if b'\x00' in content:
        return ['nul_byte_binary_content']
    if any(pattern.search(content) for pattern in SENSITIVE_CONTENT):
        return ['potential_embedded_secret']
    return []


def evaluate(root: Path, files: list[str]) -> dict:
    issues = []
    accepted = 0
    size = 0
    types: Counter[str] = Counter()
    for file in files:
        reasons = check_file(root, file)
        if reasons:
            issues.append({'path': file, 'reasons': reasons})
        else:
            accepted += 1
            path = root.joinpath(*file.split('/'))
            size += path.stat().st_size
            types[path.suffix.lower()] += 1
    return {
        'ok': not issues,
        'checked': len(files),
        'accepted': accepted,
        'source_bytes': size,
        'extension_counts': dict(sorted(types.items())),
        'violations': issues,
        'warning': ('Static safety heuristics only: no license clearance, no complete '
                    'credential detection, no executable game compilation.'),
    }


def changed_files(root: Path, base: str) -> list[str]:
    # NUL-delimited paths are safe to parse even for unusual filenames.
    proc = subprocess.run(
        ['git', 'diff', '--name-only', '--diff-filter=ACMRT', '-z',
         f'{base}...HEAD'],
        cwd=root, check=True, capture_output=True,
    )
    return [p.decode('utf-8', 'surrogateescape')
            for p in proc.stdout.split(b'\x00') if p]


def markdown(report: dict) -> str:
    state = 'PASS' if report['ok'] else 'FAIL'
    lines = [f'# Recovered source import: {state}', '',
             f"- Changed files: {report['checked']}",
             f"- Allowed source files: {report['accepted']}",
             f"- Allowed bytes: {report['source_bytes']}",
             f"- Violations: {len(report['violations'])}", '',
             '> ' + report['warning'], '', '## Violations', '']
    if not report['violations']:
        lines.append('None detected by these checks.')
    else:
        lines.extend(
            f"- `{issue['path'].replace('`','')}`: " + ', '.join(issue['reasons'])
            for issue in report['violations']
        )
    lines.append('')
    return '\n'.join(lines)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    ap.add_argument('--base', default='origin/main')
    ap.add_argument('--json', type=Path)
    ap.add_argument('--markdown', type=Path)
    args = ap.parse_args()
    try:
        paths = changed_files(args.root, args.base)
    except subprocess.CalledProcessError as exc:
        print('Cannot inspect branch diff against base:', args.base, exc)
        return 2
    report = evaluate(args.root, paths)
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    md = markdown(report)
    if args.markdown:
        args.markdown.parent.mkdir(parents=True, exist_ok=True)
        args.markdown.write_text(md, encoding='utf-8')
    print(md)
    return 0 if report['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
