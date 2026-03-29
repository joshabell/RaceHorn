// RaceHorn — Sailboat racing horn sequencer
// Hardware: Arduino Nano clone
//   Start button: D2 (INPUT_PULLUP, active LOW)
//   Reset button: D3 (INPUT_PULLUP, active LOW)
//   Horn (MOSFET module): D6

#define START_BUTTON  2
#define RESET_BUTTON  3
#define HORN_PIN      6
#define STEP_MS       500
#define PWM_LEVEL     128  // ~50% duty cycle used in test mode to reduce volume

// Each character = 500 ms. '+' = horn on, '_' = horn off.

// Main race countdown sequence:
//   5 short bursts
//   5 s silence
//   3 long bursts @ 1 s gap          <- 5-minute signal
//   1 minute silence
//   2 long bursts @ 1 s gap          <- 4-minute signal
//   1 minute silence
//   1 long burst                     <- 3-minute (prep) signal
//   30 s silence
//   3 short bursts @ 500 ms gap
//   10 s silence
//   2 short bursts @ 500 ms gap
//   10 s silence
//   1 short burst
//   5 s silence
//   5 short bursts @ 500 ms gap      <- start signal
//   3 second burst                   <- end signal
const char RACE_SEQUENCE[] =
  "+_+_+_+_+"                                                          // 5 short bursts, 500 ms gaps (4.5 s)
  "__________"                                                         // 5 s silence               (5 s)
  "++_++_++"                                                           // 3 long bursts, 500ms gaps   (4 s)
  "____________________________________________________________"       //
  "____________________________________________________"               //  > 1 min silence (120-8 chars) (-4 seconds to account for 3 minute horn)
  "++_++"                                                              // 2 long bursts, 500ms gap    (2.5 s)
  "____________________________________________________________"       //
  "_______________________________________________________"            //  > 1 min silence (-2.5 seconds to account for 2 minut horn)
  "++"                                                                 // 1 long burst              (1 s)
  "__________________________________________________________"         // 30 s silence (60-2 chars) (-1 second to account for 1 minute)
  "+_+_+"                                                              // 3 short bursts, 500 ms gaps (2.5 s)
  "_______________"                                                    // 10 s silence              (20-5 chars)
  "+_+"                                                                // 2 short bursts, 500 ms gap (1.5 s)
  "_________________"                                                  // 10 s silence
  "+"                                                                  // 1 short burst             (0.5 s)
  "_________"                                                          // 5 s silence
  "+_+_+_+_+"                                                          // 5 short bursts, 500 ms gaps (4.5 s)
  "++++++_";                                                           // 3 second burst               (3 s)

// Cancel-race sequence: 5 long bursts each separated by 1 second
const char CANCEL_SEQUENCE[] = "++__++__++__++__++";

// Finish sequence: single 2-second burst (cancel pressed outside a race)
const char FINISH_SEQUENCE[] = "++";

bool testMode = false;
bool sequenceRunning = false;

unsigned long lastLedToggle = 0;
bool ledState = false;

// ── Horn control ─────────────────────────────────────────────────────────────

void hornOn() {
  if (testMode) {
    analogWrite(HORN_PIN, PWM_LEVEL);
  } else {
    digitalWrite(HORN_PIN, HIGH);
  }
}

void hornOff() {
  if (testMode) {
    analogWrite(HORN_PIN, 0);
  } else {
    digitalWrite(HORN_PIN, LOW);
  }
}

// ── Sequence player ───────────────────────────────────────────────────────────
// Returns true if the sequence completed, false if cancelled mid-way.
// Pass cancelable=true only for the main race sequence.

bool playSequence(const char* seq, bool cancelable) {
  int len = strlen(seq);
  for (int i = 0; i < len; i++) {
    if (cancelable && digitalRead(RESET_BUTTON) == LOW) {
      hornOff();
      return false;
    }

    if (seq[i] == '+') {
      hornOn();
    } else {
      hornOff();
    }
    delay(STEP_MS);
  }
  hornOff();
  return true;
}

// ── Arduino lifecycle ─────────────────────────────────────────────────────────

void setup() {
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(HORN_PIN, OUTPUT);
  hornOff();

  // Test mode: start button held during power-on
  if (digitalRead(START_BUTTON) == LOW) {
    testMode = true;
    pinMode(LED_BUILTIN, OUTPUT);
    // Wait for button release so the held press doesn't trigger a sequence
    while (digitalRead(START_BUTTON) == LOW) { /* wait */ }
  }
}

void loop() {
  // ── Test mode LED blink (stops once a sequence starts) ────────────────────
  if (testMode && !sequenceRunning) {
    unsigned long now = millis();
    if (now - lastLedToggle >= 500) {
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
      lastLedToggle = now;
    }
  }

  // ── Start button: begin race sequence ──────────────────────────────────────
  if (!sequenceRunning && digitalRead(START_BUTTON) == LOW) {
    delay(50);  // debounce
    if (digitalRead(START_BUTTON) == LOW) {
      sequenceRunning = true;
      if (testMode) digitalWrite(LED_BUILTIN, LOW);
      bool completed = playSequence(RACE_SEQUENCE, true);
      if (!completed) {
        // Cancelled mid-race: play cancel sequence (not interruptible)
        playSequence(CANCEL_SEQUENCE, false);
      }
      sequenceRunning = false;
    }
  }

  // ── Reset button (outside a race): indicate race finish ───────────────────
  // Cancel during a race is handled inside playSequence above.
  if (!sequenceRunning && digitalRead(RESET_BUTTON) == LOW) {
    delay(50);  // debounce
    if (digitalRead(RESET_BUTTON) == LOW) {
      playSequence(FINISH_SEQUENCE, false);
    }
  }
}
