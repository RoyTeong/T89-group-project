if (NOT PICO_NO_HARDWARE)
    add_subdirectory(irline)
    add_subdirectory(motor_control)
    add_subdirectory(magnetometer)
    add_subdirectory(barcode)
    add_subdirectory(wheel_encoder)
    add_subdirectory(ultrasonic)
    add_subdirectory(wifi_server)
endif ()
