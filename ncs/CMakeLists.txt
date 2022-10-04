if(CONFIG_SUIT_PROCESSOR)
  add_subdirectory(".." suit)
  target_link_libraries(suit PUBLIC zephyr_interface)
  target_link_libraries(manifest PUBLIC zephyr_interface)
  target_link_libraries(cose PUBLIC zephyr_interface)

  zephyr_interface_library_named(suit_processor_interface)
  target_link_libraries(suit_processor_interface INTERFACE suit)
endif() # CONFIG_SUIT_PROCESSOR
