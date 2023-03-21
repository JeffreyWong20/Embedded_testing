# include "scan.h"
# include "config.h"

uint8_t getHashedID(){
    uint32_t uid_w0, uid_w1, uid_w2;
    // Get the UID words
    uid_w0 = HAL_GetUIDw0();
    uid_w1 = HAL_GetUIDw1();
    uid_w2 = HAL_GetUIDw2();

    std::hash<uint32_t> hasher;
    std::uint8_t hash = hasher(uid_w0) % 100;

    return hash;
}

void configTask(void *pvParameters){
    const TickType_t xFrequency = 200 / portTICK_PERIOD_MS;
    // xFrequency initiation interval of task set to 50ms
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8_t localwesteastArray[2];
    uint8_t west, east;
    uint8_t position = 0;
    while (1) {
        # ifndef TEST_CONFIG
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        # endif

        # ifdef TEST_CONFIG
        configFlag = true;
        # endif

        // Serial.println("config flag");
        if (configFlag == true){
            // Serial.println("config flag is true");
            delayMicroseconds(100000);
            positionTable.clear();
            endConfigFlag = false;
            delayMicroseconds(100000);
            
            // wait until most West
            west = 1;
            // Serial.println("config flag1");
            while (west == 1){ // if I am not the most west keyboard, wait until I am the most west keyboard
                // Serial.println("Most west");
                xSemaphoreTake(westeastArrayMutex, portMAX_DELAY);
                std::copy(westeastArray,westeastArray+2,localwesteastArray);
                xSemaphoreGive(westeastArrayMutex);
                west = !(localwesteastArray[0] >> 3);
                east = !(localwesteastArray[1] >> 3);
                # ifdef TEST_CONFIG
                west = 0; // when in test mode, ignore the waiting time
                east = 0; // assume single keyboard
                # endif
            }
            delayMicroseconds(100000);
            // Serial.println("config flag2");
            position = positionTable.size()+1;
            positionTable[ownID] = position;
            sendMessage('L',ownID,position);
            if (east == 0){ // If I am the last one
                sendMessage('E',0,0);
                endConfigFlag = true;
            }
            else{
                delayMicroseconds(100000);
                outBits[6] = false;
            }
            // Serial.println("config flag3");
            while (endConfigFlag == false){
            }
            outBits[6] = true;
            delayMicroseconds(500000);
            xSemaphoreTake(westeastArrayMutex, portMAX_DELAY);
            std::copy(westeastArray,westeastArray+2,localwesteastArray);
            xSemaphoreGive(westeastArrayMutex);
            previous_west = !(localwesteastArray[0] >> 3);
            previous_east = !(localwesteastArray[1] >> 3);

            delayMicroseconds(100000);
            configFlag = false;
            delayMicroseconds(100000);
            octave = 4+(positionTable[ownID]-(positionTable.size()+1)/2);
            
            if (octave == 4){
                main_speaker = true;
            }
        }
        # ifdef TEST_CONFIG
        break;
        # endif
    }
} 

