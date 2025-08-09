/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_unicode

#include <drivers/behavior.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/hid.h>
#include <zmk/keys.h>

#include <zmk-unicode/unicode.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

const struct behavior_unicode_config_t behavior_unicode_config = {
    .default_mode = DT_INST_PROP(0, default_mode),
    .minimum_length = DT_INST_PROP(0, minimum_length),
    .macos_key = DT_INST_PROP_OR(0, macos_key, LALT),
    .linux_key = DT_INST_PROP_OR(0, linux_key, LC(LS(U))),
    .linux_alt_key = DT_INST_PROP_OR(0, linux_alt_key, LC(LSHFT)),
    .win_compose_key = DT_INST_PROP_OR(0, win_compose_key, RALT),
};

struct behavior_unicode_data_t behavior_unicode_data = {};

static zmk_mod_flags_t get_explicit_mods_flag(zmk_key_t key) {
    uint16_t keycode = ZMK_HID_USAGE_ID(key);
    uint8_t page = ZMK_HID_USAGE_PAGE(key);

    if (!is_mod(page, keycode)) {
        return 0x00;
    }

    switch (keycode) {
    case HID_USAGE_KEY_KEYBOARD_LEFTCONTROL:
        return MOD_LCTL;
    case HID_USAGE_KEY_KEYBOARD_LEFTSHIFT:
        return MOD_LSFT;
    case HID_USAGE_KEY_KEYBOARD_LEFTALT:
        return MOD_LALT;
    case HID_USAGE_KEY_KEYBOARD_LEFT_GUI:
        return MOD_LGUI;
    case HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL:
        return MOD_RCTL;
    case HID_USAGE_KEY_KEYBOARD_RIGHTSHIFT:
        return MOD_RSFT;
    case HID_USAGE_KEY_KEYBOARD_RIGHTALT:
        return MOD_RALT;
    case HID_USAGE_KEY_KEYBOARD_RIGHT_GUI:
        return MOD_RGUI;
    default:
        LOG_WRN("Failed to extract mod flag from 0x%08X", key);
        return 0x00;
    }
}

static zmk_mod_flags_t get_mod_flags(zmk_key_t key) {
    zmk_mod_flags_t implicit_mods = SELECT_MODS(key);
    zmk_mod_flags_t explicit_mods = get_explicit_mods_flag(key);
    return implicit_mods | explicit_mods;
}

void queue_mask_mods(const struct zmk_behavior_binding_event *event,
                     struct zmk_behavior_binding *binding, zmk_mod_flags_t mods) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "mask_mods", .param1 = mods};
    zmk_behavior_queue_add(event, *binding, true, 0);
    zmk_behavior_queue_add(event, *binding, false, 0);
}

void queue_key_press(const struct zmk_behavior_binding_event *event,
                     struct zmk_behavior_binding *binding, zmk_key_t key) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = key};
    zmk_behavior_queue_add(event, *binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
}

void queue_key_release(const struct zmk_behavior_binding_event *event,
                       struct zmk_behavior_binding *binding, zmk_key_t key) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = key};
    zmk_behavior_queue_add(event, *binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
}

void queue_key_tap(const struct zmk_behavior_binding_event *event,
                   struct zmk_behavior_binding *binding, zmk_key_t key) {
    struct behavior_unicode_data_t *data = &behavior_unicode_data;

    // Temporarily unmask key if it is an explicit mod. No need to unmask
    // implicit mods as they are always exempted from masking.
    zmk_mod_flags_t mods = get_explicit_mods_flag(key);
    if (mods != 0x00) {
        queue_mask_mods(event, binding, data->mod_mask & ~mods);
    }

    queue_key_press(event, binding, key);
    queue_key_release(event, binding, key);

    if (mods != 0x00) {
        queue_mask_mods(event, binding, data->mod_mask);
    }
}

void unicode_input_start(const struct zmk_behavior_binding_event *event) {
    const struct behavior_unicode_config_t *cfg = &behavior_unicode_config;
    struct behavior_unicode_data_t *data = &behavior_unicode_data;
    struct zmk_behavior_binding binding;

    switch (data->mode) {
    case UC_MODE_MACOS:
        queue_key_press(event, &binding, cfg->macos_key);
        break;
    case UC_MODE_LINUX:
        queue_key_tap(event, &binding, cfg->linux_key);
        break;
    case UC_MODE_LINUX_ALT:
        queue_key_press(event, &binding, cfg->linux_alt_key);
        queue_key_tap(event, &binding, U);
        break;
    case UC_MODE_WIN_COMPOSE:
        queue_key_tap(event, &binding, cfg->win_compose_key);
        queue_key_tap(event, &binding, U);
        break;
    case UC_MODE_WIN_ALT:
        queue_key_press(event, &binding, LALT);
        queue_key_tap(event, &binding, KP_PLUS);
        break;
    case UC_MODE_EMACS:
        queue_key_tap(event, &binding, LC(X));
        queue_key_tap(event, &binding, N8);
        queue_key_tap(event, &binding, RET);
        break;
    default:
        LOG_WRN("Unknown input_mode %d", data->mode);
        break;
    }
}

void unicode_input_stop(const struct zmk_behavior_binding_event *event) {
    const struct behavior_unicode_config_t *cfg = &behavior_unicode_config;
    struct behavior_unicode_data_t *data = &behavior_unicode_data;
    struct zmk_behavior_binding binding;

    switch (data->mode) {
    case UC_MODE_MACOS:
        queue_key_release(event, &binding, cfg->macos_key);
        break;
    case UC_MODE_LINUX:
        queue_key_tap(event, &binding, SPACE);
        break;
    case UC_MODE_LINUX_ALT:
        queue_key_tap(event, &binding, SPACE);
        queue_key_release(event, &binding, cfg->linux_alt_key);
        break;
    case UC_MODE_WIN_COMPOSE:
        queue_key_tap(event, &binding, RET);
        break;
    case UC_MODE_WIN_ALT:
        queue_key_release(event, &binding, LALT);
        break;
    case UC_MODE_EMACS:
        queue_key_tap(event, &binding, RET);
        break;
    default:
        LOG_WRN("Unknown input_mode %d", data->mode);
        break;
    }
}

static uint8_t get_hexdigit(const uc_cp_t codepoint, int i) { return (codepoint >> 4 * i) & 0xf; }

void send_hexdigit(const struct zmk_behavior_binding_event *event,
                   struct zmk_behavior_binding *binding, const uint8_t hexdigit) {
    if (hexdigit < 0x0 || hexdigit > 0xf) {
        LOG_WRN("Failed to map hexdigit 0x%X to a behavior binding", hexdigit);
    }
    *binding =
        (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = hex_to_key[hexdigit]};
    zmk_behavior_queue_add(event, *binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
    zmk_behavior_queue_add(event, *binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
}

void unicode_input_sequence(const struct zmk_behavior_binding_event *event,
                            const uc_cp_t codepoint) {
    const struct behavior_unicode_config_t *cfg = &behavior_unicode_config;
    struct zmk_behavior_binding binding;

    // Send up to 6 hexdigits, the maximum supported by UTF-16.
    int i = 5;
    for (; i >= cfg->minimum_length; i--) {
        if (get_hexdigit(codepoint, i)) {
            break;
        }
    }
    for (; i >= 0; i--) {
        send_hexdigit(event, &binding, get_hexdigit(codepoint, i));
    }
}

void unicode_process_input(const struct zmk_behavior_binding_event *event,
                           const uc_cp_t codepoint) {
    const struct behavior_unicode_config_t *cfg = &behavior_unicode_config;
    struct behavior_unicode_data_t *data = &behavior_unicode_data;
    struct zmk_behavior_binding binding;

    if (codepoint == 0) {
        LOG_WRN("Code point is zero, aborting input");
        return;
    }
    if (data->mode < UC_MODE_MACOS || data->mode > UC_MODE_EMACS) {
        LOG_WRN("Unknown input_mode %d, aborting input", data->mode);
        return;
    }

    // Mask all mods except mods that need to be _held_ during input.
    data->mod_mask = ALL_MODS;
    switch (data->mode) {
    case UC_MODE_MACOS:
        data->mod_mask &= ~get_mod_flags(cfg->macos_key);
        break;
    case UC_MODE_LINUX_ALT:
        data->mod_mask &= ~get_mod_flags(cfg->linux_alt_key);
        break;
    case UC_MODE_WIN_ALT:
        data->mod_mask &= ~get_mod_flags(LALT);
        break;
    }

    LOG_DBG("input_mode %d codepoint 0x%06x masked_mods 0x%02X", data->mode, codepoint,
            data->mod_mask);
    queue_mask_mods(event, &binding, data->mod_mask);
    unicode_input_start(event);
    unicode_input_sequence(event, codepoint);
    unicode_input_stop(event);
    queue_mask_mods(event, &binding, 0x00);
}

void unicode_input_system_set(struct behavior_unicode_data_t *data, int mode) {
    LOG_DBG("Set input_mode to %d", mode);
    data->mode = mode;
}

static int on_unicode_binding_pressed(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);

    // Select input mode.
    if (binding->param1 == UC_SELECT_INPUT_MODE) {
        unicode_input_system_set(dev->data, binding->param2);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    // Process unicode input.
    bool shifted = (zmk_hid_get_explicit_mods() & (MOD_LSFT | MOD_RSFT)) != 0;
    if (shifted && binding->param2 != UC_NONE) {
        unicode_process_input(&event, binding->param2);
    } else {
        unicode_process_input(&event, binding->param1);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_unicode_binding_released(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_unicode_driver_api = {
    .binding_pressed = on_unicode_binding_pressed,
    .binding_released = on_unicode_binding_released,
};

static int behavior_unicode_init(const struct device *dev) {
    const struct behavior_unicode_config_t *cfg = dev->config;
    unicode_input_system_set(dev->data, cfg->default_mode);
    return 0;
}

BEHAVIOR_DT_INST_DEFINE(0, behavior_unicode_init, NULL, &behavior_unicode_data,
                        &behavior_unicode_config, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_unicode_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
