#include "global_variables.h"


const uint32_t interval = 100; // Display update interval

// Pin definitions
// Row select and enable
const int RA0_PIN = D3;
const int RA1_PIN = D6;
const int RA2_PIN = D12;
const int REN_PIN = A5;

// Matrix input and output
const int C0_PIN = A2;
const int C1_PIN = D9;
const int C2_PIN = A6;
const int C3_PIN = D1;
const int OUT_PIN = D11;

// Audio analogue out
const int OUTL_PIN = A4;
const int OUTR_PIN = A3;

// Joystick analogue in
const int JOYY_PIN = A0;
const int JOYX_PIN = A1;

// Output multiplexer bits
const int DEN_BIT = 3;
const int DRST_BIT = 4;
const int HKOW_BIT = 5;
const int HKOE_BIT = 6;

// Constants for the global

const char * Key_set[13] = {"Not Pressed", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

volatile uint8_t octave = 1;

volatile int8_t knob3Rotation = 0;
volatile uint8_t keyArray[7] = {0};
volatile uint8_t globalTX_Message[8]={0};
volatile uint8_t globalRX_Message[8]={0};

volatile QueueHandle_t msgInQ;
volatile QueueHandle_t msgOutQ;

SemaphoreHandle_t keyArrayMutex;
SemaphoreHandle_t RX_MessageMutex;
SemaphoreHandle_t CAN_TX_Semaphore;
SemaphoreHandle_t sound_tableMutex;

SemaphoreHandle_t sampleBufferSemaphore;

uint8_t sampleBuffer0[SAMPLE_BUFFER_SIZE];
uint8_t sampleBuffer1[SAMPLE_BUFFER_SIZE];
volatile bool writeBuffer1 = false;

uint32_t local_timestep [12] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};
volatile uint16_t global_keyArray_concated;

Key sound_table = {};

volatile bool outBits[8] = {false,false,false,true,true,true,true};


void setPinDirections(){
  pinMode(RA0_PIN, OUTPUT);
  pinMode(RA1_PIN, OUTPUT);
  pinMode(RA2_PIN, OUTPUT);
  pinMode(REN_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(OUTL_PIN, OUTPUT);
  pinMode(OUTR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(C0_PIN, INPUT);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  pinMode(C3_PIN, INPUT);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);
}
