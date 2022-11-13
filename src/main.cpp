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

std::string msg = "The quick black dog jumped over the lazy brown fox.";

static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

void startTask1(void *param){
    int msg_lgth = msg.length();

    while(true){
        for (size_t i = 0; i < msg_lgth; i++){
            std::cout << msg.at(i);
        }
        std::cout << std::endl;
        vTaskDelay(1000 / MS);
    }
}

void startTask2(void *param){
    while(true){
        std::cout << "*";
        vTaskDelay(100 / MS);
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

    xTaskCreatePinnedToCore(startTask1,
                            "Task 1",
                            1024,
                            NULL,
                            1,
                            &task1,
                            app_cpu);

    xTaskCreatePinnedToCore(startTask2,
                            "Task 2",
                            1024,
                            NULL,
                            2,
                            &task2,
                            app_cpu);

    for (size_t i = 0; i < 3; i++){
        vTaskSuspend(task2);
        vTaskDelay(2000 / MS);
        vTaskResume(task2);
        vTaskDelay(2000 / MS);
    }

    if (task1 != NULL){
        vTaskDelete(task1);
        task1 = NULL;
    }
}