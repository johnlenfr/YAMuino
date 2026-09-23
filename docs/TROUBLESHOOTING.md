# Troubleshooting

## The CD player does not react to any command

Check the problem in this order:

1. Confirm that the Arduino is powered and the sketch uploaded successfully.
2. Confirm that the IR receiver output is connected to D2.
3. Enable `DEBUG` and check whether NEC frames appear at 115200 baud.
4. Confirm that the received address is `0x0078` for Yamaha or `0xFD00` for TVIX.
5. Verify the 1 kOhm resistor path from D3 to TRS TIP.
6. Verify Arduino GND to TRS SLEEVE.
7. Confirm that TIP, RING and SLEEVE have not been misidentified.
8. Leave RING disconnected while testing this firmware version.

## Debug output shows IR frames, but Yamaha does not respond

If reception works but the CD player remains inactive:

- verify that D3 is the configured `OUT_PIN`;
- verify the TRS cable continuity;
- verify that the 1 kOhm resistor is connected in series rather than from signal to ground;
- confirm that the target really is a CDX-E410 and not a different System Control implementation;
- test Stop or Play from a known powered-on state before testing Power behavior.

## The remote is ignored

The firmware accepts only the NEC protocol and only two addresses:

- Yamaha: `0x0078`
- TVIX: `0xFD00`

Enable debug mode to see the decoded protocol, address and command. A visually similar remote may use a different address or protocol.

## One key produces multiple actions

The firmware already rejects most NEC repeat frames. If duplicates remain:

- check whether the IR receiver is producing multiple independent frames rather than repeat frames;
- inspect the debug `flags` value;
- avoid increasing delays excessively because that can make held navigation keys feel unresponsive;
- note that Next/Previous repeats are intentionally allowed.

## Next or Previous repeats while held

This is intentional. Yamaha Next/Previous and TVIX Next/Previous/Right/Left allow repeat frames.

## Play/Pause behavior is wrong after using front-panel buttons

This is a known limitation. YAMuino tracks playback state in software and does not yet read the Yamaha state-information line.

Front-panel Play, Stop, Eject or Standby operations can desynchronize the software state. Restore a known state or reset the Arduino. A future revision can use the TRS ring to synchronize automatically.

## Power toggle is reversed

The Power action also depends on YAMuino's software state. If the Arduino starts while the player is actually in Standby, the default `CD_STOPPED` state can be wrong.

A temporary workaround is to put both devices into a known state and restart the Arduino. The long-term fix is to implement state-information sensing.

## Display cycle is out of sync

Display state is tracked locally as Bright, Dimmed or Off. Front-panel or other external display changes are not detected. Cycle the TVIX display key until the visible state and software cycle align again.

## Serial Monitor is blank

Make sure:

```cpp
#define DEBUG 1
```

Then upload the firmware again and select **115200 baud** in Serial Monitor.

Debug serial initialization is compiled out when `DEBUG` is `0`.

## Compilation fails on `IRremote.hpp`

Install or update the **IRremote** library through Arduino Library Manager. This firmware uses the modern header:

```cpp
#include <IRremote.hpp>
```

It is not written for very old releases that used a different API.

## GitHub Actions build fails while local Arduino IDE build works

Open the workflow log and check whether:

- the Arduino AVR core installed correctly;
- the `IRremote` library installed correctly;
- a future `IRremote` release introduced an API change.

If a library update breaks compatibility, pin a tested library version in the workflow and document that version in the manual and changelog.
