# RaceHorn

An Arduino-based sailboat racing horn that plays the ISAF/US Sailing standard countdown sequence. Built into a waterproof ammo can with a 12V car horn and self-contained LiFePO4 battery.

---

## Bill of Materials

| # | Item | Notes |
|---|------|-------|
| 1 | Arduino Nano clone | |
| 2 | 400W 5V–36V Dual MOSFET High-Power Motor Driver Module | Used to switch the 12V horn from a 5V Arduino signal |
| 3 | 12V car horn | Mounted into ammo can with JB Weld |
| 4 | HARDROCK AMMO Waterproof Ammo Can | Hole cut and horn glued in for weatherproofing |
| 5 | LiFePO4 12V 15Ah lithium battery | Similar: Ampere Time / LiTime 12V 12Ah or 20Ah LiFePO4 |
| 6 | EBOOT MP1584EN DC-DC Buck Converter (6-pack) | Steps 12V battery down to 5V for Arduino |
| 7 | Protoboard | For connecting components |
| 8 | Hookup wire | 22–24 AWG for signal lines; heavier gauge for horn power |
| 9 | Momentary push buttons (×2) | Start and Reset/Cancel |
| 10 | Main power switch | Rated for 12V DC; switches battery positive to all electronics |
| 11 | JB Weld epoxy | For mounting horn to ammo can |

---

## Wiring

### Power

```
12V Battery (+) ──── Power Switch ──┬─────────────────────────────── MOSFET Module VIN+
                                    └──── Buck Converter IN+ ─(5V)── Arduino 5V pin
12V Battery (−) ────────────────────┬─────────────────────────────── MOSFET Module GND
                                    └──── Buck Converter IN− ──────── Arduino GND (common ground)
```

> Set the buck converter output to **5V** by trimming the onboard potentiometer while measuring the output with a multimeter before connecting the Arduino.

### Horn (via MOSFET module)

```
MOSFET Module SIG ──── Arduino D6
MOSFET Module OUT+ ─── Horn (+)
MOSFET Module OUT− ─── Horn (−)  [Horn is not polarity-sensitive on most car horns]
Horn (−) ──────────── 12V Battery (−)   [if not already through module GND]
```

> The MOSFET module's signal input is active-HIGH. When D6 goes HIGH the module switches full 12V through to the horn.

### Buttons

Both buttons are wired to ground; the Arduino's internal pull-up resistors are used.

```
Start Button  ── one leg ── Arduino D2
              ── other leg ── Arduino GND

Reset Button  ── one leg ── Arduino D3
              ── other leg ── Arduino GND
```

### Full Diagram

```
   Battery (+) ──── Power Switch ──┬──────────────────────────────────────────────────┐
   Battery (−) ───────────────────┼──────────────────────────────────────────────────┼──┐
                                  │                                                   │  │
                                  ├──── Buck Converter IN+                            │  │
                                  │     Buck Converter IN− ──── (common GND)          │  │
                                  │     Buck Converter OUT+ (5V)                      │  │
                                  │                 │                                 │  │
                                  │                 ▼                                 │  │
                                  │    ┌─────────────────────────┐                   │  │
                                  │    │      Arduino Nano        │                   │  │
                        Start Button ──│ D2                   5V  │◄── Buck OUT+      │  │
                        Reset Button ──│ D3                  GND  │──── Common GND    │  │
                                  │    │ D6 (Horn signal)         │                   │  │
                                  │    │ D13 (LED, test mode)     │                   │  │
                                  │    └────────────┬────────────┘                   │  │
                                  │                 │ D6                              │  │
                                  │                 ▼                                 │  │
                                  │    ┌─────────────────────────┐                   │  │
                                  │    │   MOSFET Driver Module   │                   │  │
                                  └────│ VIN+              OUT+  │──── Horn (+)       │  │
                                       │ SIG ◄─── D6      OUT−  │──── Horn (−)       │  │
                                   ┌───│ GND                     │                   │  │
                                   │   └─────────────────────────┘                   │  │
                                   └─────────────────────── Common GND ──────────────┘  │
                                                                         Battery (−) ────┘
```

---

## Horn Sequences

Each step in a sequence is 500 ms. `+` = horn on, `_` = horn off.

### Race Countdown (Start button)

| Time to Start | Signal |
|---|---|
| About to start working | 5 short bursts (500 ms each, 500 ms apart) |
| 3 min | 3 long bursts (1 s each, 1 s apart) |
| 2 min | 2 long bursts (1 s each, 1 s apart) |
| 1 min (prep) | 1 long burst |
| ~30s | 3 short bursts, 500 ms apart |
| ~20 s | 2 short bursts, 500 ms apart |
| ~10 s | 1 short burst |
| 5,4,3,2,1 Start | 5 short bursts (500 ms each, 500 ms apart) + 3 s burst |

### Cancel Race (Reset button pressed during a sequence)

5 long bursts (1 s each), separated by 1 s gaps.

### Finisher Signal (Reset button pressed when idle)

Single 2-second burst.

---

## Buttons

| Button | Pin | Idle behavior | During sequence |
|--------|-----|---------------|-----------------|
| Start  | D2  | Starts race countdown | No effect |
| Reset  | D3  | Plays finisher signal (2 s burst) | Cancels race; plays cancel sequence |

---

## Test Mode

Hold the **Start button** while powering on. The device enters test mode:

- The built-in LED (D13) blinks at 500 ms intervals to confirm test mode is active.
- The horn output uses **PWM (50% duty cycle)** via `analogWrite` instead of full 12V switching, reducing horn volume for indoor testing.
- The LED stops blinking once a sequence is started.
- The boot-time button press is ignored — release the button before pressing it again to trigger a sequence.

---

## Enclosure

The horn is mounted through a hole cut in a **HARDROCK AMMO waterproof ammo can** and sealed with **JB Weld** epoxy.  All electronics and the battery are housed inside the can. Step drill bits are helpful for drilling holes for the enclosure.