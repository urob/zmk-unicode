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

        if (current == '0') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N0};
        } else if (current == '1') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N1};
        } else if (current == '2') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N2};
        } else if (current == '3') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N3};
        } else if (current == '4') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N4};
        } else if (current == '5') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N5};
        } else if (current == '6') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N6};
        } else if (current == '7') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N7};
        } else if (current == '8') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N8};
        } else if (current == '9') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N9};
        } else if (current == 'A') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = A};
        } else if (current == 'B') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = B};
        } else if (current == 'C') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = C};
        } else if (current == 'D') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = D};
        } else if (current == 'E') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = E};
        } else if (current == 'F') {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = F};
        } else {
            LOG_WRN("Failed to map codepoint %c to a behavior binding", current);
            continue;
        }

        zmk_behavior_queue_add(event, binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
        zmk_behavior_queue_add(event, binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
    }
}
