#include <Arduino.h>
#include <U8g2lib.h>
#include <vector>
#include <STM32FreeRTOS.h>
#include "global_variables.h"
#include "global_functions.h"
#include "sound.h"
  
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);


// sampleISR
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
  volatile uint8_t element;
  DAC_HandleTypeDef hdac;

void sampleISR() {
  static uint32_t readCtr = 0;
  if (readCtr == SAMPLE_BUFFER_SIZE) {
    // xSemaphoreTakeFromISR(sampleBufferSemaphore, NULL);
   
    readCtr = 0;
    writeBuffer1 = !writeBuffer1;
    xSemaphoreGiveFromISR(sampleBufferSemaphore, NULL);

  }
  if (writeBuffer1){
    analogWrite(OUTR_PIN, sampleBuffer0[readCtr++]);
  }
  else{
    analogWrite(OUTR_PIN, sampleBuffer1[readCtr++]);}
  
}


TIM_TypeDef *Instance = TIM1;
HardwareTimer *sampleTimer = new HardwareTimer(Instance);
uint8_t readCols()
{
  uint8_t C0 = digitalRead(C0_PIN);
  uint8_t C1 = digitalRead(C1_PIN);
  uint8_t C2 = digitalRead(C2_PIN);
  uint8_t C3 = digitalRead(C3_PIN);
  uint8_t concate_result = (C3 << 3) | (C2 << 2) | (C1 << 1) | C0;
  return concate_result;
}

void setRow(uint8_t rowIdx)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, rowIdx & 1);
  digitalWrite(RA1_PIN, (rowIdx >> 1) & 1);
  digitalWrite(RA2_PIN, (rowIdx >> 2) & 1);
  digitalWrite(REN_PIN, HIGH);
}

//scanning the key and update currentStepSize, Keysarray 
void scanKeysTask(void * pvParameters) {
  Serial.println("inside scankeys task");
  const TickType_t xFrequency = 70 / portTICK_PERIOD_MS;
  // xFrequency initiation interval of task set to 50ms
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // xLastWakeTime will store the time (tick count) of the last initiation.
  uint8_t knob3_current_val;
  uint8_t localkeyArray[7];
  // uint8_t previouslocalkeyArray[7];
  uint32_t previous_keys = 4095;
  uint32_t current_keys = 0;
  uint32_t current_keys_shifted;
  uint32_t xor_keys;
  uint8_t local_octave;
  bool pressed;


  while (1) {
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    for (uint8_t i = 0; i < 7; i++){
        setRow(i);
        digitalWrite(OUT_PIN, outBits[i]); //output handshake signal
        delayMicroseconds(3);
        localkeyArray[i] = readCols();
    }
    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    std::copy(localkeyArray,localkeyArray+7,keyArray);
    xSemaphoreGive(keyArrayMutex);

    /// send message if any key changed
    current_keys = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    //previous_keys = previouslocalkeyArray[2] << 8 | previouslocalkeyArray[1] << 4 | previouslocalkeyArray[0];
    xor_keys = current_keys ^ previous_keys;
    // Serial.print("xor: ");
    // Serial.println(xor_keys, BIN);
    // for sound output
    
    /// modified sound map for local key press 
    current_keys_shifted = current_keys;
    
    if ((xor_keys) != 0){

      local_octave = __atomic_load_n(&octave, __ATOMIC_RELAXED);
   
      for (int i = 0; i < 12; i++){
        if ((xor_keys & 1) == 1){
          pressed = !(current_keys_shifted & 1);
          modified_soundMap(local_octave, i, pressed);
          // sendMessage(i, pressed);
        }
        xor_keys = xor_keys >> 1;
        current_keys_shifted = current_keys_shifted >> 1;
      }
      
      /// set it to previous_keys in the end. And only if current_keys is different from previous_keys
      previous_keys = current_keys; 
    }
  }


  // while(1){
  //   vTaskDelayUntil( &xLastWakeTime, xFrequency );

  //   for(int i=0; i<3; i++){
  //       setRow(i);
  //       delayMicroseconds(3);
  //       uint8_t keys = readCols();
  //       keyArray[i] = keys;
  //   }
    
   
  //   // uint16_t B3 = ((uint16_t)keyArray[2]) << 8;
  //   // uint16_t B2 = ((uint16_t)keyArray[1]) << 4;
  //   // uint16_t B99 = (uint16_t)keyArray[0];
  //   uint16_t keyArray_concated = keyArray[2] << 8 | keyArray[1] << 4 | keyArray[0];
  //   //  = B3 | B2| B99;
    
  //   __atomic_store_n(&global_keyArray_concated, keyArray_concated, __ATOMIC_RELAXED);
  // }
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

  Serial.begin(9600);
  // put your setup code here, to run once:
  setPinDirections();
  Serial.println("Hello World");
  //Initialise display
  setOutMuxBit(DRST_BIT, LOW);  //Assert display logic reset
  delayMicroseconds(2);
  setOutMuxBit(DRST_BIT, HIGH);  //Release display logic reset
  u8g2.begin();
  setOutMuxBit(DEN_BIT, HIGH);  //Enable display power supply

  //Initialise UART
  
  Serial.println("Hello World");


  std::vector <uint16_t> tmp;
  sound_table.insert(std::make_pair(octave,tmp ));
  Serial.print("check map size");
  Serial.println(sound_table.size());


  // Timer Setting
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  Serial.print("Inity");
  initialize_table();
  
  // hdac.Instance = DAC1;
  // if (HAL_DAC_Init(&hdac) != HAL_OK)
  // {
  //   Serial.println("hdac instance init fail");
  // }

  // create the mutex and assign its handle in the setup function
  keyArrayMutex = xSemaphoreCreateMutex();
  sampleBufferSemaphore = xSemaphoreCreateBinary();
  sound_tableMutex = xSemaphoreCreateMutex();
  xSemaphoreGive(sampleBufferSemaphore);
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

  xTaskCreate(
  displayUpdateTask,		    /* Function that implements the task */
  "displayKeys",		      /* Text name for the task */
  128,      		        /* Stack size in words, not bytes */
  NULL,			          /* Parameter passed into the task */
  1,			            /* Task priority */
  &displayKeysHandle );  /* Pointer to store the task handle */

  xTaskCreate(
  write_to_double_buffer,		    /* Function that implements the task */
  "write_to_buffer",		      /* Text name for the task */
  256,      		        /* Stack size in words, not bytes */
  NULL,			          /* Parameter passed into the task */
  1,			            /* Task priority */
  &write_to_double);  /* Pointer to store the task handle */

  vTaskStartScheduler();
}

void loop() {
  // Serial.println("hihihi");
}



