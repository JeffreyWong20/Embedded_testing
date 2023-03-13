#include <Arduino.h>
#include <U8g2lib.h>
#include <vector>
#include <STM32FreeRTOS.h>

#include "global_variables.h"
#include "global_functions.h"
// #include "communication.h"
#include "sound.h"
#include "display.h"
#include "scan.h"
  
// Display Sound and scan key TODO
  // volatile uint8_t element;
  // DAC_HandleTypeDef hdac;

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




void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:
  setPinDirections();
  // Serial.println("Hello World");

  initialiseDisplay();

  // Timer Setting
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  Serial.print("Inity");
  initialize_table();
  
  initializeCAN();
  
// Declare Semaphore
  sound_tableMutex = xSemaphoreCreateMutex();
  RX_MessageMutex = xSemaphoreCreateMutex();
  CAN_TX_Semaphore = xSemaphoreCreateCounting(3,3);
  sampleBufferSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(sampleBufferSemaphore);
  // xSemaphoreGive(RX_MessageMutex);

// Initialize run thread
  TaskHandle_t scanKeysHandle = NULL;
  TaskHandle_t displayKeysHandle = NULL;
  TaskHandle_t write_to_double = NULL;
  TaskHandle_t transmitHandle = NULL;
  TaskHandle_t decodeHandle = NULL;

  xTaskCreate(decodeTask, "decode", 256, NULL, 2, &decodeHandle);   

  xTaskCreate(CAN_TX_Task, "transmit", 256, NULL, 3, &transmitHandle); 

  xTaskCreate(scanKeysTask, "scanKeys", 64, NULL,5, &scanKeysHandle);  

  xTaskCreate(displayUpdateTask,"displayKeys",128,NULL,0,&displayKeysHandle );

  xTaskCreate(write_to_double_buffer, "write_to_buffer",256,NULL,5,&write_to_double);  


  vTaskStartScheduler();
}

void loop() {}



