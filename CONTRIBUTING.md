# Contributing to YAMuino

Contributions are welcome when they improve compatibility, documentation, reliability or test coverage without obscuring the small embedded design.

## Development setup

1. Use an Arduino UNO or a compatible ATmega328P board for firmware testing.
2. Install Arduino AVR board support.
3. Install the `IRremote` library.
4. Open `YAMuino.ino` from a folder named `YAMuino`.
5. Build for `arduino:avr:uno` before submitting a change.

See [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md) for the full workflow.

## Pull requests

- Keep changes focused and explain the hardware or behavioral reason for them.
- Preserve the existing NEC timing unless a change is supported by measurements or documentation.
- Document any new remote address, command code or wiring requirement.
- Update `CHANGELOG.md` for user-visible changes.
- Update the relevant document in `docs/` when behavior or wiring changes.
- Do not commit local service manuals, build outputs, IDE caches or captured personal files.

## Code style

- Use descriptive names for new constants and functions.
- Keep command codes as hexadecimal values.
- Prefer small functions over deeply nested logic.
- Keep RAM use appropriate for Arduino UNO.
- Use `F("...")` for new constant debug strings where practical.
- Keep debug output behind the `DEBUG` compile-time switch.

## Hardware reports

When reporting a hardware result, include:

- Arduino board and revision.
- IR receiver model if known.
- Yamaha player model.
- Output-stage wiring and resistor values.
- Remote model and measured NEC address/command values when relevant.
- Whether the test used front-panel controls between remote commands.

Do not post copyrighted service-manual scans or personal information in issues.
