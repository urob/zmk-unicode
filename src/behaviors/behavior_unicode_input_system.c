/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_unicode_input_system

#include <drivers/behavior.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/hid.h>

#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk-unicode/uc.h>
#include <zmk-unicode/input_system.h>
#include <zmk-unicode/send_string.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

const struct uc_input_system_config behavior_unicode_input_system_config = {
    .default_mode = DT_INST_PROP(0, default_mode),
    .macos_key = DT_INST_PROP_OR(0, macos_key, LALT),
    .linux_key = DT_INST_PROP_OR(0, linux_key, LC(LS(U))),
    .linux_alt_key = DT_INST_PROP_OR(0, linux_alt_key, LC(LSHFT)),
    .win_compose_key = DT_INST_PROP_OR(0, win_compose_key, RALT),
};

struct uc_input_system_data behavior_unicode_input_system_data = {};

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
        LOG_DBG("Failed to extract mod bit from 0x%08X", key);
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
}

void queue_key_press(const struct zmk_behavior_binding_event *event,
                     struct zmk_behavior_binding *binding, int keycode) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = keycode};
    zmk_behavior_queue_add(event, *binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
}

void queue_key_release(const struct zmk_behavior_binding_event *event,
                       struct zmk_behavior_binding *binding, int keycode) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = keycode};
    zmk_behavior_queue_add(event, *binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
}

void queue_key_tap_unguarded(const struct zmk_behavior_binding_event *event,
                             struct zmk_behavior_binding *binding, int keycode) {
    *binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = keycode};
    zmk_behavior_queue_add(event, *binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
    zmk_behavior_queue_add(event, *binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
}

void queue_key_tap(const struct zmk_behavior_binding_event *event,
                   struct zmk_behavior_binding *binding, int keycode) {
    zmk_mod_flags_t mods = get_explicit_mods_flag(keycode);

    // Temporarily unmask keycode if it is an explicit mod.
    if (mods != 0x00) {
        struct uc_input_system_data *data = &behavior_unicode_input_system_data;
        queue_mask_mods(event, binding, data->mod_mask & ~mods);
        queue_key_tap_unguarded(event, binding, keycode);
        queue_mask_mods(event, binding, data->mod_mask);
    } else {
        queue_key_tap_unguarded(event, binding, keycode);
    }
}

void unicode_input_start(const struct zmk_behavior_binding_event *event) {
    const struct uc_input_system_config *cfg = &behavior_unicode_input_system_config;
    struct uc_input_system_data *data = &behavior_unicode_input_system_data;
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
        LOG_DBG("Unknown input_mode %d", data->mode);
        break;
    }
}

void unicode_input_stop(const struct zmk_behavior_binding_event *event) {
    const struct uc_input_system_config *cfg = &behavior_unicode_input_system_config;
    struct uc_input_system_data *data = &behavior_unicode_input_system_data;
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
        LOG_DBG("Unknown input_mode %d", data->mode);
        break;
    }
}

void send_unicode_sequence(const struct zmk_behavior_binding_event *event, const char *codepoint) {
    const struct uc_input_system_config *cfg = &behavior_unicode_input_system_config;
    struct uc_input_system_data *data = &behavior_unicode_input_system_data;
    struct zmk_behavior_binding binding;

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
    LOG_DBG("input_mode %d codepoint %s masked_mods 0x%02X", data->mode, codepoint, data->mod_mask);
    queue_mask_mods(event, &binding, data->mod_mask);

    unicode_input_start(event);
    send_string(event, codepoint);
    unicode_input_stop(event);

    // Unmask mods at end of queue.
    queue_mask_mods(event, &binding, 0x00);
}

void set_unicode_input_system(struct uc_input_system_data *data, int mode) {
    LOG_DBG("Set input_mode to %d", mode);
    data->mode = mode;
}

static int on_unicode_input_system_binding_pressed(struct zmk_behavior_binding *binding,
                                                   struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    set_unicode_input_system(dev->data, binding->param1);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_unicode_input_system_binding_released(struct zmk_behavior_binding *binding,
                                                    struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_unicode_input_system_driver_api = {
    .binding_pressed = on_unicode_input_system_binding_pressed,
    .binding_released = on_unicode_input_system_binding_released,
};

static int behavior_unicode_input_system_init(const struct device *dev) {
    const struct uc_input_system_config *cfg = dev->config;
    set_unicode_input_system(dev->data, cfg->default_mode);
    return 0;
}

BEHAVIOR_DT_INST_DEFINE(0, behavior_unicode_input_system_init, NULL,
                        &behavior_unicode_input_system_data, &behavior_unicode_input_system_config,
                        POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_unicode_input_system_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
