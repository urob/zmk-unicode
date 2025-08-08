#pragma once

#define UC_NONE 0x00
#define UC_SELECT_INPUT_MODE 0xffffff

// Keep MACOS first and EMACS last.
//                                  INPUT_START              | INPUT_STOP
#define UC_MODE_MACOS 0x00       // press LALT               | release LALT
#define UC_MODE_LINUX 0x01       // tap LC(LS(U))            | tap SPACE
#define UC_MODE_LINUX_ALT 0x02   // press LC(LSHFT) tap U    | tap SPACE release LC(LSHFT)
#define UC_MODE_WIN_COMPOSE 0x03 // tap RALT U               | tap RET
#define UC_MODE_WIN_ALT 0x04     // press LALT tap KP_PLUS   | release LALT
#define UC_MODE_EMACS 0x05       // tap LC(X) N8 RET         | tap RET

#define UC_SET_MACOS UC_SELECT_INPUT_MODE UC_MODE_MACOS
#define UC_SET_LINUX UC_SELECT_INPUT_MODE UC_MODE_LINUX
#define UC_SET_LINUX_ALT UC_SELECT_INPUT_MODE UC_MODE_LINUX_ALT
#define UC_SET_WIN_COMPOSE UC_SELECT_INPUT_MODE UC_MODE_WIN_COMPOSE
#define UC_SET_WIN_ALT UC_SELECT_INPUT_MODE UC_MODE_WIN_ALT
#define UC_SET_EMACS UC_SELECT_INPUT_MODE UC_MODE_EMACS
