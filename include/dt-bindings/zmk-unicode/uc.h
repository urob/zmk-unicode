#pragma once

//                               INPUT_START              | INPUT_STOP
#define UC_MODE_MACOS 0       // press LALT               | release LALT
#define UC_MODE_LINUX 1       // tap LC(LS(U))            | tap SPACE
#define UC_MODE_LINUX_ALT 2   // press LC(LSHFT) tap U    | tap SPACE release LC(LSHFT)
#define UC_MODE_WIN_COMPOSE 4 // tap RALT U               | tap RET
#define UC_MODE_WIN_ALT 3     // press LALT tap KP_PLUS   | release LALT
#define UC_MODE_EMACS 5       // tap LC(X) N8 RET         | tap RET

#define UC_MAC UC_MODE_MACOS
#define UC_LIN UC_MODE_LINUX
#define UC_LINALT UC_MODE_LINUX_ALT
#define UC_WINC UC_MODE_WIN_COMPOSE
#define UC_WINALT UC_MODE_WIN_ALT
#define UC_EMAC UC_MODE_EMACS
