#include "sound.h"
#include "global_variables.h"
#include <map>

CentralOctaveLookUpTable centralOctaveLookUpTable;

uint16_t local_keyArray_concatenate;


void initialize_table(){
  centralOctaveLookUpTable.initializeTable(); 
}


std::map<uint8_t, uint8_t> Table;

const uint8_t octave_freq[8] = {0, 1, 2, 4, 8, 16, 32, 64};
std::map<uint8_t, std::vector<uint16_t> > local_sound_table; 

int16_t calculate_vout(){
  int32_t tmp_vout = 0;
  uint8_t count = 0;
  uint16_t step_size = 0;
  uint32_t tmp_key_index = 0;
  std::vector<uint16_t> local_pressed_keys;
  // local_keyArray_concatenate = __atomic_load_n(&global_keyArray_concated, __ATOMIC_RELAXED);
  
  // TODO: find a better method than copying
  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  local_sound_table = sound_table;
  xSemaphoreGive(sound_tableMutex);

 // TODO: find a better method than copying
  for (auto it = local_sound_table.begin(); it != local_sound_table.end(); ++it) {
    local_pressed_keys = it->second;
    if (!local_pressed_keys.empty()){
      step_size = octave_freq[it->first];
      for (int vec_index = 0; vec_index < local_pressed_keys.size(); vec_index ++){
          tmp_key_index = local_pressed_keys[vec_index];
          local_timestep[tmp_key_index] += step_size;
          if(local_timestep[tmp_key_index] >= tableSizes[tmp_key_index]) local_timestep[tmp_key_index] = 0;
          tmp_vout += centralOctaveLookUpTable.accessTable(tmp_key_index,local_timestep[tmp_key_index]);
          count+=1;
      }
    }
  }
  
  if(count == 0)
    tmp_vout = 0; 
  else 
    tmp_vout = tmp_vout/count;
  // tmp_vout = tmp_vout >> (8 - knob3Rotation);
  // Serial.print("Vout ");
  // Serial.println(tmp_vout);
  return tmp_vout;
}


void write_to_double_buffer(void *pvParameters){
  const TickType_t xFrequency = 50/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  bool initial_write_flag;
  
  while(1){
  vTaskDelayUntil( &xLastWakeTime, xFrequency );
  // // digitalWrite(LED_BUILTIN, LOW);
	xSemaphoreTake(sampleBufferSemaphore, portMAX_DELAY);
  // digitalWrite(LED_BUILTIN, HIGH);
  // Serial.print("enter");
  initial_write_flag = writeBuffer1;
	for (uint32_t writeCtr = 0; writeCtr < SAMPLE_BUFFER_SIZE; writeCtr++) {
		int16_t Vout = calculate_vout(); 
    if(initial_write_flag == writeBuffer1){
      if(writeBuffer1){sampleBuffer1[writeCtr] = Vout + 128;}
      else{sampleBuffer0[writeCtr] = Vout + 128;}
    }
    
	} 
  // Serial.println("end");
  }
}
















// void write_to_double_buffer(void *pvParameters){
//   // Serial.println("write to double begin");
//   // const TickType_t xFrequency = 70/portTICK_PERIOD_MS;
//   // TickType_t xLastWakeTime = xTaskGetTickCount();
  
//   while(1){
//   // vTaskDelayUntil( &xLastWakeTime, xFrequency );
//   // // digitalWrite(LED_BUILTIN, LOW);
// 	xSemaphoreTake(sampleBufferSemaphore, portMAX_DELAY);
//   // digitalWrite(LED_BUILTIN, HIGH);
//   Serial.print("enter");
// 	for (uint32_t writeCtr = 0; writeCtr < SAMPLE_BUFFER_SIZE; writeCtr++) {
// 		int16_t Vout = calculate_vout(); 
// 		if(writeBuffer1){
//       sampleBuffer1[writeCtr] = Vout + 128;
//       // Serial.println(" ");
//       Serial.print("A");
//       // Serial.println( sampleBuffer1[writeCtr]);
//     }
// 		else{
// 			sampleBuffer0[writeCtr] = Vout + 128;
//       Serial.print("B");
//       // Serial.println(sampleBuffer0[writeCtr]);
//     }
// 	} 
//   Serial.println("end");
//   }
// }





// int16_t calculate_vout(){
  
//   int16_t tmp_vout = 0;
//   uint8_t count = 0;
//   local_keyArray_concatenate = __atomic_load_n(&global_keyArray_concated, __ATOMIC_RELAXED);
  
//   for(int o=0; o<4; o++){
//     if(Table[o] == 0) continue;
//     local_keyArray_concatenate = Table[o];
//     for(int i=0; i<12; i++){
//       if((local_keyArray_concatenate & 1) == 0){
//         local_timestep[i] += octave_freq[0];
//         if(local_timestep[i] >= tableSizes[i]) local_timestep[i] = 0;
//         tmp_vout += centralOctaveLookUpTable.accessTable(i,local_timestep[i]);
//         count+=1;
//       }
//       local_keyArray_concatenate = local_keyArray_concatenate >> 1;
//     };
//   }
  
//   if(count==0)
//     tmp_vout=0; 
//   else 
//     tmp_vout = tmp_vout/count;
//   return tmp_vout;
// }


