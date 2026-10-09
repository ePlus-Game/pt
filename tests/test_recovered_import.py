"""Regression tests for the GitHub Actions recovered-source import gate."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'tools' / 'check_recovered_import.py'
spec = importlib.util.spec_from_file_location('check_recovered_import', SCRIPT)
checker = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checker)


class RecoveredImportTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.prefix = checker.PREFIX

    def tearDown(self):
        self.temp.cleanup()

    def put(self, rel, data=b'// harmless source\n'):
        path = self.root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        return rel

    def test_safe_old_gbk_source_accepted(self):
        rel = self.put(self.prefix + 'Core/game.cpp', b'// \xc4\xe3\xba\xc3\nint main() {return 0;}\n')
        self.assertEqual(checker.check_file(self.root, rel), [])

    def test_non_source_change_blocks_import(self):
        self.assertEqual(checker.check_file(self.root, '.github/workflows/payload.yml'),
                         ['outside_recovered_source'])

    def test_rejects_unsafe_paths_binary_and_backups(self):
        for p in ('Core/../password.cpp', 'build/a.cpp', 'Debug/a.cpp',
                  '.git/config.cpp', 'data.pak', 'secrets.txt'):
            with self.subTest(path=p):
                rel = self.put(self.prefix + p)
                self.assertTrue(checker.check_file(self.root, rel))

    def test_rejects_nul_and_private_tokens_without_exposing_value(self):
        for data in (b'\x00\x01', b'github_pat_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'):
            rel = self.put(self.prefix + 'safe/main.cpp', data)
            report = checker.evaluate(self.root, [rel])
            self.assertFalse(report['ok'])
            self.assertNotIn('github_pat_', checker.markdown(report))

    def test_rejects_oversized_files_and_symlink(self):
        rel = self.put(self.prefix + 'big.cpp', b'x' * (checker.MAX_BYTES + 1))
        self.assertIn('oversized_source_file', checker.check_file(self.root, rel))
        target = self.root / (self.prefix + 'link.cpp')
        target.parent.mkdir(parents=True, exist_ok=True)
        target.symlink_to(self.root / rel)
        self.assertIn('symlink_or_missing_file', checker.check_file(self.root, self.prefix + 'link.cpp'))

    def test_good_summary(self):
        r1 = self.put(self.prefix + 'Game/test.cpp')
        r2 = self.put(self.prefix + 'Lua/scripts/init.lua')
        result = checker.evaluate(self.root, [r1, r2])
        self.assertTrue(result['ok'])
        self.assertEqual(result['accepted'], 2)
        self.assertEqual(result['extension_counts'], {'.cpp': 1, '.lua': 1})


if __name__ == '__main__':
    unittest.main()
