#include "sound.h"
#include "global_variables.h"
#include <map>

CentralOctaveLookUpTable centralOctaveLookUpTable;

uint16_t local_keyArray_concatenate;


void initialize_table(){
  centralOctaveLookUpTable.initializeTable(); 
}


std::map<uint8_t, uint8_t> Table;
const float octave_freq[8] = {0, 0.125, 0.25, 0.5, 1, 2, 4, 8};
Key local_sound_table [12] = {}; 


int16_t calculate_vout(){
  int32_t tmp_vout = 0;
  uint8_t count = 0;
  float step_size = 0;
  uint8_t key_index = 0;
  static uint32_t phaseAcc = 0;

  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  std::copy(sound_table, sound_table+12, local_sound_table);
  xSemaphoreGive(sound_tableMutex);

  // if(sawTooth_selected){
  //   for (int i = 0; i < 12 ; i++) {
  //       if (local_sound_table[i].key_index == 0) continue;
  //       key_index = local_sound_table[i].key_index - 1;
  //       tmp_vout += (stepSizes[key_index]*octave);
  //       count += 1;
  //   }
  //   if(count == 0) tmp_vout = 0; 
  //   else tmp_vout = tmp_vout/count;
  //   phaseAcc+=tmp_vout;
  //   return phaseAcc;
  
  // }else{
    for (int i = 0; i < 12 ; i++) {
        if (local_sound_table[i].key_index == 0) continue;
        key_index = local_sound_table[i].key_index - 1;
        step_size = octave_freq[local_sound_table[i].octave];
        local_timestep[key_index] += step_size;
        // Serial.println(local_timestep[key_index]);
        // Serial.println("G");
        // Serial.println(std::round(local_timestep[key_index]));
       
        if(local_timestep[key_index] >= tableSizes[key_index]) local_timestep[key_index] = 0;
        //Serial.println(std::round(local_timestep[key_index]));
        tmp_vout += centralOctaveLookUpTable.accessTable(key_index,std::round(local_timestep[key_index]));
        //Serial.println(tmp_vout);
        count += 1;
    }
    if(count == 0) tmp_vout = 0; 
    else tmp_vout = tmp_vout/count;
    return tmp_vout;
  // }

}


void write_to_double_buffer(void *pvParameters){
  const TickType_t xFrequency = 20/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  bool initial_write_flag;
  
  while(1){
  vTaskDelayUntil( &xLastWakeTime, xFrequency );
	xSemaphoreTake(sampleBufferSemaphore, portMAX_DELAY);
  initial_write_flag = writeBuffer1;
	for (uint32_t writeCtr = 0; writeCtr < SAMPLE_BUFFER_SIZE; writeCtr++) {
		int16_t Vout = calculate_vout(); 
    if(initial_write_flag == writeBuffer1){
      if(writeBuffer1){sampleBuffer1[writeCtr] = (Vout >> (8-knob3Rotation) )+ 128;}
      else{sampleBuffer0[writeCtr] = (Vout >> (8-knob3Rotation) )+ 128;}
    }
	} 
  }
}





