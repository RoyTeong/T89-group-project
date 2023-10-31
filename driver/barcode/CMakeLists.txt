add_executable(barcode barcode.c)

# pull in common dependencies
target_link_libraries(barcode pico_stdlib 
hardware_pwm 
hardware_adc 
hardware_timer
pico_time)

# enable usb output, disable uart output
pico_enable_stdio_usb(barcode 1)
pico_enable_stdio_uart(barcode 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(barcode)

# add url via pico_set_program_url
example_auto_set_url(barcode)