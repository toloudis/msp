#!/usr/bin/env python3
"""
compile_check.py - compile every entry point listed in .effect.json manifests.

Checks that a converted .hlsl builds as plain shaders, one entry point and
profile at a time, with no Effects runtime involved.

  python compile_check.py --compiler fxc  FILE.effect.json [...]
  python compile_check.py --compiler dxc  FILE.effect.json [...]   # SM 6.0
  python compile_check.py --compiler dxc --spirv FILE.effect.json  # Vulkan

fxc compiles the profile written in the manifest (e.g. ps_5_0). dxc does not
accept SM5 profiles, so the same stage is compiled as *_6_0.
"""

import argparse
import json
import os
import subprocess
import sys


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("manifests", nargs="+")
    ap.add_argument("--compiler", choices=("fxc", "dxc"), default="fxc")
    ap.add_argument("--exe", help="path to the compiler (default: on PATH)")
    ap.add_argument("--spirv", action="store_true", help="dxc only: emit SPIR-V")
    ap.add_argument("-v", "--verbose", action="store_true", help="show warnings")
    args = ap.parse_args()

    exe = args.exe or args.compiler
    null = "NUL" if os.name == "nt" else "/dev/null"
    total = failed = 0
    for path in args.manifests:
        with open(path, encoding="utf-8") as f:
            manifest = json.load(f)
        hlsl = os.path.join(os.path.dirname(path), manifest["hlsl"])
        seen = set()
        for tech in manifest["techniques"]:
            for p in tech["passes"]:
                for stage in p["stages"].values():
                    if not stage:
                        continue
                    key = (stage["entry"], stage["profile"])
                    if key in seen:
                        continue
                    seen.add(key)
                    profile = stage["profile"]
                    if args.compiler == "fxc":
                        cmd = [exe, "/nologo", "/T", profile, "/E", stage["entry"], "/Fo", null, hlsl]
                    else:
                        profile = profile.split("_")[0] + "_6_0"
                        cmd = [exe, "-T", profile, "-E", stage["entry"], "-Fo", null, hlsl]
                        if args.spirv:
                            cmd.insert(1, "-spirv")
                    total += 1
                    r = subprocess.run(cmd, capture_output=True, text=True)
                    ok = r.returncode == 0
                    if not ok:
                        failed += 1
                    if not ok or args.verbose:
                        print("%s %s %s %s" % ("FAIL" if not ok else "ok  ", manifest["hlsl"],
                                                profile, stage["entry"]))
                        msg = "\n".join(l for l in (r.stderr + r.stdout).strip().splitlines()
                                         if "compilation object save succeeded" not in l)
                        if msg:
                            print("    " + msg.replace("\n", "\n    "))
    print("%d of %d entry point(s) compiled" % (total - failed, total))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
