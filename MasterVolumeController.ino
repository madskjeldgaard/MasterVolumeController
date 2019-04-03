#include <Bounce.h>
#include <ResponsiveAnalogRead.h>

// the MIDI channel number to send messages
const int channel = 1;

const int ledPin = 13; // Used to indicate if volume is muted

const int buttonPin = A1;
Bounce pushbutton = Bounce(buttonPin, 10);  // 10 ms debounce

const int potPin = A0;
ResponsiveAnalogRead volumePot(potPin, true);

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Get initial value from fader
  volumePot.update();
  usbMIDI.sendControlChange(7, 127 - (volumePot.getValue() / 8), channel); // Midi cc 7 = volume
}

// store previously sent values, to detect changes
int previousA0 = -1;
bool ledState = LOW;
bool muteState = false;

elapsedMillis msec = 0;

void loop() {
  // only check the analog inputs 50 times per second,
  // to prevent a flood of MIDI messages


  if (msec >= 20) {
    msec = 0;

    // Volume fader
    volumePot.update();

    if (volumePot.hasChanged()) {
      usbMIDI.sendControlChange(7, 127 - (volumePot.getValue() / 8), channel); // Midi cc 7 = volume
    }

    // Mute button
    if (pushbutton.update()) {
      if (pushbutton.fallingEdge()) {
        muteState = !muteState;
        ledState = muteState ? HIGH : LOW;
        usbMIDI.sendNoteOn(44, 127, channel); // Note 44 = master mute
        usbMIDI.sendNoteOff(44, 127, channel);
        digitalWrite(ledPin, ledState);
      }
    }

  }

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}

