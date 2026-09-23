# Remote Mapping

## Yamaha remote

Yamaha NEC address/custom code used by the project: `0x78`.

| Remote function | Command | YAMuino behavior |
| --- | ---: | --- |
| Stop | `0x01` | Send Stop; software state -> Stopped |
| Play | `0x02` | Play / Pause / Resume depending on software state |
| Next / Skip+ | `0x03` | Send Next; repeat allowed |
| Previous / Skip- | `0x04` | Send Previous; repeat allowed |
| Random | `0x07` | Send Random |
| Time / Info | `0x0A` | Send Time/Info |
| Program | `0x0B` | Send Program |
| Repeat | `0x0C` | Send Repeat |
| Power key observed by project | `0x0F` | Convert to Power On `0x7E` or Standby `0x7F` |
| Numeric 0 | `0x10` | Direct track digit 0 |
| Numeric 1 | `0x11` | Direct track digit 1 |
| Numeric 2 | `0x12` | Direct track digit 2 |
| Numeric 3 | `0x13` | Direct track digit 3 |
| Numeric 4 | `0x14` | Direct track digit 4 |
| Numeric 5 | `0x15` | Direct track digit 5 |
| Numeric 6 | `0x16` | Direct track digit 6 |
| Numeric 7 | `0x17` | Direct track digit 7 |
| Numeric 8 | `0x18` | Direct track digit 8 |
| Numeric 9 | `0x19` | Direct track digit 9 |
| Pause bus command | `0xB9` | Sent internally for pause/resume |

### Yamaha request/display commands used internally

| Function | Command |
| --- | ---: |
| Power On | `0x7E` |
| Standby | `0x7F` |
| VFD Off | `0xFD` |
| Dimmer On | `0xFE` |
| Dimmer Off | `0xFF` |

`CMD_DISPLAY = 0x4E` is a YAMuino internal pseudo-command and is not transmitted directly.

## TVIX DVICO remote

TVIX NEC address: `0xFD00`.

| TVIX key | TVIX command | Yamaha action |
| --- | ---: | --- |
| Setup | `0x00` | Program `0x0B` |
| Shuffle | `0x01` | Random `0x07` |
| Left | `0x09` | Previous `0x04` |
| Previous | `0x0A` | Previous `0x04` |
| 9 | `0x14` | Numeric 9 `0x19` |
| Goto | `0x15` | Repeat `0x0C` |
| Play/Pause | `0x16` | Play state handler |
| 0 | `0x17` | Numeric 0 `0x10` |
| 2 | `0x18` | Numeric 2 `0x12` |
| 1 | `0x19` | Numeric 1 `0x11` |
| 5 | `0x1A` | Numeric 5 `0x15` |
| 3 | `0x1B` | Numeric 3 `0x13` |
| TV Out | `0x43` | Display-state cycle |
| Right | `0x49` | Next `0x03` |
| Next | `0x4B` | Next `0x03` |
| Info | `0x55` | Time/Info `0x0A` |
| Stop | `0x56` | Stop `0x01` |
| Power | `0x57` | Power toggle handler |
| 6 | `0x58` | Numeric 6 `0x16` |
| 7 | `0x59` | Numeric 7 `0x17` |
| 8 | `0x5A` | Numeric 8 `0x18` |
| 4 | `0x5B` | Numeric 4 `0x14` |

## TVIX keys defined but not used

The supplied firmware also records codes for several TVIX keys that are intentionally not mapped to the CD player:

| Key | Code | Current status |
| --- | ---: | --- |
| Menu | `0x02` | Unused |
| Picture | `0x03` | Unused |
| Volume + | `0x04` | Reserved for possible RX-E410 use |
| Volume - | `0x05` | Reserved for possible RX-E410 use |
| Zoom | `0x06` | Unused |
| Subtitle | `0x07` | Unused |
| OK | `0x08` | Unused |
| Down | `0x0B` | Unused |
| Title | `0x40` | Unused |
| Audio | `0x41` | Unused |
| Angle | `0x42` | Unused |
| Scroll | `0x44` | Unused |
| Mute | `0x45` | Reserved for possible RX-E410 use |
| Bookmark | `0x47` | Unused |
| Up | `0x54` | Unused |

## Adding another NEC remote

To add a new remote:

1. Enable `DEBUG` and capture its NEC address and command values.
2. Define a distinct address constant.
3. Add named command constants.
4. Create a mapping function similar to `mapTvixToYamaha()`.
5. Decide deliberately which commands may repeat while a key is held.
6. Document the new mapping in this file and update the changelog.
