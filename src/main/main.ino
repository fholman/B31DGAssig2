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
#define LED1 4
#define LED2 16
#define LED3 0 // this will be changed to below just using this for now for debugging
#define LED4 2 // this will be changed to below just using this for now for debugging
#define SIGNAL1 18
#define SIGNAL2 17
//#define TOGGLE 23
#define TOGGLE 22

#define DEBUG 0 // defining debug mode, 1 = slow down time, 0 = real time

#if DEBUG // conditional compilation for debug mode
#define FACTOR 1000 // slows down the waveform
#else
#define FACTOR 1 // waveform occurs in real time
#endif

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
void digitalSig1(void);
void digitalSig2(void);
void getF1(void);
void getF2(void);
void monitorButton(void);
void monitorProgram(void);
void IRAM_ATTR toggleClick(void);

/**
 *
 * Setup method contains the system setup code
 *
 */
void setup() {
  Serial.begin(115200);

  // pin setup for LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(SIGNAL1, INPUT);
  pinMode(SIGNAL2, INPUT);

  // pin setup for buttons with pullup resistors
  pinMode(TOGGLE, INPUT_PULLUP); // enable button setup

  // attaching interrupt to the button pins
  attachInterrupt(digitalPinToInterrupt(TOGGLE), toggleClick, RISING); // interrupt on the rising edge for enable button

  cycle.attach_ms(framePeriod, frame);

  monitor.startMonitoring();
}

/**
 *
 * Loop contains logic to control the DATA and SYNC pulses
 *
 */
void loop() {
  // digitalSig1();
  // digitalSig2();
  // getF1();
  // getF2();

  // unsigned long bT = micros();
  // for (int i=0; i<1000; i++) {
  //   monitorProgram();
  // }
  // unsigned long timeItTook = micros()-bT;
  // Serial.print("Duration SerialOutput Job = ");
  // Serial.println(timeItTook / 1000);

}

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
1.	Output a digital signal. This should be HIGH for 250μs, then LOW for 50μs, then HIGH again for 300μs, then LOW again. 
You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts. 
*/
void digitalSig1() {
  monitor.jobStarted(1);
  digitalWrite(LED1, HIGH);
  delayMicroseconds(250 * FACTOR);
  digitalWrite(LED1, LOW);
  delayMicroseconds(50 * FACTOR);
  digitalWrite(LED1, HIGH);
  delayMicroseconds(300 * FACTOR);
  digitalWrite(LED1, LOW);
  monitor.jobEnded(1);
}

/*
2.	Output a second digital signal. This should be HIGH for 100μs, then LOW for 50μs, then HIGH again for 200μs, then LOW again. 
You should use spin wait loops (e.g. using calls to standard delay()) to implement the short time intervals you need. Do not use interrupts.
*/
void digitalSig2() {
  monitor.jobStarted(2);
  digitalWrite(LED2, HIGH);
  delayMicroseconds(100 * FACTOR);
  digitalWrite(LED2, LOW);
  delayMicroseconds(50 * FACTOR);
  digitalWrite(LED2, HIGH);
  delayMicroseconds(200 * FACTOR);
  digitalWrite(LED2, LOW);
  monitor.jobEnded(2);
}

/*
3.	Measure the frequency of a 3.3v square wave signal. 
The frequency of the wave signal in input will be in the range 666Hz to 1000Hz and the signal will be a standard square wave (50% duty cycle). 
Let’s call this frequency F1 and measure it in Hz. You should measure F1 by polling the signal. 
For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF1() {
  monitor.jobStarted(3);
  unsigned long period = pulseIn(SIGNAL1, HIGH) * 2;
  if (period > 0) {
    F1 = 1000000.0 / period;
    //Serial.println(F1);

    if (F1 + F2 > 1500) {
      digitalWrite(LED3, HIGH);
    }
    else {
      digitalWrite(LED3, LOW);
    }
  }
  monitor.jobEnded(3);
}

/*
4.	Measure the frequency of a second 3.3v square wave signal. 
The frequency of the wave signal in input will be in the range 833Hz to 1500Hz and the signal will be a standard square wave (50% duty cycle). 
Let’s call this frequency F2 and measure it in Hz. You should measure F2 by polling the signal. 
For this exercise, do not use interrupts (which would be the more efficient method).
*/
void getF2() {
  monitor.jobStarted(4);
  unsigned long period = pulseIn(SIGNAL2, HIGH) * 2;
  if (period > 0) {
    F2 = 1000000.0 / period;
    // Serial.println(F2);

    if (F1 + F2 > 1500) {
      digitalWrite(LED3, HIGH);
    }
    else {
      digitalWrite(LED3, LOW);
    }
  }
  monitor.jobEnded(4);
}

void monitorProgram() {
  monitor.jobStarted(5);
  monitor.doWork();
  monitor.jobEnded(5);
}

void monitorButton() {
  if (pressed != prevButtonState) {
    monitor.doWork();
  }

  digitalWrite(LED4, ledState);
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
    prevButtonState = pressed;
    ledState = !ledState;
    pressed = !pressed;
  }
}
