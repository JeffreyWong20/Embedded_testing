#include"global_variables.h"

int joystick1 = 512, joystick2 = 512;  

void joystickTask(void * pvParameters) {
  const TickType_t xFrequency = 10/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime= xTaskGetTickCount();
  while(1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Read joytick y axis and update vibrato value
    joystick1 = 512 - analogRead(JOYX_PIN);
    joystick1 = abs(joystick1) / 100;
    Serial.print(joystick1);
    // __atomic_store_n(&vibrato_amp, joystick1, __ATOMIC_RELAXED);
    // __atomic_store_n(&vibrato_freq, 4 * joystick1, __ATOMIC_RELAXED);

    // Read joytick x axis and update tremolo value
    joystick2 = 512 - analogRead(JOYY_PIN);
    joystick2 = abs(joystick2) / 100;
    // __atomic_store_n(&tremolo_freq, 10, __ATOMIC_RELAXED);
    // __atomic_store_n(&tremolo_amp, 0.2 * joystick2, __ATOMIC_RELAXED);
  }
}