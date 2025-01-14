cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk from installed location
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
project(telinha C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(telinha display_oled.c)

pico_set_program_name(telinha "telinha")
pico_set_program_version(telinha "0.1")

# Enable or disable output over UART/USB
pico_enable_stdio_uart(telinha 0)
pico_enable_stdio_usb(telinha 0)

# Include the library path
add_subdirectory("/home/heitornery/Downloads/pico_wifi/display_oled/pico-ssd1306") # <- Atualize conforme o local real da biblioteca

# Link libraries to the project
target_link_libraries(telinha
    pico_stdlib
    hardware_i2c
    pico-ssd1306 # <- Certifique-se de que este nome corresponde ao da biblioteca
)

# Add include directories
target_include_directories(telinha PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/pico-ssd1306 # <- Atualize conforme a estrutura do repositório
)
