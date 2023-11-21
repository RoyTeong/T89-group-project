add_executable(implementation main.c 
                    infrared.h
                    magnometer.h
                    motor.h
                    ultrasonic_sensor.h
                    wifi.h)

target_link_libraries(implementation pico_stdlib 
                        hardware_i2c 
                        hardware_pwm 
                        hardware_timer
                        hardware_gpio
                        hardware_dma
                        pico_lwip_iperf
                        pico_cyw43_arch_lwip_threadsafe_background)

target_include_directories(implementation PRIVATE
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
)

pico_enable_stdio_usb(implementation 1)

pico_enable_stdio_uart(implementation 1)

pico_add_extra_outputs(implementation)

example_auto_set_url(implementation)


