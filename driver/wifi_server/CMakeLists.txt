add_executable(wifi_server wifi_server.c)

target_include_directories(wifi_server PRIVATE
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
)

target_link_libraries(wifi_server pico_stdlib pico_lwip_iperf pico_cyw43_arch_lwip_poll)

pico_add_extra_outputs(wifi_server)

pico_enable_stdio_usb(wifi_server 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(wifi_server)

# add url via pico_set_program_url
example_auto_set_url(wifi_server)
