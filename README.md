# ZMK-UNICODE

This module adds a `unicode` behavior to ZMK. Some highlights:

- Add any code point to the keymap using `&uc` without prior definition
- Convenience macros for even easier input of commonly used code points
- Configurable input systems that can be switched while keyboard is in use

## Usage

To load the module, add the following entries to `remotes` and `projects` in
`config/west.yml`.

```yml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: urob
      url-base: https://github.com/urob
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: v0.3 # Set to desired ZMK release.
      import: app/west.yml
    - name: zmk-unicode
      remote: urob
      revision: v0.3 # Should match ZMK release.
  self:
    path: config
```

## Configuration

For Unicode input to work one must prepare both the _keyboard_ **and** the
_operating system_. Please read through the entire section to understand what is
required.

### 1. Initialization

To initialize the Unicode module, include the `unicode.dtsi` header near the
top of the keymap:

```c
#include <behaviors/unicode.dtsi>
```

### 2. Adding Unicode code points to the keymap

**Manual input.** Unicode code points are added to the keymap using `&uc CP1 CP2`, where `CP1` and
`CP2` are hexadecimal code points. The former is produced if the key is pressed by itself, the
latter is produced if the key is pressed while <kbd>Shift</kbd> is active. To yield a code point
`CP` independently of <kbd>Shift</kbd>, one can use the shortcut `&uc CP 0` (or `&uc CP CP`).

For instance `&uc 0xE4 0xC4` yields `ä` (`U+00E4`) when pressed by itself and yields `Ä` (`U+00C4`)
when `Shift` is active. By contrast, `&uc 0xe4 0` always yields `ä`.

Code points must be in the range of `0x00` to `0x10FFFF`. Leading zeros can be omitted without loss;
i.e., `&uc 0xE4 0` is equivalent to `&uc 0x00E4 0`.

**Padding.** Regardless of whether leading zeros are omitted _on the keymap_, they are omitted, by
default, when the code point is send to the OS. This tends to be more reliable for input systems
that I have personally tested. Should this cause issues, one can force padding to a minimum length
using the `minimum-length` property. E.g., 

```c
&uc { 
  minimum-length = <4>;
};
```

Please let me know if certain input systems require strictly positive padding.

**Convenience macros.** This module includes predefined keys for all Unicode 16.0.0 glyphs. It also
includes a curated collection of language-specific aliases, which expose a subset of keys under
shorter labels.

For instance, instead of using `&uc 0xE4 0xC4` to get `ä/Ä` one can
equivalently use `&uc UC_DE_AE`. Language-specific aliases work without additional setup. A full
list of available aliases can be seen
[here](include/zmk-unicode/keys).

To access the full set of Unicode glyphs by label, one must source one of the following headers.
```c
#include <zmk-unicode/keys-extra.h>  // All glyphs except CJK
#include <zmk-unicode/keys-full.h>   // All glyphs including CJK
```
These glyphs are accessible by their official names, prefixed by `UC_`. The exception are keys which
come in lower/uppercase pairs, which are exposed omitting `SMALL` and `CAPITAL`. For instance:
```c
&uc UC_LATIN_LETTER_A_WITH_GRAVE  // à/À
&uc UC_WINKING_FACE               // 😉
```


### 3. Selecting an input system on the keyboard

There are six configurable input systems (see below for descriptions and further customization
options). Each input system is associated with a `Default-mode` label used to configure the initial
system when the keyboard starts up, and a `Set-mode` label used to switch inputs while the
keyboard is in use.


|  | Default-mode | Set-mode |
|---|---|---|
| macOS | `UC_MODE_MACOS` | `UC_SET_MACOS` |
| Linux | `UC_MODE_LINUX` | `UC_SET_LINUX` |
| Linux (alt) | `UC_MODE_LINUX_ALT` | `UC_SET_LINUX_ALT` |
| Windows (WinCompose) | `UC_MODE_WIN_COMPOSE` | `UC_SET_WIN_COMPOSE` |
| Windows (HexNumpad) | `UC_MODE_WIN_ALT` | `UC_SET_WIN_ALT` |
| Emacs  | `UC_MODE_EMACS` | `UC_SET_EMACS` |


**Initial system.** When the keyboard starts up, it initially selects the system defined by
the `default-mode` property of the `uc` behavior. To set it, add the following to your keymap
outside its root node.

```c
&uc {
  default-mode = <UC_MODE_LINUX>;  // Replace with desired input system.
};
```

**Switching while in use.** To switch the input system while the keyboard is in use, add `&uc
UC_SET_MODE` bindings to your keymap. For instance, `&uc UC_SET_LINUX` will switch the input system
to Linux.

### 4. Preparing the OS and further configuration

Most input systems require some preparation of the OS. Continue reading to learn about the
differences between these systems, additional configuration options, and how to prepare your OS.

<details><summary>1. macOS (<code>UC_MODE_MACOS</code>)</summary>

macOS has built-in support for Unicode input, supporting all possible code points.

To enable, go to **System Preferences → Keyboard → Input Sources**, then add
Unicode Hex Input to the list (under Other), and activate it from the input
dropdown in the menu bar. Note that this may disable some Option-based shortcuts
such as Option+Left and Option+Right.

The `UC_MODE_MACOS` input system has one configurable property `macos-key`,
which defaults to `LALT`. The system will:
  1. press and hold `macos-key` (`LALT` per default)
  2. input the code point sequence
  3. release `macos-key`

To overwrite `macos-key`, add the following outside of the root node of your
keymap:

```c
&uc {
  macos-key = <LALT>;  // replace with desired key
};
```

</details>

<details><summary>2. Linux IBus (<code>UC_MODE_LINUX</code>)</summary>

For desktop environments with IBus, Unicode input is enabled by default, supports all possible code
points, and works almost anywhere. Without IBus, it works under GTK apps, but rarely anywhere else.
(Though, according to [this stack exchange answer](https://unix.stackexchange.com/a/16135), it is
possible to install `IBus` under other DEs.)

If the system is not working in certain applications, it is worth trying out `UC_MODE_LINUX_ALT`.

The `UC_MODE_LINUX` input system has one configurable property `linux-key`,
which defaults to `LC(LS(U))`. The system will:
  1. tap and release `linux-key` (`LC(LS(U))` by default)
  2. input the code point sequence
  3. tap and release `SPACE`

To overwrite `linux-key`, add the following outside of the root node of your
keymap:

```c
&uc {
  linux-key = <LC(LS(U))>;  // replace with desired key
};
```

</details>

<details><summary>3. Linux Alt (<code>UC_MODE_LINUX_ALT</code>)</summary>

This is a variant of `UC_MODE_LINUX`, which keeps holding `LCTRL + LSHFT` for
the entire input.

The `UC_MODE_LINUX_ALT` input system has one configurable property `linux-alt-key`,
which defaults to `LC(LSHSFT)`. The system will:
  1. press and hold `linux-alt-key` (`LC(LSHFT)` by default)
  2. tap and release `U`
  3. input the code point sequence
  4. tap and release `SPACE`
  5. release `linux-alt-key`

To overwrite `linux-alt-key`, add the following outside of the root node of your
keymap:

```c
&uc {
  linux-alt-key = <LC(LSHFT)>;  // replace with desired key
};
```

</details>

<details><summary>4. WinCompose (<code>UC_MODE_WIN_COMPOSE</code>)</summary>

This input system requires a third-party tool called
[WinCompose](https://github.com/samhocevar/wincompose). 
It supports all possible code points, and is the **recommended
input mode for Windows**.

To enable, install the [latest release from
GitHub](https://github.com/samhocevar/wincompose/releases/latest). Once
installed, it will automatically run on startup. This works reliably under all
versions of Windows supported by WinCompose.

The `UC_MODE_WIN_COMPOSE` input system has one configurable property `win-compose-key`,
which defaults to `RALT`. The system will:
  1. tap and release `win-compose-key` (`RALT` by default)
  2. tap and release `U`
  3. input the code point sequence
  4. tap and release `RET`

To overwrite `win-compose-key`, add the following outside of the root node of your
keymap:

```c
&uc {
  win-compose-key = <RALT>;  // replace with desired key
};
```

</details>

<details><summary>5. Windows HexNumpad (<code>UC_MODE_WIN_ALT</code>)</summary>

This is Windows' built-in hex numpad Unicode input mode. It only supports code
points up to `U+FFFF`, and is not recommended due to reliability and
compatibility issues.

To enable, run the following as an administrator, then reboot:

```cmd
reg add "HKCU\Control Panel\Input Method" -v EnableHexNumpad -t REG_SZ -d 1
```

The system will:
  1. press and hold `LALT`
  2. tap and release `KP_PLUS`
  3. input the code point sequence
  4. release `LALT`

</details>

<details><summary>6. Emacs (<code>UC_MODE_EMACS</code>)</summary>

Emacs supports code point input with the `insert-char` command.

The system will:
  1. tap and release `LC(X)`
  2. tap and release `N8`
  3. tap and release `RET`
  4. input the code point sequence
  5. tap and release `RET`

</details>

### 5. Troubleshooting Kconfig settings

Two `Kconfig` settings can be used to fine-tune the timing of the input system.

- `ZMK_UNICODE_TAP_MS` sets the time to wait (in milliseconds) between the press and release of each
key in the Unicode sequence. (defaults to 5)
- `ZMK_UNICODE_WAIT_MS` sets the time to wait (in milliseconds) between tapping keys in the Unicode
sequence. (defaults to 0)

If you experience "mangled" sequences appearing on the screen, increasing the defaults may help.
This is known to help on slow terminal connections and weak Bluetooth signals.

## Known issues

- There is currently no error checking for code points. Please make sure that
you specify a valid code point string.
- Code points are send using the default US keyboard layout. Alternative layouts
  are currently not supported.

## Example keymap

```c
#include <behaviors.dtsi>
#include <dt-bindings/zmk/keys.h>
#include <behaviors/unicode.dtsi>  // Source header for this module

// Optional: Overwrite default behavior properties
&uc {
  default-mode = <UC_MODE_LINUX>;  // Default to Linux input system
  minimum-length = <0>;            // Set to desired minimum input length
  linux-key = <LC(LS(U))>;         // Overwrite Linux compose key
  win-compose-key = <RALT>;        // Overwrite WinCompose compose key
};

/ {
  keymap {
    compatible = "zmk,keymap";
    default_layer {
      bindings = <
        /* Add some code points */
        &uc UC_DE_AE               /* ä/Ä - same as &uc 0xE4 0xC4 */
        &uc UC_DE_OE               /* ö/Ö - same as &uc 0xFC 0xDC */
        &uc UC_DE_UE               /* ü/Ü - same as &uc 0xF6 0xD6 */
        &uc 0x1F596 0              /* 🖖  */
        /* Add bindings to switch between input modes */
        &uc UC_SET_MACOS
        &uc UC_SET_LINUX
      >;
    };
  };
};
```

## Contributing

Contributions in any form are very welcome!

New _convenience macros_ should be prefixed by `UC_<NS>` where `<NS>` is a unique namespace
reflecting the use. In particular, all language headers should use the corresponding [two-letter ISO
language code](https://en.wikipedia.org/wiki/List_of_ISO_639_language_codes) as namespace. Other
headers are encouraged to use at least three-letter namespace to avoid conflicts.
