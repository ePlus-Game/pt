"""Keep the CMake compile-only Lua target aligned with the legacy VC6 project."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class LegacyLuaManifestTests(unittest.TestCase):
    def test_cmake_sources_match_legacy_dsp_except_cli(self):
        project = (ROOT / "Base/LuaLib/LuaLib.dsp").read_bytes().decode("latin-1")
        cmake = (ROOT / "Base/LuaLib/CMakeLists.txt").read_text(encoding="utf-8")
        old_sources = {
            item.replace("\\", "/").replace("./", "", 1).lower()
            for item in re.findall(r"^SOURCE\s*=\s*(.*\.c)\s*$", project, re.M)
        }
        old_sources.discard("src/lua.c")  # standalone Lua CLI, not a library source
        cmake_sources = {
            path.lower()
            for path in re.findall(r"^\s+(src/(?:baselib/)?\w+\.c)\s*$", cmake, re.M)
        }
        self.assertEqual(old_sources, cmake_sources)
        for path in cmake_sources:
            self.assertTrue((ROOT / "Base/LuaLib" / path).is_file(), path)


if __name__ == "__main__":
    unittest.main()
