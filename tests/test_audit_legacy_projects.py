import tempfile
import unittest
from pathlib import Path
from tools.audit_legacy_projects import audit, to_markdown


class AuditTest(unittest.TestCase):
    def test_windows_paths_case_insensitive_and_unresolved(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "Assets").mkdir()
            (root / "Src").mkdir()
            (root / "Assets" / "Picture.BMP").write_bytes(b"fake")
            (root / "Src" / "client.dsp").write_text(
                '# Microsoft Developer Studio Project File - Name="client"\n'
                '# TARGTYPE "Win32 (x86) Application"\n'
                'SOURCE=..\\assets\\picture.bmp\n'
                'SOURCE=..\\assets\\missing.bmp\n'
                'SOURCE=$(SDKROOT)\\include\\generated.h\n'
            )
            report = audit(root)
            self.assertEqual(report["project_count"], 1)
            self.assertEqual(report["present_references"], 1)
            self.assertEqual(report["missing_references"], 1)
            self.assertEqual(report["dynamic_references"], 1)
            self.assertIn("client.dsp", to_markdown(report))


if __name__ == "__main__":
    unittest.main()
