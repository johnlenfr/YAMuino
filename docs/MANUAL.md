# YAMuino Manual

## 1. Purpose

YAMuino is an Arduino UNO bridge that allows a Yamaha CDX-E410 CD player to receive remote-control commands without relying on the matching Yamaha RX-E410 receiver/amplifier.

The CDX-E410 System Control connection contains two logical lines:

- an **RX command line**, carrying demodulated NEC-format commands from the receiver to the CD player;
- a **state-information line**, returning the CD player's state as a DC voltage level.

The current YAMuino firmware implements the RX command line only. The state-information line is reserved for a future version.

## 2. Supported behavior

The firmware can receive commands from:

- the original Yamaha remote using Yamaha CD address/custom code `0x78`;
- a TVIX DVICO remote using NEC address `0xFD00`, with explicit command translation.

It supports Stop, Play, Pause, Next, Previous, Random, Time/Info, Program, Repeat, numeric track selection, Power On/Standby and display control as described in this repository.

## 3. Hardware required

- Arduino UNO.
- NEC-compatible IR receiver module suitable for the Arduino supply level.
- 3.5 mm TRS cable or plug for the Yamaha System Control connection.
- 1 kOhm resistor in series between Arduino D3 and the TRS tip.
- Jumper wires or a suitable permanent wiring method.
- Yamaha CDX-E410 CD player.
- A supported Yamaha or TVIX DVICO infrared remote.

A breadboard is useful during initial testing but is not required for a permanent installation.

## 4. Wiring

### 4.1 IR receiver

Connect the IR receiver according to its own pinout:

- receiver output -> Arduino D2;
- receiver ground -> Arduino GND;
- receiver supply -> the voltage required by the receiver module.

Do not assume that every three-pin IR module uses the same physical pin order.

### 4.2 Yamaha System Control TRS

| TRS contact | Function | Connection |
| --- | --- | --- |
| TIP | RX command line | Arduino D3 through 1 kOhm |
| RING | State information | Leave unconnected in this version |
| SLEEVE | Ground | Arduino GND |

The firmware uses D3 as an active command output. `lineLow()` writes the pin HIGH and `lineRelease()` writes it LOW because this project intentionally uses the output polarity shown in the supplied working implementation.

See [WIRING.md](WIRING.md) before connecting the player.

## 5. Software installation

### 5.1 Arduino IDE

1. Install Arduino IDE 2.x.
2. Install the Arduino AVR Boards package if it is not already present.
3. Open **Library Manager**.
4. Install the library named **IRremote**.
5. Place this repository in a folder named `YAMuino`.
6. Open `YAMuino.ino`.
7. Select **Arduino UNO** as the board.
8. Select the correct serial port.
9. Click **Verify**.
10. Click **Upload**.

The original source header records Arduino IDE 2.3.6 as the environment used during development.

### 5.2 Arduino CLI

After Arduino CLI and the AVR core are installed, the essential build command is:

```bash
arduino-cli compile --fqbn arduino:avr:uno .
```

The GitHub Actions workflow in `.github/workflows/arduino-build.yml` performs an equivalent automated build.

## 6. First power-up and test

1. Upload the firmware before connecting the Yamaha System Control cable.
2. Disconnect power from the Arduino and CD player while making the TRS connection.
3. Verify continuity and confirm that TIP, RING and SLEEVE have not been swapped.
4. Reconnect the Arduino and CD player.
5. Put the CD player in a known powered-on and stopped state.
6. Test **Play**, **Stop**, **Next** and **Previous**.
7. Test Power only after the basic transport commands are working.
8. If commands are not received, enable debug mode as described below.

## 7. Operating logic

### 7.1 Yamaha remote

For Yamaha frames at address `0x78`, the received command is normally passed to the CD command handler without translation.

Most repeat frames are ignored. Repeats are accepted for Next and Previous so that a held key can continue generating navigation commands.

### 7.2 TVIX remote

For TVIX frames at address `0xFD00`, the firmware converts the TVIX command to a Yamaha command. The complete mapping is in [REMOTE_MAPPING.md](REMOTE_MAPPING.md).

Repeats are accepted only for Next, Previous, Right and Left. Numeric keys are therefore emitted once even when a key is held slightly too long.

### 7.3 Power

The Yamaha remote power key observed by this project produces command `0x0F`. This is treated as a local toggle request rather than being sent directly to the CD player:

- if the internal state is Standby, YAMuino sends Yamaha request `0x7E` (Power On);
- otherwise, YAMuino sends Yamaha request `0x7F` (Standby).

The TVIX Power key is mapped to the same behavior.

### 7.4 Play / Pause / Resume

The firmware tracks one of four software states:

- `CD_STANDBY`
- `CD_STOPPED`
- `CD_PLAYING`
- `CD_PAUSED`

The Play command behaves as follows:

| Current software state | Command sent | New software state |
| --- | --- | --- |
| Stopped | PLAY `0x02` | Playing |
| Playing | PAUSE `0xB9` | Paused |
| Paused | PAUSE `0xB9` | Playing |

Stop sends `0x01` and changes the software state to Stopped.

### 7.5 Display control

`CMD_DISPLAY` (`0x4E`) is an **internal YAMuino pseudo-command** used by the TVIX mapping. It is not sent to the Yamaha bus as `0x4E`.

Instead, it rotates through three local display states:

1. Bright -> send `DIMMER_ON` (`0xFE`)
2. Dimmed -> send `VFD_OFF` (`0xFD`)
3. Off -> send `DIMMER_OFF` (`0xFF`)

## 8. NEC output generated by YAMuino

The Yamaha command output is produced in software with the timing used by the supplied implementation:

- header active interval: 9000 microseconds;
- header release interval: 4500 microseconds;
- bit active interval: 560 microseconds;
- bit 0 release interval: 560 microseconds;
- bit 1 release interval: 1690 microseconds;
- bits transmitted least-significant bit first.

The emitted frame is:

```text
custom | bitwise inverse custom | command | bitwise inverse command
```

For the CDX-E410, `custom` is `0x78`.

Interrupts are temporarily disabled while a frame is transmitted to keep timing stable.

## 9. Debugging

At the top of `YAMuino.ino`:

```cpp
#define DEBUG 0
```

Change it to:

```cpp
#define DEBUG 1
```

Recompile and upload. Open the Serial Monitor at **115200 baud**.

The firmware will report received protocol, address, command, flags, repeat detection, TVIX mapping and outgoing Yamaha commands.

Return `DEBUG` to `0` for normal operation if serial diagnostics are not required.

## 10. Important limitation: software-only state

The TRS ring is not sampled by this version. The firmware therefore does not know the CD player's real state after every possible external action.

The software state can become inaccurate when, for example:

- Play, Stop, Eject or Standby is pressed on the CD player's front panel;
- the player changes state because playback ends;
- the player performs an internal fail-safe or automatic standby transition;
- power is removed and restored independently of the Arduino.

If state becomes desynchronized, put the player into a known state and restart the Arduino, or use direct Stop/Power actions as appropriate.

## 11. State-information line reserved for future work

The Yamaha service documentation describes the ring state-information line using voltage ratios relative to the CD microprocessor supply level:

| State | Documented level |
| --- | --- |
| Play | H |
| Stop / powered on other than Play | H x 2/3 |
| Pre-standby / initialization transition | H x 1/3 |
| Standby | 0 |

A future YAMuino revision could read this line through a suitably designed high-impedance input and use it to synchronize the software state. Do not connect the ring directly to an Arduino input until the actual voltage levels and required protection have been verified on the target hardware.

## 12. Maintenance

When changing the firmware:

- keep the Yamaha custom code and command table documented;
- verify timing-sensitive changes on real hardware;
- update the relevant documentation file;
- add a changelog entry;
- make sure the GitHub Actions build passes after publication.

## 13. Reference material

This repository's protocol documentation was prepared from the supplied Yamaha CDX-E410 service manual, especially the **System Control** section, together with the supplied firmware and observed remote codes recorded in that firmware.

The third-party service manual is intentionally not tracked by Git in this prepared repository. See [THIRD_PARTY.md](THIRD_PARTY.md).
