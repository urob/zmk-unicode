#!/usr/bin/env python
"""Convert ZMK-Helper headers to ZMK-Unicode headers."""

import argparse
import re
from pathlib import Path

from tabulate import tabulate

UNICODE_HELPER = "ZMK_UNICODE_<type>(<args>)//<comment>"
PREAMBLE = """/*
 * {header} unicode characters.
 *
 * This file was automatically converted from '{file}',
 * included in the zmk-helpers module.
 */
"""


def create_macro(spec, prefix=""):
    args = spec["args"].split(",")
    macro = [f"#define {prefix + args.pop(0).upper()}"]
    macro.append(convert_codepoint(args[:4]))
    macro.append(convert_codepoint(args[4:]))
    macro.append("// " + m.group("comment"))
    return macro


def convert_codepoint(codepoint):
    if not codepoint:
        return "0"

    for i, c in enumerate(codepoint):
        codepoint[i] = re.sub("N([0-9])", r"\1", c)
    converted = "".join(codepoint).lower()
    return "0x" + converted.lstrip("0")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert zmk-helper to zmk-unicode macros"
    )

    parser.add_argument("file", help="path to zmk-helper script")
    parser.add_argument(
        "-p", "--prefix", default="", help="prefix for generated macros"
    )
    args = parser.parse_args()

    p = Path(args.file)
    if not p.is_file():
        raise FileNotFoundError(f"No such file: {p}")

    input = p.read_text().replace(" ", "")
    output = []

    pattern = re.escape(UNICODE_HELPER)
    pattern = re.sub("<[^>]+>", r"(?P\g<0>.*)", pattern)
    for m in re.finditer(pattern, input):
        output.append(create_macro(m.groupdict(), prefix=args.prefix))

    preamble = PREAMBLE.format(header=p.stem.capitalize(), file=p.name)
    table = tabulate(output, tablefmt="plain")

    print(preamble)
    print(table)
