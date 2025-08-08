#pragma once

#include <zmk/behavior.h>
#include <zmk/keys.h>

#include <dt-bindings/zmk-unicode/uc.h>

#define ALL_MODS                                                                                   \
    (MOD_LSFT | MOD_RSFT | MOD_LCTL | MOD_RCTL | MOD_LALT | MOD_RALT | MOD_LGUI | MOD_RGUI)

typedef uint32_t uc_cp_t;

zmk_key_t hex_to_key[] = {N0, N1, N2, N3, N4, N5, N6, N7, N8, N9, A, B, C, D, E, F};

struct behavior_unicode_config_t {
    uint8_t default_mode;
    uint8_t minimum_length;
    zmk_key_t macos_key;
    zmk_key_t linux_key;
    zmk_key_t linux_alt_key;
    zmk_key_t win_compose_key;
};

struct behavior_unicode_data_t {
    uint8_t mode;
    zmk_mod_flags_t mod_mask;
};
