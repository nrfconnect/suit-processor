This document gathers requirements related to the suit-manifest-processor (suit-mp) implementation.
Some requirements are fulfilled by the suit-mp itself, while others are fulfilled indirectly, or must be attended to by the user of suit-mp.

The rest of the document is divided into sections according to who will fulfill the requirements.

Any text in _italics_ is quoted from the provided link.


# Requirements covered by zcbor-generated code

## Requirements from manifest18[^manifest]:

 * _no custom commands are permitted in the common sequence._ ([6.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-required-checks))
 * _The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. A SUIT_Envelope that has not had authentication information added MUST still contain the suit-authentication-wrapper element, but the content MUST be a list containing only the SUIT_Digest._ ([8.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-authenticated-manifests))
 * Full CDDL ([Appendix A](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-a-full-cddl))

## Requirements from information model[^info_model]:

### Implemented via the CDDL

 * [_4.3.1. REQ.SEC.SEQUENCE: Monotonic Sequence Numbers_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecsequence-monotonic-se)
 * [_4.3.2. REQ.SEC.COMPATIBLE: Vendor, Device-Type Identifiers_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqseccompatible-vendor-dev)
 * [_4.3.5. REQ.SEC.AUTH.IMG_TYPE: Authenticated Payload Type_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqseccompatible-vendor-dev)
 * [_4.3.6. REQ.SEC.AUTH.IMG_LOC: Authenticated Storage Location_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthimg_loc-authentic)
 * [_4.3.7. REQ.SEC.AUTH.REMOTE_LOC: Authenticated Remote Payload_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthremote_loc-authen)
 * [_4.3.10. REQ.SEC.AUTH.COMPATIBILITY: Authenticated Vendor and Class IDs_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthcompatibility-aut)
 * [_4.3.11. REQ.SEC.RIGHTS: Rights Require Authenticity_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecrights-rights-require)



# Requirements covered by device code in suit-manifest-processor

## Requirements from manifest18[^manifest]:

 * _All payloads must be validated prior to loading._ ([4.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-suit-workflow-model))
 * _All loaded images must be validated prior to execution._ ([4.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-suit-workflow-model))
 * _The manifest processor SHOULD check that the common sequence contains at least one Check Vendor Identifier command and at least one Check Class Identifier command._ ([6.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-required-checks))
 * _Executing an update MUST either result in an error, or a verifiably correct system state._ ([6.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-interpreter-fundamental-pro))
 * _Executing a Trusted Invocation MUST either result in an error, or an invoked image._ ([6.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-interpreter-fundamental-pro))
 * _Executing the same manifest on multiple Recipients MUST result in the same system state._ ([6.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-interpreter-fundamental-pro))
 * _The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. A SUIT_Envelope that has not had authentication information added MUST still contain the suit-authentication-wrapper element, but the content MUST be a list containing only the SUIT_Digest._ ([8.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-authenticated-manifests))

## Requirements from information model[^info_model]:

 * [_4.3.4. REQ.SEC.AUTHENTIC: Cryptographic Authenticity_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthentic-cryptograph)
 * [_4.3.8. REQ.SEC.AUTH.EXEC: Secure Execution_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthexec-secure-execu)
 * [_4.3.21. REQ.SEC.MFST.CONST: Manifest Kept Immutable between Check and Use_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecmfstconst-manifest-ke)



# Requirements covered by platform code

## Requirements from information model[^info_model]:

 * [_4.3.13. REQ.SEC.ACCESS_CONTROL: Access Control_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecaccess_control-access)
 * [_4.3.16. REQ.SEC.REPORTING: Secure Reporting_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecreporting-secure-repo)
 * [_4.3.18. REQ.SEC.KEY.ROTATION: Protected Storage of Signing Keys_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqseckeyrotation-protected)



# Requirements covered by the suit-manifest-processor build system

## Own requirements

 * Regardless of the modifications to the CDDL used to build the suit-mp, or to generate the manifest, the build system should contain a step where the manifest is validated against a (largely) unmodified version of the Full CDDL.

## Requirements from manifest18[^manifest]:

 * _All payloads must be validated prior to loading._ ([4.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-suit-workflow-model))
 * _All loaded images must be validated prior to execution._ ([4.2](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-suit-workflow-model))
 * _The suit-authentication-wrapper MUST come before any element in the SUIT_Envelope, regardless of canonical encoding of CBOR. A SUIT_Envelope that has not had authentication information added MUST still contain the suit-authentication-wrapper element, but the content MUST be a list containing only the SUIT_Digest._ ([8.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-authenticated-manifests))
 * _A SUIT_Envelope that has not had authentication information added MUST still contain the suit-authentication-wrapper element, but the content MUST be a list containing only the SUIT_Digest._ ([8.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-authenticated-manifests))

## Requirements from information model[^info_model]:

 * [_4.3.8. REQ.SEC.AUTH.EXEC: Secure Execution_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecauthexec-secure-execu)
 * [_4.3.15. REQ.SEC.IMG.COMPLETE_DIGEST: Whole Image Digest_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecimgcomplete_digest-wh)



# Requirements covered by user

## Requirements from manifest18[^manifest]:

 * _The author MUST ensure that all parameters consumed by a command are set prior to invoking that command. Where Component Index = True, this means that the parameters consumed by each command MUST have been set for each Component._ ([7](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-creating-manifests))
 * _A signing application MUST verify the suit-manifest element against the SUIT_Digest prior to signing._ ([8.3](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-authenticated-manifests))
 * _All SUIT_Component_Identifiers present in suit-text MUST also be present in suit-common._ ([8.4.4](https://www.ietf.org/archive/id/draft-ietf-suit-manifest-18.html#name-suit-text))

## Requirements from information model[^info_model]:

 * [_4.3.17. REQ.SEC.KEY.PROTECTION: Protected Storage of Signing Keys_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqseckeyprotection-protect)
 * [_4.3.18. REQ.SEC.KEY.ROTATION: Rotation of Signing Keys_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqseckeyrotation-protected)
 * [_4.3.19. REQ.SEC.MFST.CHECK: Validate Manifests prior to Deployment_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecmfstcheck-validate-ma)
 * [_4.3.20. REQ.SEC.MFST.TRUSTED: Construct Manifests in a Trusted Environment_](https://www.rfc-editor.org/rfc/rfc9124.html#name-reqsecmfsttrusted-construct)



# References

[^info_model]: IETF RFC 9124 - A Manifest Information Model for Firmware Updates in Internet of Things (IoT) Devices - https://datatracker.ietf.org/doc/rfc9124/
[^manifest]: IETF draft-ietf-suit-manifest-18 - A Concise Binary Object Representation (CBOR)-based Serialization Format for the Software Updates for Internet of Things (SUIT) Manifest - https://datatracker.ietf.org/doc/html/draft-ietf-suit-manifest-18
