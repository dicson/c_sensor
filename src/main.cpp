#include <Arduino.h>
#include "enow.h"

#define ADC_PIN 34                 // Ваш пин подключения (GPIO 34)
#define LED_PIN 2                  //
#define ADC_BITS 12                // 12 бит разрешения АЦП ESP32
#define ADC_COUNTS (1 << ADC_BITS) // 4096

// --- НАСТРОЙКА ФИЛЬТРА И ШУМА ---
// Установите это значение чуть ВЫШЕ вашего фантомного тока.
// Если в покое показывает 1.0А, поставьте отсечку на 1.15А.
const float NOISE_THRESHOLD = 0.15;

// Калибровочный коэффициент (начните с теоретического 15.0 для SCT-013-015)
float CALIBRATION_FACTOR = 15.0;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogRead(ADC_PIN);
  analogReadResolution(ADC_BITS);

  // Принудительно настраиваем аттенюатор на диапазон до ~3.1-3.3В
  analogSetPinAttenuation(ADC_PIN, ADC_11db);
  esp_now_setup();
}

void loop()
{
  unsigned long start_time = millis();

  long sample_count = 0;
  float sq_current_sum = 0;

  // Переменные для фильтра постоянной составляющей (убираем 1.65В)
  float filtered_sample = 0;
  float last_filtered_sample = 0;
  float sample = analogRead(ADC_PIN);
  float last_sample = sample;

  // Измеряем сигнал в течение ровно 200 миллисекунд (10 полных периодов сети 50 Гц)
  while ((millis() - start_time) < 200)
  {
    last_sample = sample;
    sample = analogRead(ADC_PIN);
    sample_count++;

    // Цифровой фильтр высоких частот (ФВЧ)
    // Он «срезает» постоянные 1.65В и оставляет только чистую синусоиду шума или тока
    filtered_sample = 0.996 * (last_filtered_sample + sample - last_sample);

    // Суммируем квадраты мгновенных значений
    sq_current_sum += (filtered_sample * filtered_sample);

    last_filtered_sample = filtered_sample;
    delayMicroseconds(100); // Небольшая пауза для стабилизации АЦП
  }

  // Считаем среднеквадратичное значение АЦП
  float rms_adc = sqrt(sq_current_sum / sample_count);

  // Переводим попугаи АЦП в Амперы
  float Irms = rms_adc * (3.3 / ADC_COUNTS) * CALIBRATION_FACTOR;

  // --- МАГИЯ ОТСЕЧКИ ФАНТОМНОГО ТОКА ---
  if (Irms < NOISE_THRESHOLD)
  {
    Irms = 0.0;
  }

  // Вывод данных
  Serial.print("Irms: ");
  Serial.print(Irms, 2);
  Serial.print(" A | Power (~230V): ");
  Serial.print(Irms * 230.0, 0);
  Serial.println(" W");

  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  // Serial.println(analogRead(34));
  send_k_command(Irms);
  delay(1500);
}
