#include "global_variables.h"
#include "knob.cpp"
#include "sine.cpp"

#ifndef SCAN_H
#define SCAN_H

// void scanKeysTask(void *pvParameters);
void write_to_double_buffer(void *pvParameters);
void initialize_table();
int16_t calculate_vout();

#endif