# Aurora Vault OS Privacy Model

## Privacy promise

Aurora Vault OS is designed to collect as little information as possible. The first bootable prototype does not include a network stack, disk driver, filesystem, telemetry, analytics, camera driver, microphone driver, or AI network client. It therefore cannot communicate with websites or save files yet. This is intentional: the privacy boundary is established before convenience features are added.

## Default states

| Capability | Default | User-visible policy |
|---|---|---|
| Network | Off | A future network service must require an explicit enable action |
| Storage | Ephemeral | A future persistent mode must be separately selected and encrypted |
| Camera | Off | No camera driver in the prototype; future access must show an indicator |
| Microphone | Off | No microphone driver in the prototype; future access must show an indicator |
| Telemetry | None | No analytics, crash uploads, or background tracking |
| AI | Local-only planned | External AI must be opt-in and clearly labeled |
| Browser data | Not yet implemented | Future browser sessions should delete cookies and passwords by default |

## What temporary means

In the planned live mode, working state will be held in volatile memory wherever practical and will be discarded when the machine powers off. Temporary mode does not erase data that was already written by firmware, a peripheral, another operating system, a network peer, or a compromised device. Users who need stronger sanitization should use documented, hardware-appropriate procedures rather than relying on a slogan.

## Threats addressed

The design addresses routine telemetry, accidental persistence, casual tracking, and accidental exposure of unused sensors. It will later add process isolation, least-privilege device access, encrypted persistence, signed release artifacts, reproducible builds, and an auditable permission panel.

## Threats not solved by this prototype

The system does not yet defend against compromised firmware, a malicious USB device, hardware implants, supply-chain compromise, physical observation, radio side channels, a hostile network, or a fully compromised browser. No software-only operating system can honestly guarantee absolute anonymity or privacy against every adversary.

## Trust and verification

Users should download source and release artifacts from the project's chosen official repository, verify checksums or signatures, test the image in a virtual machine, and keep the original operating system recoverable. Never write an unverified image to a valuable disk, and never test security tools against systems without authorization.
