"""Focused adapter tests using real Make expansion and real CMake cache updates.

Run: python3 tools/test-build-parity.py
No Trick dependencies or configured checkout are required.
"""

import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ADAPTER = ROOT / "tools/build-parity.sh"


@unittest.skipUnless(
    shutil.which("make") and shutil.which("cmake"), "requires make and cmake"
)
class ConfigureCMakeTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="parity-test-")
        self.addCleanup(self.temp.cleanup)
        self.source = Path(self.temp.name) / "source"
        self.source.mkdir()
        self.makefiles = self.source / "share/trick/makefiles"
        self.makefiles.mkdir(parents=True)
        # Exercise the actual print-% interface, without unrelated Trick flags.
        shutil.copyfile(
            ROOT / "share/trick/makefiles/Makefile.trickconfig",
            self.makefiles / "Makefile.trickconfig",
        )
        (self.makefiles / "Makefile.common").write_text(
            "include ${TRICK_HOME}/share/trick/makefiles/config_user.mk\n"
        )
        self.config = self.makefiles / "config_user.mk"
        self.config.write_text("HDF5 =\nGSL_HOME =\n")
        (self.source / "CMakeLists.txt").write_text(
            "cmake_minimum_required(VERSION 3.25)\nproject(AdapterTest NONE)\n"
        )
        self.build = self.source / "build with spaces"
        self.env = dict(os.environ, TRICK_HOME="/incorrect/inherited/home")
        self.env.pop("MAKEFLAGS", None)
        self.env.pop("MFLAGS", None)

    def run_adapter(self, *args):
        result = subprocess.run(
            [
                shutil.which("bash"),
                str(ADAPTER),
                "configure-cmake",
                "-S",
                str(self.source),
                "-B",
                str(self.build),
                *args,
            ],
            cwd=self.source,
            env=self.env,
            text=True,
            capture_output=True,
            check=False,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def cache(self):
        return (self.build / "CMakeCache.txt").read_text()

    def test_enabled_expansion_overrides_spaces_and_disabled_rerun(self):
        self.config.write_text(
            "PREFIX = /dependencies with spaces\n"
            "HDF5 = $(PREFIX)/hdf5\n"
            "GSL_HOME = ignored\n"
            "override GSL_HOME = $(PREFIX)/gsl\n"
        )
        self.env["GSL_HOME"] = "/wrong/environment"
        self.run_adapter(
            "-DTRICK_ENABLE_HDF5:BOOL=OFF",
            "-DCALLER_VALUE:STRING=one argument with spaces",
        )
        cache = self.cache()
        self.assertIn("TRICK_ENABLE_HDF5:BOOL=ON", cache)
        self.assertIn("TRICK_ENABLE_GSL:BOOL=ON", cache)
        self.assertIn("HDF5_ROOT:PATH=/dependencies with spaces/hdf5", cache)
        self.assertIn("GSL_ROOT:PATH=/dependencies with spaces/gsl", cache)
        self.assertIn("CALLER_VALUE:STRING=one argument with spaces", cache)
        self.config.write_text("HDF5 =\noverride GSL_HOME =\n")
        self.run_adapter()
        cache = self.cache()
        self.assertIn("TRICK_ENABLE_HDF5:BOOL=OFF", cache)
        self.assertIn("TRICK_ENABLE_GSL:BOOL=OFF", cache)
        self.assertNotIn("HDF5_ROOT:", cache)
        self.assertNotIn("GSL_ROOT:", cache)
        self.assertNotIn("CALLER_VALUE:", cache)

    def test_changed_prefix_rediscovers_libraries(self):
        # Exercise FindGSL's real discovery cache, not only forwarded ROOT hints.
        # These fixtures need only headers and library filenames; nothing links them.
        prefixes = [Path(self.temp.name) / name for name in ("prefix A", "prefix B")]
        for prefix in prefixes:
            (prefix / "include/gsl").mkdir(parents=True)
            (prefix / "lib").mkdir()
            (prefix / "include/gsl/gsl_sf.h").write_text("")
            (prefix / "include/gsl/gsl_version.h").write_text(
                '#define GSL_VERSION "2.7"\n'
            )
            for library in ("libgsl.a", "libgslcblas.a"):
                (prefix / "lib" / library).write_text("")
        (self.source / "CMakeLists.txt").write_text(
            "cmake_minimum_required(VERSION 3.25)\n"
            "project(AdapterTest NONE)\n"
            "if(TRICK_ENABLE_GSL)\n"
            "  find_package(GSL REQUIRED)\n"
            "endif()\n"
        )
        self.env.pop("GSL_ROOT_DIR", None)
        for prefix in (prefixes[0], prefixes[1], None, prefixes[0]):
            self.config.write_text("HDF5 =\nGSL_HOME = {}\n".format(prefix or ""))
            self.run_adapter()
            cache = self.cache()
            if prefix is None:
                self.assertNotIn("GSL_LIBRARY:", cache)
                self.assertNotIn("GSL_INCLUDE_DIR:", cache)
            else:
                self.assertIn(f"GSL_INCLUDE_DIR:PATH={prefix}/include", cache)
                self.assertIn(f"GSL_LIBRARY:FILEPATH={prefix}/lib/libgsl.a", cache)
                self.assertIn(
                    f"GSL_CBLAS_LIBRARY:FILEPATH={prefix}/lib/libgslcblas.a", cache
                )


if __name__ == "__main__":
    unittest.main()
