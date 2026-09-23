# Third-Party Material and References

## Arduino IRremote

YAMuino depends on the Arduino **IRremote** library and includes it with:

```cpp
#include <IRremote.hpp>
```

IRremote is a separate third-party project with its own license and release history. Install it through Arduino Library Manager or Arduino CLI. It is not vendored into this repository.

## Yamaha CDX-E410 service documentation

The project was prepared using a Yamaha CDX-E410 service manual supplied with the source archive. The relevant technical material includes the System Control bus description, NEC command/request codes and state-information levels.

The supplied PDF is placed in `reference-local/` only for the repository owner's local reference. That directory is excluded by `.gitignore` so the manual is not automatically republished to GitHub.

If you have permission to redistribute that document and intentionally want it in the public repository, remove or narrow the `reference-local/` ignore rule only after confirming the applicable rights.

The original documentation created for YAMuino in `docs/` summarizes the protocol details required to understand the project and does not require the PDF to be present in the public repository.

## Trademarks

Yamaha, CDX-E410 and RX-E410 are names associated with Yamaha products. TVIX and DVICO are names associated with their respective products or owners. Their use here is descriptive and does not imply endorsement or affiliation.

## Project license

No open-source license has been selected in this prepared package. The repository owner should choose and add an appropriate license if they want to grant public reuse rights beyond ordinary copyright permissions.
