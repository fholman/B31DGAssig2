#include <Ticker.h>

#include <B31DGMonitor.h>

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

B31DGCyclicExecutiveMonitor monitor(1990);
Ticker cycle;
float framePeriod = 2;
unsigned long frameCounter = 0;

// function prototypes
void digitalSig1(void);                            // requirement 1
void digitalSig2(void);                            // requirement 2
void getF1(void);                                  // requirement 3
void getF2(void);                                  // requirement 4
unsigned long measureFrequency(gpio_num_t signal); // requirement 3/4
void monitorProgram(void);                         // requirement 5
void monitorLED(void);                             // requirement 6
void monitorButton(void);                          // requirement 7
void IRAM_ATTR toggleClick(void);                  // requirement 7


/**
 *
 * Setup method contains the system setup code
 *
 */
void setup() {
  Serial.begin(9600);

  // setting pin mode of LEDs as an output
  gpio_set_direction(LED1,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3,GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4,GPIO_MODE_OUTPUT);

  // setting signal generator pins as an input
  gpio_set_direction(SIGNAL1,GPIO_MODE_INPUT);
  gpio_set_direction(SIGNAL2,GPIO_MODE_INPUT);

  // pin setup for buttons with pullup resistors
  pinMode(TOGGLE, INPUT_PULLUP); // enable button setup

  // attaching interrupt to the button pins
  attachInterrupt(digitalPinToInterrupt(TOGGLE), toggleClick, RISING); // interrupt on the rising edge for enable button

  // attaching frame method to ticker object so that it executes at regular intervals
  cycle.attach_ms(framePeriod, frame);

  // start monitoring the cyclic executive scheduler
  monitor.startMonitoring();
}

/*
 *
 * Main loop is left empty as functions are called using a ticker object
 *
 */
void loop() {}

/*
 *
 * Frame to be executed at regular intervals
 *
 * Each frame was split into 2ms intervals and each full cycle was therefore 30 frames or 60ms this is explored further in the report and cyclic executive plan
 *
 */
void frame() {
  unsigned int slot = frameCounter % 30;

  switch (slot) {
    case 0:  digitalSig1(); digitalSig2();                   monitorProgram(); monitorButton(); break;
    case 1:                                getF1();                                             break;
    case 2:  digitalSig1(); digitalSig2();                                                      break;
    case 3:                 digitalSig2();          getF2();                                    break;
    case 4:  digitalSig1();                                  monitorProgram();                  break;
    case 5:                 digitalSig2();                   monitorProgram();                  break;
    case 6:  digitalSig1(); digitalSig2();                                     monitorButton(); break;
    case 7:                                getF1();                                             break;
    case 8:  digitalSig1(); digitalSig2();                   monitorProgram(); monitorButton(); break;
    case 9:                                         getF2();                                    break;
    case 10: digitalSig1(); digitalSig2();                   monitorProgram();                  break;
    case 11:                digitalSig2();          getF2();                                    break;
    case 12: digitalSig1(); digitalSig2();                                     monitorButton(); break;
    case 13:                               getF1();                                             break;
    case 14: digitalSig1(); digitalSig2();                   monitorProgram();                  break;
    case 15:                digitalSig2();                   monitorProgram(); monitorButton(); break;
    case 16:                               getF1();                                             break;
    case 17: digitalSig1(); digitalSig2();                                     monitorButton(); break;
    case 18: digitalSig1();                         getF2();                                    break;
    case 19:                digitalSig2();                   monitorProgram();                  break;
    case 20: digitalSig1(); digitalSig2();                   monitorProgram();                  break;
    case 21:                digitalSig2(); getF1();                                             break;
    case 22: digitalSig1();                         getF2();                                    break;
    case 23:                digitalSig2();                   monitorProgram();                  break;
    case 24: digitalSig1(); digitalSig2();                                     monitorButton(); break;
    case 25:                                                 monitorProgram();                  break;
    case 26: digitalSig1(); digitalSig2();                                     monitorButton(); break;
    case 27:                               getF1();                                             break;
    case 28:                digitalSig2();          getF2();                                    break;
    case 29: digitalSig1(); digitalSig2();                   monitorProgram(); monitorButton(); break;
  }

  frameCounter++;
}

/*
 * Method represents task 1 and these are its requirements:
 *
 * 1.	Output a digital signal. This should be HIGH for 250μs, then LOW for 50μs, then HIGH again for 300μs, then LOW again. 
 * You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts. 
*/
void digitalSig1() {
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

/*
 * Method represents task 2 and these are its requirements:
 *
 * 2.	Output a second digital signal. This should be HIGH for 100μs, then LOW for 50μs, then HIGH again for 200μs, then LOW again. 
 * You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts.
*/
void digitalSig2() {
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

/*
 * Method represents task 3 and these are its requirements: 
 *
 * 3.	Measure the frequency of a 3.3v square wave signal. 
 * The frequency of the wave signal in input will be in the range 666Hz to 1000Hz and the signal will be a standard square wave (50% duty cycle). 
 * Let’s call this frequency F1 and measure it in Hz. You should measure F1 by polling the signal. 
 * For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF1() {
  monitor.jobStarted(3);

  // frequency measurement was handled in a different function ro prevent repeated code
  unsigned long period = measureFrequency(SIGNAL1);

  // the ESP32 proved to record a few sporadic frequency values during operation. This cause the LED to flicker which is not wanted for this program
  // to mitigate for this the period value was checked for if it was too far outside the range given by the requirement and in this case it was not recorded
  // in this case any signal with a frequency lower than 606HZ was ignored
  if (period < 1650 && period > 0) F1 = 1000000 / period;

  // through testing it was discovered that if the signal generator is turned off during operation the measured Frequency value would be within the 100,000s
  // to mitigate for this issue it was decided that when this occured the frequency value would be set to 0HZ to allow the system to be more realistic
  if (F1 > 100000) F1= 0.0;

  // LED monitoring was handled in a different function ro prevent repeated code
  monitorLED();

  monitor.jobEnded(3);
}

/*
 * Method represents task 4 and these are its requirements:
 * 
 * 4.	Measure the frequency of a second 3.3v square wave signal. 
 * The frequency of the wave signal in input will be in the range 833Hz to 1500Hz and the signal will be a standard square wave (50% duty cycle). 
 * Let’s call this frequency F2 and measure it in Hz. You should measure F2 by polling the signal. 
 * For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF2() {
  monitor.jobStarted(4);

  // frequency measurement was handled in a different function ro prevent repeated code
  unsigned long period = measureFrequency(SIGNAL2);

  // the ESP32 proved to record a few sporadic frequency values during operation. This cause the LED to flicker which is not wanted for this program
  // to mitigate for this the period value was checked for if it was too far outside the range given by the requirement and in this case it was not recorded
  // in this case any signal with a frequency lower than 606HZ was ignored
  if (period < 1650 && period > 0) F2 = 1000000 / period;

  // through testing it was discovered that if the signal generator is turned off during operation the measured Frequency value would be within the 100,000s
  // to mitigate for this issue it was decided that when this occured the frequency value would be set to 0HZ to allow the system to be more realistic
  if (F2 > 100000) F2 = 0.0;

  // LED monitoring was handled in a different function ro prevent repeated code
  monitorLED();
  
  monitor.jobEnded(4);
}

/*
 * Method represents task 5 and these are its requirements:
 *
 * 5.	Call the monitor’s method doWork().
 */
void monitorProgram() {
  monitor.jobStarted(5);
  monitor.doWork();
  monitor.jobEnded(5);
}

/*
 * Method manages button presses as outlined by this requirement. The logic behind handling the button input is done within this method to ensure
 * the ISR does not waste too much CPU time which would cause violations. This allows the button to be checked at control intervals outlined by the cyclic executive
 *
 * 7.	Monitor a pushbutton. Toggle the state of a second LED and call the monitor’s method doWork() whenever the pushbutton is pressed.
 */
void monitorButton() {
  if (pressed != prevButtonState) {
    monitor.doWork();
  }

  gpio_set_level(LED4, (int) ledState);
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

/*
 * Method manages changing the state of the LED depending on frequency readings. This method uses up an extremely little amount of time so is called after each frequency is measured
 *
 * Coursework requirement:
 * 6.	Use a LED to indicate whether the sum of the two frequencies F1 and F2 is greater than 1500, i.e. when F1+F2 > 1500. 
 */
void monitorLED() {
  if (F1 + F2 > 1500) {
    gpio_set_level(LED3, HIGH);
  }
  else {
    gpio_set_level(LED3, LOW);
  }
}

/**
 * Method to handle toggle button interrupt
 *
 * Method has very little logic to prevent using up too much CPU time
 */
void IRAM_ATTR toggleClick() {
  unsigned long currentTime = millis(); // keeps track of number of milliseconds that have passed 
  
  // ignore interrupts that happen within debounceDelay
  if (currentTime - lastDebounceTime > debounceDelay) {
    lastDebounceTime = currentTime; // Update last debounce time
    prevButtonState = pressed;
    ledState = !ledState;
    pressed = !pressed;
  }
}
