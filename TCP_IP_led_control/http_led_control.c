#include "pico/cyw43_arch.h"  // Biblioteca para controle do Wi-Fi no Raspberry Pi Pico W
#include "pico/stdlib.h"      // Biblioteca padrão para GPIOs e outros
#include "lwip/tcp.h"         // Biblioteca para comunicação TCP
#include <string.h>           // Biblioteca para manipulação de strings
#include <stdio.h>            // Biblioteca padrão para entrada/saída (printf)

// Define o pino usado para controlar o LED
#define LED_PIN 12

// Define as credenciais do Wi-Fi
#define WIFI_SSID "ESSE WIFI NÃO É PARA VOCÊ"    // Nome da rede Wi-Fi
#define WIFI_PASS "usapobre"  // Senha da rede Wi-Fi

// Define a resposta HTTP com opções para ligar e desligar o LED
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><body>" \
                      "<h1>Controle do LED</h1>" \
                      "<p><a href=\"/led/on\">Ligar LED</a></p>" \
                      "<p><a href=\"/led/off\">Desligar LED</a></p>" \
                      "</body></html>\r\n"

// Callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {  // Se não há dados, a conexão foi fechada
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;  // Obtem os dados da requisição HTTP

    // Verifica qual URL foi requisitada
    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_PIN, 1);  // Liga o LED
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_PIN, 0);  // Desliga o LED
    }

    // Envia a resposta HTML
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);  // Libera o buffer recebido
    return ERR_OK;
}

// Callback para gerenciar conexões recebidas
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);  // Associa o callback HTTP para esta conexão
    return ERR_OK;
}

// Função que inicia o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();  // Cria um novo PCB para TCP
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {  // Tenta usar a porta 80
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);  // Configura o PCB em modo escuta
    tcp_accept(pcb, connection_callback);  // Define o callback para novas conexões
    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa a comunicação de saída padrão
    sleep_ms(10000);   // Aguarda antes de iniciar
    printf("Iniciando servidor HTTP\n");

    if (cyw43_arch_init()) {  // Inicializa o Wi-Fi
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();  // Habilita o modo estação (cliente)
    printf("Conectando ao Wi-Fi...\n");

    // Tenta conectar à rede Wi-Fi
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    } else {
        printf("Conectado.\n");
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    // Configura o pino do LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicia o servidor HTTP
    start_http_server();

    // Mantém o programa em execução e o Wi-Fi ativo
    while (true) {
        cyw43_arch_poll();  // Necessário para manter a conexão Wi-Fi
        sleep_ms(100);      // Aguarda brevemente antes de verificar novamente
    }

    cyw43_arch_deinit();  // Finaliza a operação do Wi-Fi (nunca será chamado)
    return 0;
}
