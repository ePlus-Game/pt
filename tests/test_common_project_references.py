"""Regression checks for Base/Common project source/header references."""
import unittest
from pathlib import Path, PureWindowsPath

from tools.audit_legacy_projects import inspect


ROOT = Path(__file__).resolve().parents[1]


class CommonProjectReferencesTest(unittest.TestCase):
    def test_all_available_shared_headers_are_referenced_correctly(self):
        project = ROOT / "Base" / "Common" / "Common.dsp"
        report = inspect(project, ROOT)
        shared_dir = ROOT / "Share" / "Header" / "Common"
        existing = {p.name.casefold() for p in shared_dir.iterdir() if p.is_file()}
        missing = [PureWindowsPath(item).name for item in report["missing"]]
        # Fail when a real header exists but the project still points at a bad path.
        incorrect = [name for name in missing if name.casefold() in existing]
        self.assertEqual(incorrect, [])
        # Two headers remain genuinely absent from this source snapshot.
        self.assertTrue(set(missing).issubset({"Timer.h", "SkillInfomation.h"}))


if __name__ == "__main__":
    unittest.main()
