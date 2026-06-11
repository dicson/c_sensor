#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "enow.h"

// --- Константы ---
//static const uint8_t CONTROLLER_ADDRESS[] = {0xac, 0xa7, 0x04, 0xea, 0x07, 0x80};// test ac:a7:04:ea:07:80
static const uint8_t CONTROLLER_ADDRESS[] = {0x98, 0x88, 0xe0, 0x04, 0xe2, 0x48};
static esp_now_peer_info_t peerInfo;

/**
 * Отправляет значение силы тока на контроллер через ESP-NOW.
 *
 * @param i Значение силы тока.
 */
void send_k_command(float i)
{
    struct_command myData;
    myData.relay = 254;       // спец. значение = сила тока
    myData.state = false;     // не используется
    myData.k_value = i;
    esp_err_t result = esp_now_send(CONTROLLER_ADDRESS, (uint8_t *)&myData, sizeof(myData));

    if (result != ESP_OK)
    {
        Serial.printf("Send K error: 0x%02X\n", result);
    }
}

/**
 * Выполняет начальную настройку ESP-NOW и WiFi.
 * Устанавливает режим STA, фиксирует канал и регистрирует коллбэки.
 */
void esp_now_setup()
{
    // Минимальная инициализация - без disconnect
    WiFi.mode(WIFI_STA);
    delay(200);

    uint8_t newMACAddress[] = {0x68, 0x25, 0xdd, 0xfd, 0x24, 0x94}; // pump sensor 68:25:dd:fd:24:94
    esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

    // Установим канал 1 явно до инициализации ESP-NOW
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    // esp_wifi_set_ps(WIFI_PS_NONE);
    // esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

    esp_err_t err = esp_now_init();
    if (err != ESP_OK)
        return;


    memcpy(peerInfo.peer_addr, CONTROLLER_ADDRESS, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    esp_now_add_peer(&peerInfo);
}
