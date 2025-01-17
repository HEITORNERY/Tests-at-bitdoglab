#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <string.h>

// Definições para o display SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C // Endereço I2C do display
#define I2C_SDA 14          // Pino SDA
#define I2C_SCL 15          // Pino SCL

// Definições para o Wi-Fi
#define WIFI_SSID "Venancio_2.4GHz"
#define WIFI_PASS "21071314"

// Instância do display SSD1306
ssd1306_t display;

// Função para calcular a temperatura interna com base nos valores do sensor
float read_internal_temperature() {
    uint16_t adc_value = adc_read();
    float voltage = adc_value * (3.3f / (1 << 12)); // 12-bit ADC resolution
    float temperature_celsius = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature_celsius;
}

// Função para exibir texto no display OLED
void display_message(const char *message, int delay_ms) {
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 0, 0, 1, message);
    ssd1306_show(&display);
    sleep_ms(delay_ms);
}

// Função para exibir a temperatura no display OLED
void display_temperature(float temperature) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Temp: %.2f C", temperature);
    display_message(buffer, 2000); // Exibe a temperatura por 2 segundos
}

// Função para enviar a temperatura para um servidor via HTTP
void send_temperature_to_server(float temperature) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        display_message("Erro ao criar PCB", 2000);
        return;
    }

    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, 192, 168, 0, 103); // IP do servidor

    if (tcp_connect(pcb, &server_ip, 5000, NULL) != ERR_OK) { // Porta 5000
        printf("Erro ao conectar ao servidor\n");
        display_message("Erro ao conectar", 2000);
        return;
    }

    display_message("Conectado ao Servidor!", 2000);

    char request[256];
    snprintf(request, sizeof(request), "GET /update?temp=%.2f HTTP/1.1\r\nHost: 10.30.44.103\r\n\r\n", temperature);
    tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_close(pcb);

    display_message("Dado enviado", 2000);
}

int main() {
    stdio_init_all();
    sleep_ms(10000);

    // Inicializa o ADC e seleciona o canal 4 (sensor de temperatura interno)
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // Inicializa I2C no canal 1
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) {
        printf("Falha ao inicializar o display SSD1306\n");
        display_message("Falha ao inicializar", 2000);
        return 1;
    }

    printf("Display SSD1306 inicializado com sucesso!\n");
    display_message("Display inicializado", 2000);

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        display_message("Erro ao inicializar Wi-Fi", 2000);
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    display_message("Conectando ao Wi-Fi", 2000);

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        display_message("Falha ao conectar Wi-Fi", 2000);
        return 1;
    } else {
        printf("Conectado.\n");
        display_message("Wi-Fi conectado", 2000);
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    while (true) {
        float temperatureC = read_internal_temperature();
        printf("Internal Temperature: %.2f °C\n", temperatureC);
        display_temperature(temperatureC);
        send_temperature_to_server(temperatureC);
        sleep_ms(10000); // Envia a cada 10 segundos
    }
}
