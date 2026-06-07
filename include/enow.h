#ifndef MY_DATA_H
#define MY_DATA_H

typedef struct __attribute__((packed))
{
    int32_t relay;   // номер реле (255 = установка K)(254 = сила тока)
    uint8_t state;   // вкл/выкл
    float k_value; // коэффициент K (используется при relay == 255) или сила тока при relay = 254
} struct_command;

void esp_now_setup();
void send_k_command(float k);

#endif // MY_DATA_H