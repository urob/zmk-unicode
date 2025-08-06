#include <string.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <dt-bindings/zmk/keys.h>
#include <zmk-unicode/send_string.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/**
 * Iterate over a codepoint string and add keycodes to behavior queue.
 *
 * This is a crude implementation in lieue of a future zmk_send_key behavior.
 * There is no error checking of the input sequence and the charmap is
 * hardcoded to US_EN. This will eventually be replaced by a call to
 * zmk_send_keys once it is implemented in upstream ZMK.
 */
void send_string(const struct zmk_behavior_binding_event *event, const char *codepoint) {
    for (char current = *codepoint; current != '\0'; current = *++codepoint) {
        struct zmk_behavior_binding binding;

        if (strcmp(&current, "0") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N0};
        } else if (strcmp(&current, "1") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N1};
        } else if (strcmp(&current, "2") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N2};
        } else if (strcmp(&current, "3") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N3};
        } else if (strcmp(&current, "4") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N4};
        } else if (strcmp(&current, "5") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N5};
        } else if (strcmp(&current, "6") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N6};
        } else if (strcmp(&current, "7") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N7};
        } else if (strcmp(&current, "8") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N8};
        } else if (strcmp(&current, "9") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N9};
        } else if (strcmp(&current, "A") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = A};
        } else if (strcmp(&current, "B") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = B};
        } else if (strcmp(&current, "C") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = C};
        } else if (strcmp(&current, "D") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = D};
        } else if (strcmp(&current, "E") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = E};
        } else if (strcmp(&current, "F") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = F};
        } else {
            LOG_WRN("Failed to map codepoint %s to a behavior binding", &current);
            continue;
        }

        zmk_behavior_queue_add(event, binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
        zmk_behavior_queue_add(event, binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
    }
}
