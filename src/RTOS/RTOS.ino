#include <Ticker.h>

#include <B31DGMonitor.h>

#include <driver/gpio.h>

/**
 *
 * @author Fraser Holman
 * @date 10/03/2025
 *
 * @brief B31DG Assignment 2 - Part 1 Cyclic Execution Code
 *
 *
 * 
*/

// pin assignments
// #define LED1 4
// #define LED2 16
// #define LED3 15
// #define LED4 2
#define LED1 GPIO_NUM_4
#define LED2 GPIO_NUM_16
#define LED3 GPIO_NUM_0
#define LED4 GPIO_NUM_2

#define SIGNAL1 GPIO_NUM_18
#define SIGNAL2 GPIO_NUM_17
#define TOGGLE 22


float F1 = 0.0;
float F2 = 0.0;

const uint8_t debounceDelay = 50; // debounce time in milliseconds used for button presses
volatile unsigned long lastDebounceTime = 0; // store last press time for enable button
bool pressed = false;
bool prevButtonState = false;
volatile byte ledState = LOW; // used to toggle an LED on/off
bool monitoring = false;

SemaphoreHandle_t freqMutex1 = xSemaphoreCreateMutex();
SemaphoreHandle_t freqMutex2 = xSemaphoreCreateMutex();
SemaphoreHandle_t buttonMutex = xSemaphoreCreateBinary();
SemaphoreHandle_t monitorMutex = xSemaphoreCreateMutex();

// B31DGCyclicExecutiveMonitor monitor(50);
B31DGCyclicExecutiveMonitor monitor(300);

unsigned long startOfProgram;

// function prototypes
void digitalSig1(void *pvParameters);
void digitalSig2(void *pvParameters);
void getF1(void *pvParameters);
void getF2(void *pvParameters);
void monitorButton(void *pvParameters);
void monitorProgram(void *pvParameters);
void IRAM_ATTR toggleClick(void);
void vTaskDelayMS(int ms);

/**
 *
 * Setup method contains the system setup code
 *
 */
void setup() {
  Serial.begin(9600);

  gpio_set_direction(LED1,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4,GPIO_MODE_OUTPUT);

  gpio_set_direction(SIGNAL1,GPIO_MODE_INPUT);
  gpio_set_direction(SIGNAL2,GPIO_MODE_INPUT);

  // pin setup for buttons with pullup resistors
  pinMode(TOGGLE, INPUT_PULLUP); // enable button setup

  // attaching interrupt to the button pins
  attachInterrupt(digitalPinToInterrupt(TOGGLE), toggleClick, RISING); // interrupt on the rising edge for enable button

  startOfProgram = micros() + 40;

  // RTOS setup
  xTaskCreate(monitorButton, "Task6", 1024, NULL, 0, NULL);
  xTaskCreate(monitorLed, "Task7", 1024, NULL, 0, NULL);
  xTaskCreate(digitalSig1, "Task1", 2048, NULL, 2, NULL);
  xTaskCreate(digitalSig2, "Task2", 2048, NULL, 2, NULL);
  xTaskCreate(getF1, "Task3", 2048, NULL, 2, NULL);
  xTaskCreate(getF2, "Task4", 2048, NULL, 2, NULL);
  xTaskCreate(monitorProgram, "Task5", 2048, NULL, 2, NULL);

  monitor.startMonitoring();
}

/**
 *
 * Empty Loop Function
 *
 */
void loop() {}

/*
1.	Output a digital signal. This should be HIGH for 250μs, then LOW for 50μs, then HIGH again for 300μs, then LOW again. 
You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts. 
*/
void digitalSig1(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(4);

  uint16_t counter = 0;

  while (1) {
    counter++;

    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobStarted(1);
    // // xSemaphoreGive(monitorMutex);

    gpio_set_level(LED1, HIGH);
    delayMicroseconds(250);
    gpio_set_level(LED1, LOW);
    delayMicroseconds(50);
    gpio_set_level(LED1, HIGH);
    delayMicroseconds(300);
    gpio_set_level(LED1, LOW);
    
    // // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobEnded(1);
    // // xSemaphoreGive(monitorMutex);

    // new delay method in relation to when start monitoring is activated
    // vTaskDelayMS((int)((startOfProgram + 4000 * counter - micros()) / 1000));

  }
}

/*
2.	Output a second digital signal. This should be HIGH for 100μs, then LOW for 50μs, then HIGH again for 200μs, then LOW again. 
You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts.
*/
void digitalSig2(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(3);

  uint16_t counter = 0;

  while (1) {
    counter++;
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobStarted(2);
    // xSemaphoreGive(monitorMutex);

    gpio_set_level(LED2, HIGH);
    delayMicroseconds(100);
    gpio_set_level(LED2, LOW);
    delayMicroseconds(50);
    gpio_set_level(LED2, HIGH);
    delayMicroseconds(200);
    gpio_set_level(LED2, LOW);

    // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobEnded(2);
    // xSemaphoreGive(monitorMutex);

    // vTaskDelayMS((int)((startOfProgram + 3000 * counter - micros()) / 1000));
  }
}

/*
3.	Measure the frequency of a 3.3v square wave signal. 
The frequency of the wave signal in input will be in the range 666Hz to 1000Hz and the signal will be a standard square wave (50% duty cycle). 
Let’s call this frequency F1 and measure it in Hz. You should measure F1 by polling the signal. 
For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF1(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10);

  unsigned long period = 0;

  unsigned long start;
  unsigned long startOfPulse;

  int state;

  uint16_t counter = 0;

  while (1) {
    counter++;

    monitor.jobStarted(3);

    xSemaphoreTake(freqMutex1, portMAX_DELAY);

    state = gpio_get_level(SIGNAL1);

    startOfPulse = micros();

    while (gpio_get_level(SIGNAL1) == state && micros() - startOfPulse < 750);

    start = micros();

    while (gpio_get_level(SIGNAL1) != state && micros() - startOfPulse < 1500);

    period = ( micros() - start ) * 2;

    // if (period < 1700 && period > 900) {
    //   F1 = 1000000 / period;
    // }

    if (period > 0) {
      F1 = 1000000 / period;
    }

    xSemaphoreGive(freqMutex1);

    monitor.jobEnded(3);

    vTaskDelayMS((int)((startOfProgram + 10000 * counter - micros()) / 1000));
  }
}

/*
4.	Measure the frequency of a second 3.3v square wave signal. 
The frequency of the wave signal in input will be in the range 833Hz to 1500Hz and the signal will be a standard square wave (50% duty cycle). 
Let’s call this frequency F2 and measure it in Hz. You should measure F2 by polling the signal. 
For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF2(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10);

  unsigned long period = 0;

  unsigned long start;
  unsigned long startOfPulse;

  int state;

  uint16_t counter = 0;

  while (1) {
    counter++;

    monitor.jobStarted(4);

    xSemaphoreTake(freqMutex2, portMAX_DELAY);

    state = gpio_get_level(SIGNAL2);

    startOfPulse = micros();

    while (gpio_get_level(SIGNAL2) == state && micros() - startOfPulse < 600);

    start = micros();

    while (gpio_get_level(SIGNAL2) != state && micros() - startOfPulse < 1200);

    period = ( micros() - start ) * 2;

    // if (period < 1500 && period > 600) {
    //   F2 = 1000000 / period;
    // }

    if (period > 0) {
      F2 = 1000000 / period;
    }

    xSemaphoreGive(freqMutex2);
    
    monitor.jobEnded(4);

    vTaskDelayMS((int)((startOfProgram + 10000 * counter - micros()) / 1000));
  }
}

void monitorProgram(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(5);

  uint16_t counter = 0;

  while (1) {
    counter++;
    // vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobStarted(5);
    // xSemaphoreGive(monitorMutex);

    monitor.doWork();
    
    // xSemaphoreTake(monitorMutex, portMAX_DELAY);
    monitor.jobEnded(5);
    // xSemaphoreGive(monitorMutex);

    //vTaskDelayUntil(&xLastWakeTime, xFrequency);

    vTaskDelayMS((int)((startOfProgram + 5000 * counter - micros()) / 1000));

  }
}

void monitorButton(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(100);

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    if (xSemaphoreTake(buttonMutex, portMAX_DELAY) == pdTRUE) {
      monitor.doWork();
      ledState = !ledState;
      gpio_set_level(LED4, (int) ledState);
    }
  }
}

void monitorLed(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(100);

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    xSemaphoreTake(freqMutex1, portMAX_DELAY);

    float tempF1 = F1;
    // Serial.println(tempF1);

    xSemaphoreGive(freqMutex1);
    
    xSemaphoreTake(freqMutex2, portMAX_DELAY);

    float tempF2 = F2;
    //Serial.println(tempF2);

    xSemaphoreGive(freqMutex2);

    if (tempF1 + tempF2 > 1500) {
      gpio_set_level(LED3, HIGH);
    }
    else {
      gpio_set_level(LED3, LOW);

    }
  }
}

void vTaskDelayMS(int ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}


/**
 *
 * Method to handle toggle button interrupt
 *
 */
void IRAM_ATTR toggleClick() {
  unsigned long currentTime = millis(); // keeps track of number of milliseconds that have passed 
  
  // ignore interrupts that happen within debounceDelay
  if (currentTime - lastDebounceTime > debounceDelay) {

    lastDebounceTime = currentTime; // Update last debounce time

    xSemaphoreGiveFromISR(buttonMutex, NULL);
    
  }
}
