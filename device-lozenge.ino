// Teensy Midi Controller
// Original by Philip Cunningham

// Edit device name before compiling:
// bin/contrib/arduino-1.6.12/hardware/teensy/avr/cores/usb_midi/usb_private.h

// noise tolerance
const int THRESHOLD = 2;

// The number of pots
const int pots = 8;

int ledPin = 11;
int btnState = 1;

int pot_lookup[pots] = {A0, A1, A2, A3, A4, A5, A6, A7};
int button_toggle[pots] = {0, 0, 0, 0, 0, 0, 0, 0};
//int button_momentary[pots] = {0, 0, 0, 0, 0};

// previous read value for thresholding
int lastreading[pots];
int weighted[pots];

// Set midi channel to send to
int channel = 0x3;

// frame counter
int frame = 0;

void setup()
{
  for (int p = 0; p < pots; p++) {
    lastreading[p] = 0;
    weighted[p] = 0;
  }
  
  // Standard midi rate
  Serial.begin(31250);
  
  for (int p = 0; p < pots; p++) {
    pinMode(pot_lookup[p], INPUT);
    digitalWrite(pot_lookup[p], button_toggle[p] ? HIGH : LOW);
  }
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  delay(250);
  digitalWrite(ledPin, LOW);
  delay(250);
  digitalWrite(ledPin, HIGH);
  delay(250);
  digitalWrite(ledPin, LOW);
  delay(250);
  digitalWrite(ledPin, HIGH);
}

void loop()
{
  // Get reading for all potentiometers & switches
  for (int i = 0; i < pots; i++ ) {
    // http://electronics.stackexchange.com/a/64699
    weighted[i] += (analogRead(pot_lookup[i]) - weighted[i]) / 5;
    if (!(frame % 5)) {
      int midival = weighted[i] / 8;
      int midiprev = lastreading[i] / 8;
      if (abs(lastreading[i] - weighted[i]) > THRESHOLD && midival != midiprev) {
        if (button_toggle[i]) {
          usbMIDI.sendControlChange(pots - i - 1, midival < 64 ? 0 : 127, channel);
//      } else if (button_momentary) {
//        usbMIDI.sendControlChange(i, midival < 64 ? 0 : 127, channel);
        } else {
          usbMIDI.sendControlChange(pots - i - 1, 127 - midival, channel);
        }
        lastreading[i] = weighted[i];
      }
    }
  }
  // Delay output
  delay(2);
  frame += 1;
}
