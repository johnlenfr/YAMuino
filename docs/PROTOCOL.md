# Protocol Notes

## Yamaha System Control bus

The Yamaha CDX-E410 service documentation describes an external System Control bus between the RX-E410 receiver and CDX-E410 CD player with two lines:

1. **RX command line** - requests and operation-key codes from the receiver to the CD player.
2. **State-information line** - CD player state represented by a DC voltage level.

YAMuino currently implements only the RX command line.

## RX command format

The System Control command signal is documented as a **demodulated NEC infrared remote-control waveform**. YAMuino therefore does not transmit a modulated 38 kHz IR carrier on D3; it transmits the demodulated timing directly to the command line.

The firmware emits this NEC frame:

```text
+-----------+----------+---------+---------+-----------+-----------+
| 9 ms mark | 4.5 ms   | custom  | ~custom | command   | ~command  |
|           | space    | 8 bits  | 8 bits  | 8 bits    | 8 bits   |
+-----------+----------+---------+---------+-----------+-----------+
                                                     + final 560 us mark
```

Each data byte is sent least-significant bit first.

### Bit timing used in the firmware

| Element | Active interval | Release interval |
| --- | ---: | ---: |
| Header | 9000 us | 4500 us |
| Bit 0 | 560 us | 560 us |
| Bit 1 | 560 us | 1690 us |
| Final mark | 560 us | - |

## Yamaha custom code

The CDX-E410 command custom code used by the firmware and service documentation is:

```text
0x78
```

The full standard frame therefore contains `0x78`, `~0x78`, the command byte, and the inverse command byte.

## Yamaha request commands

These commands are documented as RX request commands:

| Name | Code | Purpose |
| --- | --- | --- |
| Power On | `78-7E` | Power-on request |
| Power Off / Standby | `78-7F` | Standby request |
| Play | `78-02` | Playback request |
| Stop | `78-01` | Playback stop request |
| Dimmer On | `78-FE` | Reduce display brightness |
| Dimmer Off | `78-FF` | Cancel display dimming |
| VFD Off | `78-FD` | Turn the front display off |

## Operation-key codes

The service documentation lists these operation keys under custom code `0x78`:

| Data | Function |
| --- | --- |
| `01` | Stop |
| `02` | Play |
| `B9` | Pause |
| `03` | Skip/Scan + |
| `04` | Skip/Scan - |
| `07` | Random |
| `0A` | Time/Info |
| `0B` | Program |
| `0C` | Repeat |
| `10`..`19` | Numeric 0..9 |
| `8E` | Up / Folder + |
| `8F` | Down / Folder - |
| `9E` | Right / File + |
| `9F` | Left / File - |
| `C1` | Enter |

The current YAMuino firmware uses the CD transport, program, repeat and numeric subset. The folder/file/enter codes are documented here for completeness but are not currently mapped by the supplied sketch.

## Project-specific power-key handling

The supplied firmware records that the original remote's Power key is received as command `0x0F`. YAMuino does not forward `0x0F` as a CD command. It uses that event as a software toggle and sends either request `0x7E` or `0x7F` according to its current software state.

## Internal display pseudo-command

`CMD_DISPLAY = 0x4E` exists only inside YAMuino's software mapping. It is used to trigger a three-state display cycle and is never transmitted directly as Yamaha command `78-4E` by `handleCdCommand()`.

## State-information line

The service documentation represents CD state using DC levels relative to a high level `H`:

| Name | Level | Meaning |
| --- | --- | --- |
| `PORT_PLAY` | `H` | Playback period |
| `PORT_STOP` | `H x 2/3` | Powered on, other than Play |
| `PORT_PRE_STANDBY` | `H x 1/3` | Standby/initialization transition |
| `PORT_STANDBY` | `0` | Standby |

The current firmware does not sample this line.

## IR input protocols

YAMuino accepts only frames decoded by `IRremote` as NEC:

```cpp
if (protocol != NEC) {
  return;
}
```

Two input addresses are recognized:

| Remote | Address |
| --- | --- |
| Yamaha CD remote | `0x0078` |
| TVIX DVICO remote | `0xFD00` |

All other addresses are ignored.

## Repeat frames

The `IRDATA_FLAGS_IS_REPEAT` flag is used to reject repeated key frames for most actions.

- Yamaha: repeat is accepted for Next and Previous.
- TVIX: repeat is accepted for Next, Previous, Right and Left.
- Other mapped commands, including numeric keys, are sent only on the initial press.

This filtering is separate from the 150 ms delay after each outgoing Yamaha frame and the 30 ms delay before the high-level command handler.
