# Wiring Guide

## Overview

YAMuino uses one Arduino input for infrared reception and one Arduino output for the Yamaha System Control command line.

```text
IR remote
   |
   v
+-------------------+
| IR receiver       |
| OUT ---------- D2 |----+
| GND --------- GND |    |
| VCC ---- supply   |    |
+-------------------+    |
                         v
                    +-----------+
                    | Arduino   |
                    | UNO       |
                    |        D3 |----[ 1 kOhm ]---- TRS TIP
                    |       GND |------------------ TRS SLEEVE
                    +-----------+

TRS RING = Yamaha state-information line; not connected in this version.
```

## Arduino pins

| Pin | Direction | Purpose |
| --- | --- | --- |
| D2 | Input | Demodulated IR receiver output |
| D3 | Output | Yamaha System Control RX command signal |
| GND | - | Shared ground with Yamaha System Control sleeve |
| D0/D1 | Serial | Optional debug UART when `DEBUG` is enabled |

## TRS pinout used by the project

A standard 3.5 mm TRS plug has three contacts:

```text
plug end
   /
  /  TIP    -> RX command line
 /___
     \ RING -> state-information line
      \____
           \ SLEEVE -> GND
```

Always identify the contacts with a continuity tester instead of relying only on cable colors.

## Output connection

The supplied project wiring is:

```text
Arduino D3 ---- 1 kOhm ---- TRS TIP
Arduino GND -------------- TRS SLEEVE
TRS RING ----------------- not connected
```

The firmware's helper functions intentionally implement the polarity used by the original project:

```cpp
void lineLow() {
  digitalWrite(OUT_PIN, HIGH);
}

void lineRelease() {
  digitalWrite(OUT_PIN, LOW);
}
```

The function names refer to the logical active/released bus state, not directly to the Arduino pin voltage.

## IR receiver connection

IR receiver modules do not all share the same physical pin order. Consult the receiver's datasheet or board markings before applying power.

The logical connections are:

```text
IR receiver OUT -> Arduino D2
IR receiver GND -> Arduino GND
IR receiver VCC -> supply voltage specified for the module
```

## Connection procedure

1. Upload the firmware with the Yamaha cable disconnected.
2. Remove power from the Arduino and the CD player.
3. Identify TIP, RING and SLEEVE using a multimeter if the cable is not clearly documented.
4. Install the 1 kOhm series resistor between D3 and TIP.
5. Connect Arduino GND to SLEEVE.
6. Leave RING isolated.
7. Check for accidental shorts between all TRS contacts.
8. Restore power and test basic transport commands.

## Electrical caution

This repository documents the wiring used by the supplied project; it is not a general Yamaha System Control electrical-interface specification. Do not assume that another Yamaha product, cable or board revision uses identical levels.

The Yamaha service documentation describes the ring state line in relative DC levels (`H`, `2/3 H`, `1/3 H`, `0`) rather than as an Arduino-ready logic input. A future ring-reading circuit should therefore be measured and protected appropriately before connection to an MCU input.
