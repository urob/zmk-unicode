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
#include <zmk/hid.h>
#include <dt-bindings/zmk/modifiers.h>
#include <zmk-unicode/input_system.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_unicode_config {
    const char *codepoint;
    const char *shifted_codepoint;
};

static int on_unicode_binding_pressed(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_unicode_config *cfg = dev->config;

    if (zmk_hid_get_explicit_mods() & (MOD_LSFT | MOD_RSFT)) {
        send_unicode_sequence(&event, cfg->shifted_codepoint);
    } else {
        send_unicode_sequence(&event, cfg->codepoint);
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

static int behavior_unicode_init(const struct device *dev) { return 0; }

#define UC_INST(n)                                                                                 \
    static struct behavior_unicode_config behavior_unicode_config_##n = {                          \
        .codepoint = DT_INST_PROP(n, codepoint),                                                   \
        .shifted_codepoint = DT_INST_PROP_OR(n, shifted_codepoint, DT_INST_PROP(n, codepoint)),    \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_unicode_init, NULL, NULL, &behavior_unicode_config_##n,    \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                      \
                            &behavior_unicode_driver_api);

DT_INST_FOREACH_STATUS_OKAY(UC_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
