# ZMK-UNICODE

This module adds a `unicode` behavior to ZMK. Some highlights:

- Add any code point to the keymap using `&uc` without prior definition
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
    revision: v0.3
      import: app/west.yml
    - name: zmk-unicode
      remote: urob
      revision: v0.3 # set to same as ZMK version above
  self:
    path: config
```

## Configuration

For unicode input to work one must prepare both the _keyboard_ **and** the
_operating system_. Please read through the entire section to understand what is
required.

### 1. Initialization

To initialize the unicode subsystem, include the `unicode.dtsi` header near the
top of the keymap:

```c
#include <behaviors/unicode.dtsi>
```

### 2. Adding Unicode code points to the keymap

Unicode code points can be added to the keymap using `&uc CP1 CP2`, where `CP1` and `CP2` are
hexadecimal code points. The former is produced if the key is pressed by itself, the latter is
produced if the key is pressed while `Shift` is active. Adding `&uc CP 0` will produce `CP` in
either case.

For instance `&uc 0xe4 0xc4` produces `ä` (`U+00E4`) when pressed by itself and
produces `Ä` (`U+00C4`) when `Shift` is active. 

Code points must be in the range of `0x00` to `0x10ffff` and can omit any leading zeros. That is,
`&uc 0xe4 0` and `&uc 0x00e4` are exactly equivalent. Either binding will by default omit any
leading zeros when sending the code point to the OS. For all input systems that I have personally
tested, this is more reliable then padding to a fixed length with leading zeros. In case this causes problems, one can force padding to a minimum length using the `minimum-length` property. E.g.,
```c
&uc {
  minimum-length = <4>;  // Replace with desired minimum length.
};
```

Please let me know if certain input systems require setting this larger than zero.

This module includes a collection of convenience macros to simplify the inclusion of common code
points. For instance, instead of using `&uc 0xe4 0xc4` to get `ä/Ä` one can equivalently use `&uc
UC_DE_AE`. All currently available macros can be seen [here](include/zmk-unicode/keys). 

### 3. Selecting an input system on the keyboard

There are six configurable input systems (see below for descriptions and further customization
options).

The _initial_ input system (selected when the keyboard starts up) is defined by adding the
following outside the root node acto the keymap. For instance:
```c
&uc {
  default-mode = <UC_MODE_LINUX>;  // Replace with desired default mode.
};
```

To _switch_ the input system while the keyboard is in use, add `&uc
MODE` bindings to your keymap. For instance, the following two bindings
can be used to toggle the input mode between Windows and Linux
```c
/ {
  keymap {
    compatible = "zmk,keymap";
    default_layer {
      bindings = <
        &uc UC_SET_WIN_COMPOSE &uc UC_SET_LINUX
      >;
    };
  };
};
```

### 4. Preparing the OS and further configuration

For Unicode input to work, one must (i) select the right input system on the keyboard, and (ii)
(in most cases) must prepare the OS.

There are six configurable input systems. Use the `Identifier` label when configuring the
`default-mode` option, and use the `Selection-Macro` label as argument to `&uc` for switching
the input system while the keyboard is in use.

|  | Identifier | Selection-Macro |
|---|---|---|
| macOS | `UC_MODE_MACOS` | `UC_SET_MACOS` |
| Linux | `UC_MODE_LINUX` | `UC_SET_LINUX` |
| Linux (alt) | `UC_MODE_LINUX_ALT` | `UC_SET_LINUX_ALT` |
| Windows (WinCompose) | `UC_MODE_WIN_COMPOSE` | `UC_SET_WIN_COMPOSE` |
| Windows (HexNumpad) | `UC_MODE_WIN_ALT` | `UC_SET_WIN_ALT` |
| Emacs  | `UC_MODE_EMACS` | `UC_SET_EMACS` |


Continue reading to learn about the differences between these systems, additional configuration
options, and how to prepare your OS.

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
  minimum-length = <4>;            // Pad input to at least 4 digits
  linux-key = <LC(LS(U))>;         // Overwrite Linux compose key
  win-compose-key = <RALT>;        // Overwrite WinCompose compose key
};

/ {
  keymap {
    compatible = "zmk,keymap";
    default_layer {
      bindings = <
        /* Add some code points */
        &uc 0xe4 0xc4       /* ä/Ä */
        &uc 0xf6 0xd6       /* ü/Ü */
        &uc 0xfc 0xdc       /* ö/Ö */
        &uc 0x1f596 0x2764  /* 🖖/❤ */
        /* Add bindings to switch between input modes */
        &uc UC_SET_WIN_COMPOSE  &uc UC_SET_LINUX  &uc UC_SET_LINUX_ALT  &uc UC_SET_MACOS
      >;
    };
  };
};

```
