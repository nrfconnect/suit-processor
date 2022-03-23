This document gathers requirements related to the suit-manifest-processor implementation.
Some requirements are fulfilled by the suit-mp itself, while others are fulfilled indirectly, or must be attended to by the user of suit-mp.

The rest of the document is divided into sections according to who will fulfill the requirements.

# Requirements covered by zcbor-generated code

## Requirements from manifest16[^manifest]:

 * no custom commands are permitted in the common sequence. (6.2)
 * The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. All validators MUST reject any SUIT_Envelope that begins with any element other than a suit-authentication-wrapper (NOTE: key delegation MAY relax this requirement to include a delegation structure as well). (8.3)
 * Full CDDL (Appendix A)

## Requirements from information model[^info_model]:

### Implemented via the CDDL

 * 4.3.1. REQ.SEC.SEQUENCE: Monotonic Sequence Numbers
 * 4.3.2. REQ.SEC.COMPATIBLE: Vendor, Device-Type Identifiers
 * 4.3.5. REQ.SEC.AUTH.IMG_TYPE: Authenticated Payload Type
 * 4.3.6. REQ.SEC.AUTH.IMG_LOC: Authenticated Storage Location
 * 4.3.7. REQ.SEC.AUTH.REMOTE_LOC: Authenticated Remote Payload
 * 4.3.10. REQ.SEC.AUTH.COMPATIBILITY: Authenticated Vendor and Class IDs
 * 4.3.11. REQ.SEC.RIGHTS: Rights Require Authenticity

# Requirements covered by device code in suit-manifest-processor

## Requirements from manifest16[^manifest]:

 * All payloads must be validated prior to loading. (4.2)
 * All loaded images must be validated prior to execution. (4.2)
 * The manifest processor SHOULD check that the common sequence contains at least one Check Vendor Identifier command and at least one Check Class Identifier command. (6.2)
 * Executing an update MUST either result in an error, or a verifiably correct system state. (6.3)
 * Executing a Trusted Invocation MUST either result in an error, or an invoked image. (6.3)
 * Executing the same manifest on multiple Recipients MUST result in the same system state. (6.3)
 * The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. All validators MUST reject any SUIT_Envelope that begins with any element other than a suit-authentication-wrapper (NOTE: key delegation MAY relax this requirement to include a delegation structure as well). (8.3)

## Requirements from information model[^info_model]:

### 4.3.4. REQ.SEC.AUTHENTIC: Cryptographic Authenticity

The authenticity of an update MUST be demonstrable. Typically, this means that updates must be digitally signed. Because the manifest contains information about how to install the update, the manifest's authenticity must also be demonstrable. To reduce the overhead required for validation, the manifest contains the cryptographic digest of the firmware image, rather than a second digital signature. The authenticity of the manifest can be verified with a digital signature or Message Authentication Code. The authenticity of the firmware image is tied to the manifest by the use of a cryptographic digest of the firmware image.

__Mitigates:__ THREAT.IMG.NON_AUTH (Section 4.2.9), THREAT.NET.ONPATH (Section 4.2.7)

__Implemented by:__ Signature (Section 3.15), Payload Digests (Section 3.13)

### 4.3.8. REQ.SEC.AUTH.EXEC: Secure Execution

The target SHOULD verify firmware at the time of boot. This requires authenticated payload size and firmware digest.

__Mitigates:__ THREAT.IMG.REPLACE (Section 4.2.8)

__Implemented by:__ Payload Digests (Section 3.13), Size (Section 3.14)

### 4.3.21. REQ.SEC.MFST.CONST: Manifest Kept Immutable between Check and Use

Both the manifest and any data extracted from it MUST be held
immutable between its authenticity verification (time of check) and
its use (time of use). To make this guarantee, the manifest MUST fit
within internal memory or secure memory, such as encrypted memory.
The recipient SHOULD defend the manifest from tampering by code or
hardware resident in the recipient -- for example, other processes or
debuggers.

If an application requires that the manifest be verified before
storing it, then this means the manifest MUST fit in RAM.

__Mitigates:__ THREAT.MFST.TOCTOU (Section 4.2.18)

__Implemented by:__ Proper system design with sufficient resources and
implementation avoiding TOCTOU attacks

# Requirements covered by platform code

## Requirements from information model[^info_model]:

### 4.3.13. REQ.SEC.ACCESS_CONTROL: Access Control

If a device grants different rights to different actors, then an exercise of those rights MUST be validated against a list of rights for the actor. This typically takes the form of an Access Control List (ACL). ACLs are applied to two scenarios:
An ACL decides which elements of the manifest may be overridden and by which actors.
An ACL decides which component identifier / storage identifier pairs can be written by which actors.

__Mitigates:__ THREAT.MFST.OVERRIDE (Section 4.2.13), THREAT.UPD.UNAPPROVED (Section 4.2.11)

__Implemented by:__ Client-side code, not specified in manifest

### 4.3.16. REQ.SEC.REPORTING: Secure Reporting

Status reports from the device to any remote system MUST be performed
over an authenticated, confidential channel in order to prevent
modification or spoofing of the reports.

__Mitigates:__ THREAT.NET.ONPATH (Section 4.2.7)

__Implemented by:__ Transport Security / Manifest format triggering
generation of reports

### 4.3.18. REQ.SEC.KEY.ROTATION: Protected Storage of Signing Keys

Cryptographic keys for signing/authenticating manifests SHOULD be
replaced from time to time. Because it is difficult and risky to
replace a trust anchor, keys used for signing updates SHOULD be
delegates of that trust anchor.

If key expiration is performed based on time, then a secure clock is
needed. If the time source used by a recipient to check for
expiration is flawed, an old signing key can be used as current,
which compounds THREAT.KEY.EXPOSURE (Section 4.2.16).

__Mitigates:__ THREAT.KEY.EXPOSURE (Section 4.2.16)

__Implemented by:__ Secure storage technology, which is a system design/
implementation aspect outside the scope of the manifest format

# Requirements covered by the suit-manifest-processor build system

## Requirements from manifest16[^manifest]:

 * The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. All validators MUST reject any SUIT_Envelope that begins with any element other than a suit-authentication-wrapper (NOTE: key delegation MAY relax this requirement to include a delegation structure as well). (8.3)
 * A SUIT_Envelope that has not had authentication information added MUST still contain the suit-authentication-wrapper element, but the content MUST be a list containing only the SUIT_Digest. (8.3)
 * Full CDDL (Appendix A)
  * Regardless of the modifications to the CDDL used to build the suit-mp, or to generate the manifest, the build system should contain a step where the manifest is validated against a (largely) unmodified version of the Full CDDL.

## Requirements from information model[^info_model]:

### [4.3.8. REQ.SEC.AUTH.EXEC: Secure Execution](#438-reqsecauthexec-secure-execution)

### 4.3.15. REQ.SEC.IMG.COMPLETE_DIGEST: Whole Image Digest

The digest SHOULD cover all available space in a fixed-size storage
location. Variable-size storage locations MUST be restricted to
exactly the size of deployed payload. This prevents any data from
being distributed without being covered by the digest. For example,
XIP microcontrollers typically have fixed-size storage. These
devices should deploy a digest that covers the deployed firmware
image, concatenated with the default erased value of any remaining
space.

__Mitigates:__ THREAT.IMG.EXTRA (Section 4.2.15)

__Implemented by:__ Payload Digests (Section 3.13)

# Requirements covered by user

## Requirements from manifest16[^manifest]:

 * The author MUST ensure that all parameters consumed by a command are set prior to invoking that command. Where Component Index = True, this means that the parameters consumed by each command MUST have been set for each Component. (7)
 * A signing application MUST verify the suit-manifest element against the SUIT_Digest prior to signing. (8.3)
 * All SUIT_Component_Identifiers present in suit-text MUST also be present in suit-common (8.4.4)

## Requirements from information model[^info_model]:

### 4.3.17. REQ.SEC.KEY.PROTECTION: Protected Storage of Signing Keys

Cryptographic keys for signing/authenticating manifests SHOULD be
stored in a manner that is inaccessible to networked devices -- for
example, in an HSM or an air-gapped computer. This protects against
an attacker obtaining the keys.

Keys SHOULD be stored in a way that limits the risk of a legitimate,
but compromised, entity (such as a server or developer computer)
issuing signing requests.

__Mitigates:__ THREAT.KEY.EXPOSURE (Section 4.2.16)

__Implemented by:__ Hardware-assisted isolation technologies, which are
outside the scope of the manifest format

### [4.3.18. REQ.SEC.KEY.ROTATION: Protected Storage of Signing Keys](#4318-reqseckeyrotation-protected-storage-of-signing-keys)

### 4.3.19. REQ.SEC.MFST.CHECK: Validate Manifests prior to Deployment

Manifests SHOULD be verified prior to deployment. This reduces
problems that may arise with devices installing firmware images that
damage devices unintentionally.

__Mitigates:__ THREAT.MFST.MODIFICATION (Section 4.2.17)

__Implemented by:__ Testing infrastructure. While outside the scope of
the manifest format, proper testing of low-level software is
essential for avoiding unnecessary downtime or worse situations.

### 4.3.20. REQ.SEC.MFST.TRUSTED: Construct Manifests in a Trusted Environment

For high-risk deployments, such as large numbers of devices or
devices that provide critical functions, manifests SHOULD be
constructed in an environment that is protected from interference,
such as an air-gapped computer. Note that a networked computer
connected to an HSM does not fulfill this requirement (see
THREAT.MFST.MODIFICATION (Section 4.2.17)).

__Mitigates:__ THREAT.MFST.MODIFICATION (Section 4.2.17)

__Implemented by:__ Physical and network security for protecting the
environment where firmware updates are prepared to avoid
unauthorized access to this infrastructure

# References

[^info_model]: IETF RFC 9124 - A Manifest Information Model for Firmware Updates in Internet of Things (IoT) Devices - https://datatracker.ietf.org/doc/rfc9124/
[^manifest]: IETF draft-ietf-suit-manifest-16 - A Concise Binary Object Representation (CBOR)-based Serialization Format for the Software Updates for Internet of Things (SUIT) Manifest - https://datatracker.ietf.org/doc/html/draft-ietf-suit-manifest-16
