"""Regression tests for VC6 batch configuration selection, without MSDEV."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MATRIX = re.compile(
    r'@\{Name="([^"]+)"; Path="([^"]+)"; Config="([^"]+)"\}'
)


class Vc6BuildMatrixTest(unittest.TestCase):
    def test_all_declared_configs_exist_in_original_project(self):
        script = (ROOT / "tools/build-vc6-all.ps1").read_text(encoding="utf-8")
        targets = MATRIX.findall(script)
        self.assertEqual(len(targets), 8)
        self.assertEqual({name for name, _, _ in targets}, {
            "LuaLib", "Common", "CoreClient", "CoreServer",
            "Engine", "Represent2", "Faith", "lord",
        })
        for name, raw_path, pattern in targets:
            with self.subTest(project=name):
                project = ROOT / Path(raw_path.replace("\\", "/"))
                self.assertTrue(project.is_file(), raw_path)
                declared = re.findall(
                    r'^# Name "([^"]+)"\s*$',
                    project.read_bytes().decode("latin-1"), re.M
                )
                self.assertGreater(len(declared), 0)
                for mode in ("Debug", "Release"):
                    self.assertIn(pattern.format(mode), declared)

    def test_preflight_runs_without_msdev(self):
        script = (ROOT / "tools/build-vc6-all.ps1").read_text(encoding="utf-8")
        self.assertLess(script.index("if ($DryRun)"), script.index("Get-Command -Name $Msdev"))
        self.assertIn("-cnotcontains $cfg", script)
        self.assertIn("Remove-Item -LiteralPath $log", script)


if __name__ == "__main__":
    unittest.main()
