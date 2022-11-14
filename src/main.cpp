#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace std;

#define MS portTICK_PERIOD_MS

#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

static TaskHandle_t task1 = NULL;

void testTask(void *param){
    int a = 1;
    int b[100]; //This is 400 bytes and initially exceeds the 1024 allocated to the task (768 + 400).
    while(true){
        for (size_t i = 0; i < 100; i++){
            b[i] = a + 1;
        }
        std::cout << b[0] << std::endl;
        vTaskDelay(1000 / MS);
    }
}

extern "C" void app_main();

void app_main() {
    std::cout << std::endl;
    std::cout << "---FreeRTOS Demo---" << std::endl;
    std::cout << std::endl;
    std::cout << "Setup and loop task running on core: ";
    std::cout << xPortGetCoreID();
    std::cout << " with priority ";
    std::cout << uxTaskPriorityGet(NULL) << std::endl;

    xTaskCreatePinnedToCore(testTask,
                            "Task 1",
                            1024,
                            NULL,
                            1,
                            &task1,
                            app_cpu);
}