cmake_minimum_required(VERSION 3.8)
project(NB_IOT_test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        eeprom/eeprom.c
        eeprom/eeprom.h
        led/lcd.h
        led/led.c
        app.c
        app.h)

add_executable(NB_IOT_test ${SOURCE_FILES})