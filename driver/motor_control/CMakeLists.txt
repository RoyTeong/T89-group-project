add_executable(motor_control
motor_control.c
        )

target_link_libraries(motor_control pico_stdlib hardware_pwm)

pico_enable_stdio_usb(motor_control 1) # Enable USB serial
pico_enable_stdio_uart(motor_control 0) # Disable uart

# create map/bin/hex file etc.
pico_add_extra_outputs(motor_control)

# add url via pico_set_program_url
example_auto_set_url(motor_control)

