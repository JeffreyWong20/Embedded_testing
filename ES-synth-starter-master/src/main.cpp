#include <Arduino.h>
#include <U8g2lib.h>
#include <vector>
#include <STM32FreeRTOS.h>
#include "global_variables.h"
#include "global_functions.h"
#include "sound.h"
#include "scanKeys.h"
  
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);
TIM_TypeDef *Instance = TIM1;
HardwareTimer *sampleTimer = new HardwareTimer(Instance);

// sampleISR
// volatile uint8_t element;
// void sampleISR() {
//   digitalToggle(LED_BUILTIN);
//   static int32_t phaseAcc = 0;
//   phaseAcc += currentStepSize;
//   static uint32_t local_timestep [12] = {
//     0, 0, 0, 0,
//     0, 0, 0, 0,
//     0, 0, 0, 0
//   };
  
//   int32_t Vout = 0;
//   for(int i=0; i< element ;i++){
//     local_timestep[a[i]] += 1;
//     if(local_timestep[a[i]]>tableSizes_sub1[a[i]]){
//         local_timestep[a[i]] = 0;
//     };
//     Vout += centralOctaveLookUpTable.accessTable(a[i],local_timestep[a[i]]);
//   };
  
//   if(element != 0 ){
//     Vout = Vout / element;
//     analogWrite(OUTR_PIN, Vout + 128);
//   }
//   digitalToggle(LED_BUILTIN);
// }

//Display Sound and scan key
// void sampleISR() {
//   static uint32_t readCtr = 0;
//   if (readCtr == SAMPLE_BUFFER_SIZE) {
//     // xSemaphoreTakeFromISR(sampleBufferSemaphore, NULL);
   
//     readCtr = 0;
//     writeBuffer1 = !writeBuffer1;
//     xSemaphoreGiveFromISR(sampleBufferSemaphore, NULL);

//   }
//   if (writeBuffer1){
//     analogWrite(OUTR_PIN, sampleBuffer0[readCtr++]);
//   }
//   else{
//     analogWrite(OUTR_PIN, sampleBuffer1[readCtr++]);}
  
// }


void sampleISR() {
  digitalToggle(LED_BUILTIN);

  int32_t Vout = 0;
  Vout = calculate_vout();
  std::map<uint8_t, std::vector<uint16_t> > local_sound_table; 
  //xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  //Serial.println(sound_table.size());
  //xSemaphoreGive(sound_tableMutex);

  // analogWrite(OUTR_PIN, Vout + 128);
  Serial.print(Vout);
  
  digitalToggle(LED_BUILTIN);
}

//displaying keysarray 
void displayUpdateTask(void * pvParameters){
  const TickType_t xFrequency = 100/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1){
    vTaskDelayUntil( &xLastWakeTime, xFrequency );

    //Update display
    u8g2.clearBuffer();         // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.drawStr(2,10,"Helllo World!");  // write something to the internal memory
    u8g2.sendBuffer();          // transfer internal memory to the display
  
  }
}

//-------------------------------------Buffer tasked-------------------------------






//Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value) {
      digitalWrite(REN_PIN,LOW);
      digitalWrite(RA0_PIN, bitIdx & 0x01);
      digitalWrite(RA1_PIN, bitIdx & 0x02);
      digitalWrite(RA2_PIN, bitIdx & 0x04);
      digitalWrite(OUT_PIN,value);
      digitalWrite(REN_PIN,HIGH);
      delayMicroseconds(2);
      digitalWrite(REN_PIN,LOW);
}

void setup() {

  // put your setup code here, to run once:
  setPinDirections();
  //Initialise display
  setOutMuxBit(DRST_BIT, LOW);  //Assert display logic reset
  delayMicroseconds(2);
  setOutMuxBit(DRST_BIT, HIGH);  //Release display logic reset
  u8g2.begin();
  setOutMuxBit(DEN_BIT, HIGH);  //Enable display power supply

  //Initialise UART
  Serial.begin(9600);
  Serial.println("Hello World");

  //Init sound table
  std::vector <uint16_t> tmp;
  sound_table.insert(std::make_pair(octave,tmp ));
  Serial.print("check map size");
  Serial.println(sound_table.size());

  //Init sound Array
  for(int i=0; i<4; i++){
    sound_array[i] = 0 ;
  }

  //Init sine table
  Serial.println("Init Table");
  initialize_table();

  // Timer Setting
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  
  // create the mutex and assign its handle in the setup function
  keyArrayMutex = xSemaphoreCreateMutex();
  sound_tableMutex = xSemaphoreCreateMutex();
  sampleBufferSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(sampleBufferSemaphore);
  xSemaphoreGive(sound_tableMutex);

  // Initialize run thread
  TaskHandle_t scanKeysHandle = NULL;
  TaskHandle_t displayKeysHandle = NULL;
  TaskHandle_t write_to_double = NULL;

  xTaskCreate(
  scanKeysTask,		    /* Function that implements the task */
  "scanKeys",		      /* Text name for the task */
  64,      		        /* Stack size in words, not bytes */
  NULL,			          /* Parameter passed into the task */
  1,			            /* Task priority */
  &scanKeysHandle );  /* Pointer to store the task handle */

  // xTaskCreate(
  // displayUpdateTask,		    /* Function that implements the task */
  // "displayKeys",		      /* Text name for the task */
  // 128,      		        /* Stack size in words, not bytes */
  // NULL,			          /* Parameter passed into the task */
  // 1,			            /* Task priority */
  // &displayKeysHandle );  /* Pointer to store the task handle */

  // xTaskCreate(
  // write_to_double_buffer,		    /* Function that implements the task */
  // "write_to_buffer",		      /* Text name for the task */
  // 256,      		        /* Stack size in words, not bytes */
  // NULL,			          /* Parameter passed into the task */
  // 1,			            /* Task priority */
  // &write_to_double);  /* Pointer to store the task handle */

  vTaskStartScheduler();
}

void loop() {
  // Serial.println("hihihi");
}



