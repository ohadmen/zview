#!/usr/bin/env python3
"""Convert a SPIR-V binary file to a C++ header with a uint32_t array."""
import sys
import struct

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input.spv> <output.h> <array_name>", file=sys.stderr)
        sys.exit(1)
    in_path, out_path, name = sys.argv[1], sys.argv[2], sys.argv[3]
    data = open(in_path, "rb").read()
    n = len(data) // 4
    words = struct.unpack(f"<{n}I", data[: n * 4])
    with open(out_path, "w") as f:
        f.write("#pragma once\n#include <cstdint>\n")
        f.write(f"static const uint32_t {name}[] = {{")
        f.write(",".join(f"0x{w:08x}" for w in words))
        f.write(f"}};\n")
        f.write(f"static const uint32_t {name}_len = {n};\n")

if __name__ == "__main__":
    main()
