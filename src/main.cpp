#include "Arduino.h"
#include <Bounce.h>
#include <ResponsiveAnalogRead.h>

// the MIDI channel number to send messages
const int channel = 1;

// Setup led pin
const int ledPin = 13; // Used to indicate if volume is muted

// Setup button
const int buttonPin = 1;
Bounce pushbutton = Bounce(buttonPin, 50); // 10 ms debounce
const int noteNum = 44;

// Setup potentiometer
enum POTDIRECTION { NORMAL, REVERSE };
int potDirection{NORMAL};
const int potPin = A0;
const int potCCNum = 7;
ResponsiveAnalogRead volumePot(potPin, true);

// Transmit midi cc mode
// 14 bit or "normal" 7 bit midi
enum MODES { MODE_14BIT, MODE_NORMAL };
int mode{MODE_NORMAL};

// store previously sent values, to detect changes
bool ledState = LOW;
bool muteState = false;

inline void readPot() {
  // Volume fader
  volumePot.update();

  if (volumePot.hasChanged()) {

    // 14 bit midi mode
    if (mode == MODE_14BIT) {
      int potVal = volumePot.getValue();
      potVal = potVal << 1;

      auto lowBitVal = potVal & 0x7F;
      lowBitVal = (potDirection == NORMAL) ? lowBitVal : 127 - lowBitVal;
      auto highBitVal = (potVal >> 7) & 0x7F;
      highBitVal = (potDirection == NORMAL) ? highBitVal : 127 - highBitVal;

      usbMIDI.sendControlChange(potCCNum, lowBitVal, channel);
      usbMIDI.sendControlChange(potCCNum + 32, highBitVal, channel);
    }

    // Normal 7 bit mode
    if (mode == MODE_NORMAL) {
      int potVal = volumePot.getValue();
      potVal = potVal / 8;
      potVal = (potDirection == NORMAL) ? potVal : 127 - potVal;
      usbMIDI.sendControlChange(potCCNum, potVal, channel);
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

inline void initialButtonReadAction() {
  if (readButton() == 1) {
    /* mode = MODE_14BIT; */
    potDirection = NORMAL;
    blinkLED(200);
    blinkLED(200);
  } else {
    potDirection = REVERSE;

    /* mode = MODE_NORMAL; */
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLDOWN);

  /* analogReadResolution(11); */

  /* smooth input values */
  /* analogReadAveraging(16); */

  potDirection = REVERSE;

  if (mode == MODE_14BIT) {
    volumePot.setAnalogResolution(8192);
  }

  // Read button and use value to set midi cc mode
  initialButtonReadAction();

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
