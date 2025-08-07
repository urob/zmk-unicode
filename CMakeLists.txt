if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_UNICODE app PRIVATE src/behaviors/behavior_unicode.c)
  target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_UNICODE app PRIVATE src/behaviors/behavior_mask_mods.c)

  zephyr_include_directories(include)
endif()
