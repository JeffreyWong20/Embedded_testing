#include <algorithm>
#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

// test mode enable initialisation
// #define DISABLE_THREADS
// #define TEST_SCANKEYS
// #define TEST_DISPLAY
// # define TEST_DECODE
// # define TEST_TRANSMIT


#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

// // Constants

extern const uint32_t interval;

// // Pin definitions
// // Row select and enable
extern const int RA0_PIN;
extern const int RA1_PIN;
extern const int RA2_PIN;
extern const int REN_PIN;

// Matrix input and output
extern const int C0_PIN;
extern const int C1_PIN;
extern const int C2_PIN;
extern const int C3_PIN;
extern const int OUT_PIN;

// Audio analogue out
extern const int OUTL_PIN;
extern const int OUTR_PIN;

// Joystick analogue in
extern const int JOYY_PIN;
extern const int JOYX_PIN;

// Output multiplexer bits
extern const int DEN_BIT;
extern const int DRST_BIT;
extern const int HKOW_BIT;
extern const int HKOE_BIT;

// Global variables
extern const int32_t stepSizes[12];
extern const char *Key_set[13];
extern volatile uint8_t octave;
extern volatile int8_t knob3Rotation;
extern volatile uint8_t keyArray[7];
extern volatile uint8_t globalTX_Message[8];
extern volatile uint8_t globalRX_Message[8];
extern volatile QueueHandle_t msgInQ;
extern volatile QueueHandle_t msgOutQ;
extern SemaphoreHandle_t keyArrayMutex;
extern SemaphoreHandle_t RX_MessageMutex;
extern SemaphoreHandle_t CAN_TX_Semaphore;
extern SemaphoreHandle_t sound_tableMutex;

extern SemaphoreHandle_t sampleBufferSemaphore;
extern volatile uint16_t global_keyArray_concated;

extern volatile bool writeBuffer1;
extern volatile bool outBits[8];



//Activating part
extern uint32_t local_timestep [12];
const uint16_t SAMPLE_BUFFER_SIZE = 168;
extern uint8_t sampleBuffer0[SAMPLE_BUFFER_SIZE];
extern uint8_t sampleBuffer1[SAMPLE_BUFFER_SIZE];

struct Key {
    uint8_t octave;
    uint8_t key_index;
};

extern Key sound_table [12];

//setup function
void setPinDirections();







#endif