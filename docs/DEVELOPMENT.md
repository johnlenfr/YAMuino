# Development Guide

## Project layout

The repository intentionally keeps the Arduino sketch at the root so that a clone named `YAMuino` is also a valid Arduino sketch directory.

```text
YAMuino/
├── YAMuino.ino
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── SECURITY.md
├── .gitignore
├── .gitattributes
├── .editorconfig
├── .github/
│   ├── ISSUE_TEMPLATE/
│   ├── pull_request_template.md
│   ├── dependabot.yml
│   └── workflows/
│       └── arduino-build.yml
├── docs/
│   ├── README.md
│   ├── MANUAL.md
│   ├── WIRING.md
│   ├── PROTOCOL.md
│   ├── REMOTE_MAPPING.md
│   ├── TROUBLESHOOTING.md
│   ├── DEVELOPMENT.md
│   ├── GITHUB_DESKTOP.md
│   └── THIRD_PARTY.md
└── reference-local/        # ignored by Git
    └── ... local reference material ...
```

## Build target

Primary board:

```text
Arduino UNO / ATmega328P
FQBN: arduino:avr:uno
```

Dependency:

```text
IRremote
```

## Building with Arduino IDE

- Board: Arduino UNO
- Sketch: `YAMuino.ino`
- Library: `IRremote`

Use **Verify** before uploading.

## Building with Arduino CLI

Typical setup:

```bash
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli lib install "IRremote"
arduino-cli compile --fqbn arduino:avr:uno .
```

The repository workflow performs these steps on GitHub.

## Firmware structure

The sketch is divided into these logical parts:

1. Yamaha command constants.
2. TVIX command constants.
3. Software CD and display states.
4. Low-level System Control line helpers.
5. NEC bit, byte and frame transmission.
6. Yamaha command transmission helper.
7. Optional serial-debug helpers.
8. High-level Power, Play/Pause and Display behavior.
9. TVIX-to-Yamaha mapping.
10. Arduino setup and receive loop.

## Timing-sensitive code

`sendNECFrame()` temporarily disables interrupts while generating the NEC output frame. Treat changes to the following values as timing-critical:

```text
9000 us header active
4500 us header release
560 us bit active
560 us logical 0 release
1690 us logical 1 release
```

Do not move long serial operations or other interrupt-dependent logic into the timing path.

## Adding Yamaha commands

1. Add a named `CMD_*` constant using the documented hexadecimal value.
2. If the command requires state handling, add an explicit case to `handleCdCommand()`.
3. Otherwise, it can flow through the default `sendYamaha(command)` path.
4. Update `docs/REMOTE_MAPPING.md` and `docs/PROTOCOL.md` as appropriate.

## Adding another remote

Use the TVIX implementation as the pattern:

1. Record the remote protocol, address and command bytes with `DEBUG = 1`.
2. Add the remote address constant.
3. Add named button constants.
4. Add a mapping function that outputs Yamaha command values.
5. Add a new address branch in `loop()`.
6. Define repeat policy explicitly.
7. Document the remote mapping.

## Debugging

Set:

```cpp
#define DEBUG 1
```

Serial speed:

```text
115200 baud
```

Useful output includes:

- protocol name;
- decoded address;
- decoded command;
- flags;
- repeat indication;
- TVIX-to-Yamaha translated command;
- outgoing Yamaha command.

## Future state-line development

The most important architectural improvement is reading the TRS ring state-information line. Before implementing it:

1. Measure the real voltage levels on the target CDX-E410.
2. Design an input that does not load the Yamaha line significantly.
3. Protect the Arduino ADC/input from voltages outside its permitted range.
4. Define thresholds for Play, Stop, Pre-Standby and Standby.
5. Debounce/filter transitions.
6. Keep a fallback behavior for disconnected or invalid state-line values.

The service documentation expresses the levels as ratios of the microprocessor high level, so do not hard-code guessed voltages without measurements.

## Release process

For a future release:

1. Build locally for Arduino UNO.
2. Test the supported remotes on hardware.
3. Update documentation.
4. Move changelog entries from `Unreleased` into a versioned section.
5. Commit and push.
6. Confirm the GitHub Actions build passes.
7. Create a Git tag such as `v1.1.0` and a GitHub Release if desired.
