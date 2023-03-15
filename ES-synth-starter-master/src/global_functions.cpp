#include "global_functions.h"

///  append or delete the given [octave,key_index] from the global sound_map
void modified_soundMap(uint8_t local_octave, uint8_t key_index, bool append){

  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  
  if (append) {
    //pressed
    for (int g = 0; g < 12 ;g++){
      // Serial.print("pressed the sound table content");
      // Serial.print(sound_table[g].key_index);
      if (sound_table[g].key_index == 0){
        sound_table[g].key_index = key_index;
        sound_table[g].octave = local_octave;
        // Serial.print("pressed end");
        // Serial.println(sound_table[g].key_index);
        break;

      }
    }

  }else{
    for (int g = 0; g < 12 ;g++){
      // Serial.print("released the sound table content");
      // Serial.print(sound_table[g].key_index);
      if (sound_table[g].key_index == key_index){
        sound_table[g].key_index = 0;
        sound_table[g].octave = 0;
        // Serial.print("released end");
        // Serial.println(sound_table[g].key_index);
        break;
      }
    }
  }
  xSemaphoreGive(sound_tableMutex);
}
