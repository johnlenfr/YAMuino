# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project intends to use semantic versioning for future tagged releases.

## [Unreleased]

### Planned

- Read the Yamaha System Control state-information line from the TRS ring.
- Synchronize the internal Standby / Stop / Play / Pause state with the real CD player state.
- Add optional activity indication.
- Document an optional open-collector output stage if future hardware testing requires it.
- Add enclosure and installation photographs when available.

## [1.0.0] - 2026-09-23

### Added

- Initial GitHub-ready repository based on the supplied `yamaha_v4.ino` firmware.
- Arduino UNO firmware renamed to `YAMuino.ino` for repository/sketch consistency.
- Yamaha CDX-E410 NEC System Control transmission.
- Original Yamaha remote handling for supported CD commands.
- TVIX DVICO NEC remote mapping.
- Power On / Standby remapping.
- Play / Pause / Resume state handling.
- Display Bright / Dimmed / Off cycling.
- NEC repeat filtering.
- English README, manual, wiring, protocol, mapping, troubleshooting and development documentation.
- GitHub issue and pull-request templates.
- GitHub Actions Arduino compile workflow.
- Dependabot configuration for GitHub Actions.

### Known limitations

- The TRS ring state-information line is not read.
- Internal software state can become desynchronized when front-panel buttons are used.
- The output circuit reflects the hardware configuration used by the original project and has not been characterized for other Yamaha models.
