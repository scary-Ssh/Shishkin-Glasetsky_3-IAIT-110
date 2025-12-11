#include <Arduino_FreeRTOS.h>
#include <queue.h>

#define LDR_PIN A0
#define SERIAL_BAUD 9600

QueueHandle_t xLuxQueue;
const TickType_t xDelay200ms = pdMS_TO_TICKS(200);

const int cal_adc[] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1023};
const float cal_lux[] = {0.0, 10.0, 60.0, 150.0, 300.0, 500.0, 800.0, 1200.0, 1600.0, 2200.0, 2500.0};
const int cal_size = 11;

float interpolateLux(int adc) {

}

void TaskMeasureLuminosity(void *pvParameters) {
    float lux;
    for (;;) {
        int adc = analogRead(LDR_PIN);
        lux = interpolateLux(adc);
        xQueueSend(xLuxQueue, &lux, portMAX_DELAY);
        vTaskDelay(xDelay200ms);
    }
}

void TaskLogger(void *pvParameters) {
    float lux;
    for (;;) {
        if (xQueueReceive(xLuxQueue, &lux, portMAX_DELAY) == pdPASS) {
            Serial.print("Lux (RTOS): ");
            Serial.println(lux, 2);
        }
    }
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    xLuxQueue = xQueueCreate(5, sizeof(float));
    
    if (xLuxQueue != NULL) {
        xTaskCreate(TaskMeasureLuminosity, "Measure", 128, NULL, 2, NULL);
        xTaskCreate(TaskLogger, "Logger", 128, NULL, 1, NULL);
        vTaskStartScheduler();
    } else {
        Serial.println("Ошибка создания очереди!");
    }
}


void loop() {}
