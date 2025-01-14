#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Função para calcular a temperatura interna com base nos valores do sensor
float read_internal_temperature() {
    // Lê o valor bruto do ADC
    uint16_t adc_value = adc_read();

    // Converte o valor do ADC em tensão
    float voltage = adc_value * (3.3f / (1 << 12)); // 12-bit ADC resolution

    // Calcula a temperatura em graus Celsius
    float temperature_celsius = 27.0f - (voltage - 0.706f) / 0.001721f;

    return temperature_celsius;
}

// Converte temperatura de Celsius para Fahrenheit
float celsius_to_fahrenheit(float temp_celsius) {
    return (temp_celsius * 9.0f / 5.0f) + 32.0f;
}

int main() {
    // Inicializa a UART para saída no terminal
    stdio_init_all();

    // Inicializa o ADC e seleciona o canal 4 (sensor de temperatura interno)
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    while (true) {
        // Lê e calcula a temperatura
        float temperatureC = read_internal_temperature();

        // Exibe os resultados no console
        printf("Internal Temperature: %.2f °C\n", temperatureC);

        // Espera 1 segundo antes de medir novamente
        sleep_ms(1000);
    }

    return 0;
}
