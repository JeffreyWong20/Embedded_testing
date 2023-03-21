#include"joystick.h"

// int joystick1 = 512, joystick2 = 512;  
// int32_t joystickx_out;
// int32_t previous_sawTooth_selected;

/// @brief 
/// JoystickTask 
/// This is a seperate task because analogRead take too much time.
/// @param pvParameters 
void joystickTask(void *pvParameters){
  int32_t joystickx_out;
  int32_t previous_sawTooth_selected;

  const TickType_t xFrequency = 10/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime= xTaskGetTickCount();
  while(1) {
    # ifndef TEST_JOYSTICK
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
    # endif
    joystickx_out = 512 - analogRead(JOYX_PIN);
    if(joystickx_out > 200 | joystickx_out < -200){
        if(previous_sawTooth_selected){
            previous_sawTooth_selected = 0;
            sawTooth_selected = !sawTooth_selected;
          }
        }else{
            previous_sawTooth_selected = 1;
        }
    //Serial.print(sawTooth_selected);
      # ifdef TEST_JOYSTICK
      break;
      #endif
  }
  
}


// void joystickTask(void *pvParameters){
//     int32_t joystickx_out;
//     int32_t previous_sawTooth_selected;
//     bool previous_direction;
//   const TickType_t xFrequency = 1000/portTICK_PERIOD_MS;
//   TickType_t xLastWakeTime= xTaskGetTickCount();
//   while(1) {
//     vTaskDelayUntil(&xLastWakeTime, xFrequency);
//     joystickx_out = 512 - analogRead(JOYX_PIN);
//     if(joystickx_out > 200){
//         if(!previous_direction){
//             previous_direction = 0;
//             sawTooth_val += 1;
//             }
//         }
//      else if (joystickx_out < 200){

//}    
//     //Serial.print(sawTooth_selected);
//   }
// }