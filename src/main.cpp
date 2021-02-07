#include "Arduino.h"
#include <Bounce.h>
#include <ResponsiveAnalogRead.h>

const auto debug = false;

// Set bits in ADC (advertied usable bits are 12 for LC, 13 for 3.2)
const int bitsADC = 13;
const int usableResolution = pow(2, bitsADC);
const int readSmoothing = 8;

// the MIDI channel number to send messages
const int channel = 1;

// Note number of noteOn/noteOff for button
const int noteNum = 44;

// Setup led pin
const int ledPin = 13; // Used to indicate if volume is muted

// Setup button
const int buttonPin = 1;
Bounce pushbutton = Bounce(buttonPin, 50); // 10 ms debounce

// Setup potentiometer
enum POTDIRECTION { NORMAL, REVERSE };
int potDirection{NORMAL};
const int potPin = A0;
int potCCNum = 7;
ResponsiveAnalogRead volumePot(potPin, true);

// Transmit midi cc mode
// 14 bit or "normal" 7 bit midi
enum MODES { MODE_14BIT, MODE_NORMAL };
int mode{MODE_NORMAL};

// store previously sent values, to detect changes
bool ledState = LOW;
bool muteState = false;

inline void fourteen_bit() {
  // Read pot
  int potVal = volumePot.getValue();

  // Shift bits so that it becomes 14 bit
  potVal = potVal << (14 - bitsADC);

  // Lower 7 bits of signal
  auto lowBitVal = potVal & 0x7F;
  lowBitVal = (potDirection == NORMAL) ? lowBitVal : 127 - lowBitVal;

  // Upper 7 bits of signal
  auto highBitVal = (potVal >> 7) & 0x7F;
  highBitVal = (potDirection == NORMAL) ? highBitVal : 127 - highBitVal;

  // Debugging
  if (debug) {
    Serial.println("Debug info for pot 1");

    Serial.print("Pot1 cc:");
    Serial.println(potCCNum);

    Serial.print("Usable bits: ");
    Serial.println(bitsADC);

    Serial.print("Usable resolution: ");
    Serial.println(usableResolution);

    Serial.print("Raw value: ");
    Serial.println(potVal);

    Serial.print("Low bit val: ");
    Serial.println(lowBitVal);

    Serial.print("High bit val: ");
    Serial.println(highBitVal);
  }

  // Send cc
  usbMIDI.sendControlChange(potCCNum + 32, lowBitVal, channel);
  usbMIDI.sendControlChange(potCCNum, highBitVal, channel);
}

inline void seven_bit() {
  // Read pot
  int potVal = volumePot.getValue();

  // Remove everything above 7 bits
  potVal = potVal >> (bitsADC - 7);

  // Choose direction
  potVal = (potDirection == NORMAL) ? potVal : 127 - potVal;

  // Send cc
  usbMIDI.sendControlChange(potCCNum, potVal, channel);
}

inline void readPot() {
  // Volume fader
  volumePot.update();

  if (volumePot.hasChanged()) {

    // 14 bit midi mode
    if (mode == MODE_14BIT) {
      fourteen_bit();
    }

    // Normal 7 bit mode
    if (mode == MODE_NORMAL) {
      seven_bit();
    }
  }
}

inline int readButton() {
  if (pushbutton.update()) {
    if (pushbutton.fallingEdge()) {
      return 1;
    } else {
      return 0;
    }
  }
}

inline void readMute() {
  if (readButton() == 1) {
    muteState = !muteState;
    ledState = muteState ? HIGH : LOW;
    usbMIDI.sendNoteOn(noteNum, 127, channel); // Note 44 = master mute
    usbMIDI.sendNoteOff(noteNum, 127, channel);
    digitalWrite(ledPin, ledState);
  }
}

inline void blinkLED(int blinkTime) {
  digitalWrite(ledPin, 1);
  delay(blinkTime * 0.5);
  digitalWrite(ledPin, 0);
  delay(blinkTime * 0.5);
}

void setup() {
  // Block until serial is connected (on linux)
  if (debug) {
    while (!Serial)
      ;
  }

  // Setup pins
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLDOWN);

  // Analog resolution stuff
  analogReadResolution(bitsADC);
  analogReadAveraging(readSmoothing);

  volumePot.setAnalogResolution(usableResolution);
  volumePot.enableSleep();
  volumePot.enableEdgeSnap();

  // Read button and use value to set midi cc mode
  if (digitalRead(buttonPin) == HIGH) {
    mode = MODE_14BIT;
    potCCNum += 1;

    // Announce 14 bit mode
    blinkLED(150);
    blinkLED(250);
    blinkLED(70);
    blinkLED(450);
  }

  potDirection = REVERSE;


  // Get initial value when connecting
  readPot();
}

void loop() {
  readPot();
  readMute();

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}
