# Security Policy

YAMuino is a small offline embedded project and does not expose a network service. Security reports are still useful when a defect could cause unsafe electrical behavior, unexpected command transmission, corrupted firmware behavior or a supply-chain problem in repository automation.

## Reporting

If GitHub Private Vulnerability Reporting is enabled for this repository, use it for sensitive reports. Otherwise, open a public issue containing only the minimum non-sensitive information needed to establish the problem and ask the maintainer for a private contact channel if details should not be public.

## Scope

Relevant reports include:

- Repository workflow or dependency issues that could execute untrusted code.
- Firmware defects that could cause persistent unintended output on the System Control line.
- Documentation errors that could create a credible hardware-damage risk.

General feature requests and compatibility questions should use the normal issue templates.
