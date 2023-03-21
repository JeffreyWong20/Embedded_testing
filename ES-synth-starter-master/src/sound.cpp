#include "sound.h"
#include "global_variables.h"
#include <map>

CentralOctaveLookUpTable centralOctaveLookUpTable;

uint16_t local_keyArray_concatenate;


void initialize_table(){
  centralOctaveLookUpTable.initializeTable(); 
}




#include <cmath>
#include <vector>

// Generate a vector of harmonics for a violin sound
std::vector<double> generateViolinHarmonics(double baseFrequency, int numHarmonics) {
    std::vector<double> harmonics(numHarmonics);
    for (int i = 0; i < numHarmonics; ++i) {
        harmonics[i] = std::sin((i + 1) * baseFrequency * 2.0 * M_PI);
    }
    return harmonics;
}

// Generate a violin sound wave for the given duration and sample rate
std::vector<double> generateViolinWave(double duration, double sampleRate) {
    const int numSamples = duration * sampleRate;

    // Generate harmonics for violin sound
    const double baseFrequency = 440.0; // A4
    const int numHarmonics = 20;
    std::vector<double> harmonics = generateViolinHarmonics(baseFrequency, numHarmonics);

    // Generate waveform by adding up harmonics
    std::vector<double> waveform(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        double sample = 0.0;
        for (int j = 0; j < numHarmonics; ++j) {
            sample += harmonics[j] * std::sin((j + 1) * baseFrequency * 2.0 * M_PI * i / sampleRate);
        }
        waveform[i] = sample;
    }

    return waveform;
}







std::map<uint8_t, uint8_t> Table;
const float octave_freq[8] = {0, 0.125, 0.25, 0.5, 1, 2, 4, 8};
Key local_sound_table [12] = {}; 



uint32_t AtypeOfSound(uint32_t phaseAcc){
  int32_t tmp_vout = 0;
  uint8_t count = 0;
  float step_size = 1;
  uint8_t key_index = 0;
  uint8_t sine_index = 0;

  for (int i = 0; i < 12 ; i++) {
      if (local_sound_table[i].key_index == 0) continue;
      key_index = local_sound_table[i].key_index - 1;

      local_timestep[sine_index] += step_size;
      if(local_timestep[sine_index] >= tableSizes[sine_index]) local_timestep[sine_index] = 0;

      tmp_vout += (stepSizes[key_index] * (1+ 0.1*centralOctaveLookUpTable.accessTable(sine_index,std::round(local_timestep[sine_index])))); // this sine wave of that octave);
      count += 1;
    }
    if(count == 0) tmp_vout = 0; 
    else tmp_vout = tmp_vout/count;
    phaseAcc+=tmp_vout;
    return (phaseAcc >> 24) - 128;
}

int16_t calculate_vout(){
  int32_t tmp_vout = 0;
  uint8_t count = 0;
  float step_size = 0;
  uint8_t key_index = 0;
  static uint32_t phaseAcc = 0;
  static uint32_t count10 = 0;

  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  std::copy(sound_table, sound_table+12, local_sound_table);
  xSemaphoreGive(sound_tableMutex);


  /// Disable sound;
  if((!main_speaker) || (mute)) return tmp_vout; 

  if(sawTooth_selected){
    // generateViolinWave(10, 22000)[count10++];
     //return AtypeOfSound(phaseAcc);

    for (int i = 0; i < 12 ; i++) {
        if (local_sound_table[i].key_index == 0) continue;
        key_index = local_sound_table[i].key_index - 1;
        tmp_vout += (stepSizes[key_index]);
        count += 1;
    }
    if(count == 0) tmp_vout = 0; 
    else tmp_vout = tmp_vout/count;
    phaseAcc+=tmp_vout;
    return (phaseAcc >> 24) - 128;

  }else{

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
  }

}


void write_to_double_buffer(void *pvParameters){
  const TickType_t xFrequency = 20/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  bool initial_write_flag;
  
  while(1){
    #ifndef TEST_BUFFER
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    #endif


    #ifdef TEST_BUFFER
  
      /// all key is pressed
      for (int m = 0; m < 12; m++){
          sound_table[m].octave = 3;
          sound_table[m].key_index = m;
      }
          
      #ifdef SAWTooth_selected
        sawTooth_selected = true;
      #endif
      main_speaker = true;
      mute = false;

    #endif

    xSemaphoreTake(sampleBufferSemaphore, portMAX_DELAY);
    initial_write_flag = writeBuffer1;

    for (uint32_t writeCtr = 0; writeCtr < SAMPLE_BUFFER_SIZE; writeCtr++) {
      int16_t Vout = calculate_vout(); 
      if(initial_write_flag == writeBuffer1){
        if(writeBuffer1){sampleBuffer1[writeCtr] = (Vout >> (8-knob3Rotation) )+ 128;}
        else{sampleBuffer0[writeCtr] = (Vout >> (8-knob3Rotation) )+ 128;}
      }
    } 
    #ifdef TEST_BUFFER
    break;
    #endif
  }
}





