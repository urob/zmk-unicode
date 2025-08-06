#pragma once

#include <zmk/behavior.h>
#include <dt-bindings/zmk/modifiers.h>

#define ALL_MODS                                                                                   \
    (MOD_LSFT | MOD_RSFT | MOD_LCTL | MOD_RCTL | MOD_LALT | MOD_RALT | MOD_LGUI | MOD_RGUI)

struct uc_input_system_config {
    int default_mode;
    int macos_key;
    int linux_key;
    int linux_alt_key;
    int win_compose_key;
};

struct uc_input_system_data {
    int mode;
};

void unicode_input_start(const struct zmk_behavior_binding_event *event);
void unicode_input_stop(const struct zmk_behavior_binding_event *event);
void send_unicode_sequence(const struct zmk_behavior_binding_event *event, const char *codepoint);
