#include <Ticker.h>

#include <B31DGMonitor.h>

#include <driver/gpio.h>

#include "esp_heap_caps.h"

/**
 *
 * @author Fraser Holman
 * @date 02/04/2025
 *
 * @brief B31DG Assignment 2 - Part 2 RTOS Code
 *
 *
 * 
*/

// pin assignments
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
volatile byte ledState = LOW; // used to toggle an LED on/off

// B31DGCyclicExecutiveMonitor monitor(2565);
B31DGCyclicExecutiveMonitor monitor(1965);

// declared mutex's and semaphore's
SemaphoreHandle_t freqMutex1 = xSemaphoreCreateMutex();
SemaphoreHandle_t freqMutex2 = xSemaphoreCreateMutex();
SemaphoreHandle_t buttonMutex = xSemaphoreCreateBinary();

// function prototypes
void digitalSig1(void *pvParameters); // requirement 1
void digitalSig2(void *pvParameters); // requirement 2
void getF1(void *pvParameters); // requirement 3
void getF2(void *pvParameters); // requirement 4
unsigned long measureFrequency(gpio_num_t signal); // requirement 3/4
void monitorProgram(void *pvParameters); // requirement 5
void monitorLed(void *pvParameters); // requirement 6
void monitorButton(void *pvParameters); // requirement 7
void IRAM_ATTR toggleClick(void); // requirement 7

/**
 *
 * Setup method contains the system setup code
 *
 */
void setup() {
  Serial.begin(9600);

  // setting the LED pins as an output
  gpio_set_direction(LED1,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4,GPIO_MODE_OUTPUT);

  // setting the signal pins as an input
  gpio_set_direction(SIGNAL1,GPIO_MODE_INPUT);
  gpio_set_direction(SIGNAL2,GPIO_MODE_INPUT);

  // pin setup for buttons with pullup resistors
  pinMode(TOGGLE, INPUT_PULLUP); // enable button setup

  // attaching interrupt to the button pins
  attachInterrupt(digitalPinToInterrupt(TOGGLE), toggleClick, RISING); // interrupt on the rising edge for enable button

  // RTOS setup

  // monitoring tasks are given a very low priority to ensure it does not preempt an important task vital for this system
  xTaskCreate(monitorButton, "Task6", 2048, NULL, 0, NULL);
  xTaskCreate(monitorLed, "Task7", 2048, NULL, 0, NULL);

  // the main tasks for this RTOS system are given a higher priority
  xTaskCreate(digitalSig1, "Task1", 2048, NULL, 1, NULL);
  xTaskCreate(digitalSig2, "Task2", 2048, NULL, 1, NULL);
  xTaskCreate(getF1, "Task3", 2048, NULL, 1, NULL);
  xTaskCreate(getF2, "Task4", 2048, NULL, 1, NULL);
  xTaskCreate(monitorProgram, "Task5", 2048, NULL, 1, NULL);

  monitor.startMonitoring();
}

/**
 *
 * Empty Loop Function as required by RTOS
 *
 */
void loop() {}

/*
 * Method represents task 1 and these are its requirements:
 *
 * 1.	Output a digital signal. This should be HIGH for 250μs, then LOW for 50μs, then HIGH again for 300μs, then LOW again. 
 * You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts. 
*/
void digitalSig1(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(4); // method is only run once every 4ms

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    monitor.jobStarted(1);

    gpio_set_level(LED1, HIGH);
    delayMicroseconds(250);
    gpio_set_level(LED1, LOW);
    delayMicroseconds(50);
    gpio_set_level(LED1, HIGH);
    delayMicroseconds(300);
    gpio_set_level(LED1, LOW);
    
    monitor.jobEnded(1);
  }
}

/*
 * Method represents task 2 and these are its requirements:
 *
 * 2.	Output a second digital signal. This should be HIGH for 100μs, then LOW for 50μs, then HIGH again for 200μs, then LOW again. 
 * You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts.
*/
void digitalSig2(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(3); // task is only run once every 3ms

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    monitor.jobStarted(2);

    gpio_set_level(LED2, HIGH);
    delayMicroseconds(100);
    gpio_set_level(LED2, LOW);
    delayMicroseconds(50);
    gpio_set_level(LED2, HIGH);
    delayMicroseconds(200);
    gpio_set_level(LED2, LOW);

    monitor.jobEnded(2);
  }
}

/*
 * Method represents task 3 and these are its requirements: 
 *
 * 3.	Measure the frequency of a 3.3v square wave signal. 
 * The frequency of the wave signal in input will be in the range 666Hz to 1000Hz and the signal will be a standard square wave (50% duty cycle). 
 * Let’s call this frequency F1 and measure it in Hz. You should measure F1 by polling the signal. 
 * For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF1(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // task is only run once every 10ms

  unsigned long period = 0;

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    monitor.jobStarted(3);

    // frequency measurement was handled in a different function ro prevent repeated code
    period = measureFrequency(SIGNAL1);

    xSemaphoreTake(freqMutex1, portMAX_DELAY); // takes mutex to prevent shared resources

    // the ESP32 proved to record a few sporadic frequency values during operation. This cause the LED to flicker which is not wanted for this program
    // to mitigate for this the period value was checked for if it was too far outside the range given by the requirement and in this case it was not recorded
    // in this case any signal with a frequency lower than 606HZ was ignored
    if (period < 1650 && period > 0) F1 = 1000000 / period;

    // through testing it was discovered that if the signal generator is turned off during operation the measured Frequency value would be within the 100,000s
    // to mitigate for this issue it was decided that when this occured the frequency value would be set to 0HZ to allow the system to be more realistic
    if (F1 > 100000) F1= 0.0; 

    xSemaphoreGive(freqMutex1); // gives back mutex after writing to shared resource

    monitor.jobEnded(3);
  }
}

/*
 * Method represents task 4 and these are its requirements:
 * 
 * 4.	Measure the frequency of a second 3.3v square wave signal. 
 * The frequency of the wave signal in input will be in the range 833Hz to 1500Hz and the signal will be a standard square wave (50% duty cycle). 
 * Let’s call this frequency F2 and measure it in Hz. You should measure F2 by polling the signal. 
 * For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF2(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // task is only run once every 10ms

  unsigned long period = 0;

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    monitor.jobStarted(4);

    // frequency measurement was handled in a different function ro prevent repeated code
    period = measureFrequency(SIGNAL2);

    xSemaphoreTake(freqMutex2, portMAX_DELAY); // takes mutex to prevent shared resources

    // the ESP32 proved to record a few sporadic frequency values during operation. This cause the LED to flicker which is not wanted for this program
    // to mitigate for this the period value was checked for if it was too far outside the range given by the requirement and in this case it was not recorded
    // in this case any signal with a frequency lower than 606HZ was ignored
    if (period < 1650 && period > 0) F2 = 1000000 / period;

    // through testing it was discovered that if the signal generator is turned off during operation the measured Frequency value would be within the 100,000s
    // to mitigate for this issue it was decided that when this occured the frequency value would be set to 0HZ to allow the system to be more realistic 
    if (F2 > 100000) F2 = 0.0;

    xSemaphoreGive(freqMutex2); // gives back mutex after writing to shared resource
    
    monitor.jobEnded(4);
  }
}

/*
 * Method represents task 5 and these are its requirements:
 *
 * 5.	Call the monitor’s method doWork().
 */
void monitorProgram(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(5);

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    monitor.jobStarted(5);

    monitor.doWork();
    
    monitor.jobEnded(5);
  }
}

/*
 * Method manages button presses as outlined by this requirement. The logic behind handling the button input is done within this method to ensure
 * the ISR does not waste too much CPU time which would cause violations. The task is given low priority to ensure it does not affect the main cycle
 *
 * 7.	Monitor a pushbutton. Toggle the state of a second LED and call the monitor’s method doWork() whenever the pushbutton is pressed.
 */
void monitorButton(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(120); // task is only run once every 120ms

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    if (xSemaphoreTake(buttonMutex, portMAX_DELAY) == pdTRUE) { // task will wait until it is signalled by the ISR
      monitor.doWork();
      ledState = !ledState;
      gpio_set_level(LED4, (int) ledState);
    }
  }
}

/*
 * Method manages changing the state of the LED depending on frequency readings
 * To protect shared resources a Mutex is used to ensure this task is not reading the variable at the same time it is written
 * The task is given a very low priority to ensure it does not affect the main RTOS operation time
 *
 * Coursework requirement:
 * 6.	Use a LED to indicate whether the sum of the two frequencies F1 and F2 is greater than 1500, i.e. when F1+F2 > 1500. 
 */
void monitorLed(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(120); // task is only run once every 120ms

  while (1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    xSemaphoreTake(freqMutex1, portMAX_DELAY);

    float tempF1 = F1; // frequency is saved to a temporary variable to prevent deadlock / wasting cpu time when trying to access multiple mutex's

    xSemaphoreGive(freqMutex1);
    
    xSemaphoreTake(freqMutex2, portMAX_DELAY);

    float tempF2 = F2; // simialrly here each frequency is read in turn to prevent deadlock / wasting cpu time while trying to take two different mutex's

    xSemaphoreGive(freqMutex2);

    if (tempF1 + tempF2 > 1500) {
      gpio_set_level(LED3, HIGH);
    }
    else {
      gpio_set_level(LED3, LOW);

    }
  }
}

/*
 * Method used to measure frequency
 *
 * @param signal represents which pin to read frequency from
 */
unsigned long measureFrequency(gpio_num_t signal) {
  unsigned long period = 0;

  unsigned long start;
  unsigned long startOfPulse;

  int state;

  // retrieves state of input
  state = gpio_get_level(signal);

  // starts a timer to be used to timeout the system if needed
  startOfPulse = micros();

  // function waits for the signal input to change level or to timeout if too much time is used
  while (gpio_get_level(signal) == state && micros() - startOfPulse < 750);

  // starts measuring the frequency of the signal
  start = micros();

  // function waits for the signal input to change level again or timeout
  while (gpio_get_level(signal) != state && micros() - startOfPulse < 1500);

  // uses the measured time to calculate the period of the frequency
  // this is multiplied by two as only half of the signal is measured for efficiency
  period = ( micros() - start ) * 2;

  return period;
}

/**
 * Method to handle toggle button interrupt
 *
 * Method signals an RTOS task using a binary semaphore to handle the interrupt to prevent the ISRs using up too much CPU time
 */
void IRAM_ATTR toggleClick() {
  xSemaphoreGiveFromISR(buttonMutex, NULL); // uses binary semaphore to signal monitor button task to run
}