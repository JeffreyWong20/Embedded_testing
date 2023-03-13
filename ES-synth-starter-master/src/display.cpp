#include "display.h"

// Display driver object
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, bitIdx & 0x01);
  digitalWrite(RA1_PIN, bitIdx & 0x02);
  digitalWrite(RA2_PIN, bitIdx & 0x04);
  digitalWrite(OUT_PIN, value);
  digitalWrite(REN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(REN_PIN, LOW);
}


void displayUpdateTask(void *pvParameters)
{
    const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t output;
    const char *output_key;


    Key local_sound_table [12] = {}; 

    // Update display
    while (1)
    {
        # ifndef TEST_DISPLAY
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        # endif
        
        u8g2.clearBuffer();                 // clear the internal memory
        u8g2.setFont(u8g2_font_5x7_tr); // choose a suitable font

        xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
        std::copy(sound_table, sound_table+12, local_sound_table);
        xSemaphoreGive(sound_tableMutex);
        u8g2.setCursor(2,10);
        u8g2.print("Note:");
        u8g2.setCursor(34,10);
        for (int m = 0; m < 12; m++){
            if(local_sound_table[m].key_index != 0){
                u8g2.print(Key_set[local_sound_table[m].key_index]);
            }
        }

        u8g2.setCursor(75,30);
        u8g2.print("Volume:");
        u8g2.setCursor(115,30);
        u8g2.print(knob3Rotation);


        u8g2.setCursor(75,10);
        u8g2.print("Octave:");
        u8g2.setCursor(115,10);
        u8g2.print(octave);

        u8g2.sendBuffer();
        //Toggle LED
        digitalToggle(LED_BUILTIN);
        # ifdef TEST_DISPLAY
        break;
        #endif
    }
}

void initialiseDisplay(){
    setOutMuxBit(DRST_BIT, LOW); // Assert display logic reset
    delayMicroseconds(2);
    setOutMuxBit(DRST_BIT, HIGH); // Release display logic reset
    u8g2.begin();
    setOutMuxBit(DEN_BIT, HIGH); // Enable display power supply
}