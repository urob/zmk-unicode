# ZMK-UNICODE

This module adds a `unicode` behavior to ZMK. Some highlights:

- Configurable input system with several preconfigured drivers
- The input system can be switched at any time
- Optional shifted codepoints for unicode behavior instances

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

### 2. Defining unicode behaviors

A `unicode` behavior instance is defined by a `codepoint` property and,
optionally, a `shifted-codepoint` property. If the latter is specified,
it is used in lieu of the former when `Shift` is active.

Both `codepoint` and `shifted-codepoint` should be entered as  4 (or 5) digit
_uppercase_ strings without a leading `U+`.

For instance, the following sets up a behavior that produces `ä` if pressed by itself and produces
`Ä` if pressed while `Shift` is active.

```dts
/ {
  behaviors {
    uc_ae: uc_ae {
      compatible = "zmk,behavior-unicode";
      #binding-cells = <0>;
      codepoint = "00E4";
      shifted-codepoint = "00C4";
    };
  };
};
```

### 3. Selecting an input system on the keyboard

There are six configurable input systems (see below for descriptions and further customization
options). The _initial_ input system (selected when the keyboard starts up) is defined by adding the
following outside the root node to the keymap. For instance:
```c
&uc_input {
  default-mode = <UC_MODE_LINUX>;  // Set the default mode here.
};
```

To _switch_ the input system while the keyboard is in use, add `&uc_input
MODE` bindings to your keymap. For instance, the following two bindings
can be used to toggle the input mode between Windows and Linux
```c
/ {
  keymap {
    compatible = "zmk,keymap";
    default_layer {
      bindings = <
        &uc_input UC_WINC &uc_input UC_LIN
      >;
    };
  };
};
```

### 4. Preparing the OS and further configuration

For Unicode input to work, one must (i) select the right input system on the keyboard, and (ii)
(in most cases) must prepare the OS.

Currently, there are six available input systems are available (most with additional configuration
options). Continue reading to decide which one is right and how to prepare your OS. Input
systems can be referred to either by their full name or by their alias shortcut.

<details><summary>1. macOS (<code>UC_MODE_MACOS</code> or <code>UC_MAC</code>)</summary>

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
&uc_input {
  macos-key = <LALT>;  // replace with desired key
};
```

</details>

<details><summary>2. Linux IBus (<code>UC_MODE_LINUX</code> or <code>UC_LIN</code>)</summary>

For Linux distros with IBus, Unicode input is enabled by default, supports all
possible code points, and works almost anywhere. Without IBus, it works under
GTK apps, but rarely anywhere else.

If the system is not working, it is worth trying out `UC_MODE_LINUX_ALT`.

The `UC_MODE_LINUX` input system has one configurable property `linux-key`,
which defaults to `LC(LS(U))`. The system will:
  1. tap and release `linux-key` (`LC(LS(U))` by default)
  2. input the code point sequence
  3. tap and release `SPACE`

To overwrite `linux-key`, add the following outside of the root node of your
keymap:

```c
&uc_input {
  linux-key = <LC(LS(U))>;  // replace with desired key
};
```

</details>

<details><summary>3. Linux Alt (<code>UC_MODE_LINUX_ALT</code> or <code>UC_LINALT</code>)</summary>

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
&uc_input {
  linux-alt-key = <LC(LSHFT)>;  // replace with desired key
};
```

</details>

<details><summary>4. WinCompose (<code>UC_MODE_WIN_COMPOSE</code> or <code>UC_WINC</code>)</summary>

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
&uc_input {
  win-compose-key = <RALT>;  // replace with desired key
};
```

</details>

<details><summary>5. Windows HexNumpad (<code>UC_MODE_WIN_ALT</code> or <code>UC_WINALT</code>)</summary>

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

<details><summary>6. Emacs (<code>UC_MODE_EMACS</code> or <code>UC_EMAC</code>)</summary>

Emacs supports code point input with the `insert-char` command.

The system will:
  1. tap and release `LC(X)`
  2. tap and release `N8`
  3. tap and release `RET`
  4. input the code point sequence
  5. tap and release `LALT`

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
