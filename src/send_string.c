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

        if (strcmp(&current, "a") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = A};
        } else if (strcmp(&current, "b") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = B};
        } else if (strcmp(&current, "c") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = C};
        } else if (strcmp(&current, "d") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = D};
        } else if (strcmp(&current, "e") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = E};
        } else if (strcmp(&current, "f") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = F};
        } else if (strcmp(&current, "g") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = G};
        } else if (strcmp(&current, "h") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = H};
        } else if (strcmp(&current, "i") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = I};
        } else if (strcmp(&current, "j") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = J};
        } else if (strcmp(&current, "k") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = K};
        } else if (strcmp(&current, "l") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = L};
        } else if (strcmp(&current, "m") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = M};
        } else if (strcmp(&current, "n") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = N};
        } else if (strcmp(&current, "o") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = O};
        } else if (strcmp(&current, "p") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = P};
        } else if (strcmp(&current, "q") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = Q};
        } else if (strcmp(&current, "r") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = R};
        } else if (strcmp(&current, "s") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = S};
        } else if (strcmp(&current, "t") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = T};
        } else if (strcmp(&current, "u") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = U};
        } else if (strcmp(&current, "v") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = V};
        } else if (strcmp(&current, "w") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = W};
        } else if (strcmp(&current, "x") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = X};
        } else if (strcmp(&current, "y") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = Y};
        } else if (strcmp(&current, "z") == 0) {
            binding = (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = Z};
        } else if (strcmp(&current, "0") == 0) {
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
        } else {
            LOG_WRN("Failed to map codepoint %s to a behavior binding", &current);
            continue;
        }

        zmk_behavior_queue_add(event, binding, true, CONFIG_ZMK_UNICODE_TAP_MS);
        zmk_behavior_queue_add(event, binding, false, CONFIG_ZMK_UNICODE_WAIT_MS);
    }
}
