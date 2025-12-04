#!/usr/bin/env -S uv run --script
#
# /// script
# requires-python = ">=3.14"
# dependencies = ["requests", "tabulate"]
# ///

"""Generate zmk-unicode headers for all Unicode blocks."""

import argparse
import re
import unicodedata as ud
from pathlib import Path

import requests
from tabulate import tabulate

REQUESTS_TIMEOUT = 5

PREFIX = "UC_"
UD_VERSION = ud.unidata_version
BLOCKS = f"https://unicode.org/Public/{UD_VERSION}/ucd/Blocks.txt"

PREAMBLE = f"""/*
 * Unicode keys for U+{{start}} .. U+{{end}}
 *
 * This file was generated from Unicode {UD_VERSION}.
 *
 * See http://www.unicode.org/versions/Unicode{UD_VERSION} for source data.
 */

#pragma once

"""


def sanitize(name):
    return re.sub(r"[^\w]+", "_", name)


class Char:
    def __init__(self, ch):
        self.ch = ch
        if len(ch) == 1:
            self.codepoint = ord(ch)
            self.name = self._name()
        else:
            self.codepoint, self.name = None, None

    def __bool__(self):
        return all([self.name, self._category_is_okay(), self._is_normalized()])

    def __eq__(self, other):
        if isinstance(other, Char):
            return self.ch == other.ch
        return False

    def hex(self):
        return f"0x{self.codepoint:X}" if self else "0"

    def _category_is_okay(self):
        # Exclude control codes and marks.
        return ud.category(self.ch)[0] not in {"C", "M"}

    def _is_normalized(self):
        # Exclude aliases unless name is distinct.
        nch = ud.normalize("NFKC", self.ch)
        if nch == self.ch or len(nch) > 1:
            return True
        sub = " LETTER| SYMBOL| SMALL| CAPITAL"
        return re.sub(sub, "", ud.name(nch)) != re.sub(sub, "", self.name)

    def _name(self):
        try:
            return ud.name(self.ch)
        except ValueError:
            return None


class Macro:
    def __init__(self, ch):
        lower = ch.lower()
        upper = ch.upper()
        if lower != upper and lower.upper() == upper and upper.lower() == lower:
            self.lower = Char(lower)
            self.upper = Char(upper)
        else:
            self.lower = Char(ch)
            self.upper = Char("\x00")

    def __bool__(self):
        return bool(self.lower)

    def dict(self):
        return {
            "name": self.name(),
            "lower": self.lower.hex(),
            "upper": self.upper.hex(),
            "glyph": self.glyph(),
        }

    def name(self):
        if self.upper:
            lower_ = self.lower.name.split(" ")
            if lower_[1] == "SMALL":
                del lower_[1]
            return sanitize(" ".join(lower_))
        return sanitize(self.lower.name)

    def glyph(self):
        if self.upper:
            return "/".join([self.lower.ch, self.upper.ch])
        return self.lower.ch


def fetch_blocks(retry=0):
    if retry==0:
        print('Fetching block data... ', end='')
    try:
        response = requests.get(BLOCKS, timeout=REQUESTS_TIMEOUT)
        if response.status_code == 200:
            print('done')
            return response.text
    except requests.exceptions.ConnectionError:
        pass
    if retry < 5:
        print('failed. Retrying... ', end='')
        return fetch_blocks(retry=retry + 1)
    print('failed')
    raise ValueError('Could not download blocks')


def parse_block(start, end):
    block_data = {}
    for cp in range(start, end + 1):
        macro = Macro(chr(cp))
        if macro:
            dict_ = macro.dict()
            block_data[dict_["name"]] = dict_

    return block_data


def write_block(block_data, start, end, name, outdir):
    macros = []
    for m in block_data.values():
        macros.append(
            [
                f"#define {PREFIX + m['name']}",
                m["lower"],
                m["upper"],
                f"// {m['glyph']}",
            ]
        )
    preamble = PREAMBLE.format(start=start, end=end)
    table = tabulate(macros, tablefmt="plain")
    file = outdir / (sanitize(name).lower() + ".h")
    file.write_text(preamble + table)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert zmk-helper to zmk-unicode macros"
    )
    parser.add_argument("-o", "--out", required=True)
    args = parser.parse_args()
    outdir = Path(args.out)

    blocks = fetch_blocks()
    pattern = re.compile(r"([0-9A-F]+)\.\.([0-9A-F]+);\ (\S.*\S)")
    print('Parsing blocks... ', end='')
    for block in blocks.splitlines():
        if m := pattern.match(block):
            start, end, name = m.groups()
            block_data = parse_block(int(start, 16), int(end, 16))
            write_block(block_data, start, end, name, outdir)
    print('done')
