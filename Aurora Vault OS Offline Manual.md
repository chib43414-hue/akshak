# Aurora Vault OS Offline Manual

## Mission

Aurora Vault OS is intended for privacy education, defensive security practice, and safe communication. Perform tests only on systems you own or where you have explicit written permission. A command being technically possible does not make unauthorized access legal or ethical.

## Privacy basics

Use unique passphrases with a password manager, enable multi-factor authentication where appropriate, update software, verify downloads using trusted signatures or hashes, and keep sensitive work in the temporary session unless you intentionally enable encrypted persistence. Treat unexpected links, attachments, QR codes, and login prompts as untrusted until independently verified.

## Threat-model exercise

Write down what you are protecting, who might attack it, what access the attacker has, and what consequences matter. Separate ordinary web tracking from malware, account takeover, physical theft, compromised firmware, and hostile-network scenarios. Choose controls that address the specific threat instead of promising “perfect privacy.”

## Safe security lab workflow

Create an isolated virtual machine or a dedicated test network. Use intentionally vulnerable training targets and keep them disconnected from real accounts and production systems. Record the scope and authorization before testing, capture only the minimum data needed, stop immediately when the scope is exceeded, and securely delete lab data when the exercise is complete.

A safe learning sequence is asset inventory, basic networking, authentication and authorization, secure configuration, logging, vulnerability identification, remediation, and retesting. Exploit development and offensive automation are outside the first Aurora Vault release and must never be used against systems without authorization.

## Communication privacy

The first communication layer should open browser-based services only after the user explicitly enables networking. The system should display that networking is active, show the enabled interface, and provide a clear shutdown control. Browser isolation, cookie deletion at session end, and no saved passwords are preferred defaults.

## Incident response checklist

If you suspect compromise, disconnect networking, do not enter more credentials, record the time and visible symptoms, preserve only the evidence you are authorized to keep, change credentials from a known-clean device, revoke active sessions, notify the service provider, and restore from verified backups. Do not delete evidence blindly if a formal investigation is required.

## Aurora commands planned

The prototype currently provides `help`, `status`, `manual`, and `clear`. Future commands will expose privacy state, device permissions, storage mode, and network controls without hiding important actions behind opaque automation.
