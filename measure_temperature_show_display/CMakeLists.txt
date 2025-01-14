cmake_minimum_required(VERSION 3.13)

# Configuração de projeto
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o pico_sdk
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

# Importa o pico_sdk
include(pico_sdk_import.cmake)
project(measure_temperature_display C CXX ASM)

# Inicializa o Raspberry Pi Pico SDK
pico_sdk_init()

# Adiciona o executável do projeto
add_executable(measure_temperature_display measure_temperature_show_display.c)

# Definição de informações do projeto
pico_set_program_name(measure_temperature_display "measure_temperature_display")
pico_set_program_version(measure_temperature_display "0.1")

# Configura opções de saída UART/USB
pico_enable_stdio_uart(measure_temperature_display 0)
pico_enable_stdio_usb(measure_temperature_display 1)

# Adiciona a biblioteca SSD1306 ao projeto
add_subdirectory("/home/heitornery/Downloads/pico_wifi/measure_temperature_show_display/pico-ssd1306") # Atualize este caminho para a localização da biblioteca

# Inclui as dependências ao projeto
target_link_libraries(measure_temperature_display
    pico_stdlib
    hardware_adc
    hardware_i2c
    pico-ssd1306 # Biblioteca SSD1306
)

# Adiciona os diretórios de cabeçalhos
target_include_directories(measure_temperature_display PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    "/home/heitornery/Downloads/pico_wifi/measure_temperature_show_display/pico-ssd1306/include" # Atualize este caminho
)

# Geração de arquivos adicionais
pico_add_extra_outputs(measure_temperature_display)
