# Manifest Processor Architecture

The processor consists of two main parts, the core and the platform.

## Core

The core contains all the code that does not have side-effects, which means it mostly decides in which order things happen.
It has high-level control over the processing of the manifest.
The core does the following (often with help from the platform API):

 * Decode all CBOR encoded data.
 * Run all required checks at the appropriate time. This includes:

   * Signature checks
   * Digest checks
   * CBOR structure
   * Other checks mandated in the SUIT spec
   * Conditions present in the command sequences

 * Interpret and run the command sequences in the manifest.

The core is implemented in this repository.
The core is designed to be resistant to fault injection attacks.


## Platform

The platform contains everything that has side-effects, i.e. writing of payloads and metadata, as well as other platform specific operations such as:

 * Cryptographic operations (because the crypto might be hardware accelerated)
 * Resolving of component IDs, which amounts to keeping track of the memory layout
 * Storing and comparing sequence numbers, because this should be hardware-backed to prevent tampering
 * Comparing vendor/class/device IDs, because this needs to be aware of component IDs.
 * Fetching of payloads, since this needs to be performed on whatever protocols are available on the platform.
 * Booting of images, because this is dependent on the architecture of the platform, as well as the OS.

The platform is implemented either in this repository or out-of-tree.
An out-of-tree implementation requires no change to this repository.

A platform must implement the API found in [include/suit_platform.h](include/suit_platform.h)

If a platform action (e.g. suit_plat_fetch) is offloaded to another execution unit (thread/core/chip) the platform implementation can choose to return with the return code SUIT_ERR_WAIT to indicate that the operation should be retried later, typically after receiving a notification from the execution unit performing the operation.


## Storage

### Components

A component can refer to any piece of memory or data.

In the manifest a component is identified by its "component identifier" which is a list of `bstr`s.
The meaning of a component identifier is platform-specific (except temporary components), so the interpretation of component IDs happens in the platform.
The component ID will be passed to the platform API as a single string, in the way it appears in the encoded manifest.
The platform can use zcbor-generated code to interpret the component ID.

__Example__

The component ID can be structured somewhat like a path, e.g.

```
App_FW_Component_ID = ['app', 'fw']
App_FW_Metadata_Component_ID = ['app', 'fw', 'metadata']
Bootloader_FW_Component_ID = ['bootloader', 'fw']

Component_ID //= App_FW_Component_ID
Component_ID //= App_FW_Metadata_Component_ID
Component_ID //= Bootloader_FW_Component_ID
```

#### Properties

`struct suit_component_properties` describes the properties that all components have.

 * level: The level of protection of the underlying storage. Can be e.g. "internal" or "external".
 * mode: The RWX mode of the component. This should be enforced by HW protection if possible.
 * read_size: The size of the current contents of the component.
 * write_size: The capacity of the component. According to REQ.SEC.IMG.COMPLETE_DIGEST[^info_model] digests should cover the entire write_size.
 * address: The address of the start of the current slot of the component.
 * slot: The current slot number. This is used when a component can be stored in one of several locations[^manifest].

Any of the properties can be implicit or immutable.
E.g. an image bank will typically have an immutable mode (RWX), address and write_size (capacity), while the read_size could be retrieved from a header in the firmware residing in the bank.
Or, a temporary component (described below) will typically have a matching read_size and write_size and a RW mode.

### Temporary components

Whenever a manifest needs somewhere arbitrary to put a payload while performing the update, it can request a temporary component.
"Temporary" means the component will live only until the current invocation of the manifest is complete, either

 * During the update (during suit-payload-fetch + suit-install), or
 * During secure boot (during suit-validate + suit-load + suit-invoke)

The manifest can request a temporary component by using the following component ID:

```
Temp_Component_ID_Elems = [
    'temp',
    UID: bstr,
    Size: bstr .cbor uint,
    Align: bstr .cbor uint,
    ?Context: bstr,
]

Component_ID //= Temp_Component_ID
```

I.e. a list of five `bstr`s:

 1. Ascii text spelling "temp".
 2. A unique ID to be able to disambiguate two different temporary components even if the other parameters are equal (could be e.g. a text name or a unique number).
 3. The size of the component as a (CBOR) integer wrapped in a `bstr`.
 4. The required alignment of the component as a (CBOR) integer wrapped in a `bstr`.
 5. An optional platform-specific `bstr` that can be used to differentiate where to place the temporary component.

When fetching into a temporary component, the platform looks for a suitable space to fetch into, using the size to "allocate" the correct amount of storage.
This way, multiple temporary components can be located inside a bigger area without having to separately report the size of each component.
A temporary component will always reside in non-volatile storage, unless otherwise specified by the context.

There is code in the manifest processor repo to implement temporary components, given a suitable memory area.

### Internal storage

Internal storage is defined as non-volatile storage that is as secure as the storage containing the most secure updateable firmware in the system.

Internal storage should be locked with hardware-backed locking mechanisms before the digest is checked, especially when the component will later be booted/executed.
The manifest itself should be write-protected before it is processed.

The platform must be able to temporarily store various state data during an update.

### Internal RAM

Internal RAM used by the manifest processor will be partially protected from fault injection, especially the data in `struct suit_processor_state` and `struct suit_manifest_params` since these largely store the outcome of cryptographic operations and other security-critical checks, or the data protected by such tests.
All pointers expected to point into the stored manifest should be checked to ensure they are pointing within the correct memory area.

The internal RAM should be wiped or RWX-protected before invoking other firmware.

### External storage

External storage is any non-volatile storage that is not internal storage.
This includes e.g. off-chip external flash.

It's possible to provide the code for interfacing with external storage as part of the update.
For example, the code can be copied into RAM and executed by the manifest.
The code should allow the manifest processor to retrieve arbitrary chunks into internal RAM.

When copying into internal storage, this will likely happen in chunks small enough to fit in internal RAM.
The platform should check each fragment's digest before writing it.
This means tampering or corruption can be uncovered earlier, and before writing corrupt data into internal storage.
The following custom parameter and directive can be used:

```
SUIT_Chunk_Digests = [
    chunk-size: uint,
    + SUIT_Digest
]
suit-parameter-custom-chunk-digests = -10
SUIT_Parameters //= (suit-parameter-custom-chunk-digests
    => bstr .cbor SUIT_Chunk_Digests)

suit-directive-custom-copy-chunks = -10
SUIT_Directive //= (suit-directive-custom-copy-chunks, SUIT_Rep_Policy)
```

`suit-directive-custom-copy-chunks` does the same as `suit-directive-copy`, and in addition, it checks the digest of each chunk according to the digests in `suit-parameter-custom-chunk-digests`.
Each digest in `SUIT_Chunk_Digests` is a digest over a fragment of size `chunk-size`.
The exception is the last digest, which, if the total size is not divisible by `chunk-size`, is a digest over a smaller chunk.
E.g. if `chunk-size` is 1024 bytes, and the total size is 3500 bytes, the digest list must contain 4 digests.
The first digest is over bytes 0-1023, the second over 1024-2047, the third over 2048-3071, the fourth over 3072-3499.

### Manifest(s)

The manifest or manifests themselves must be securely stored while being processed.
They must be write-protected before being checked for validity and until they are completely processed (REQ.SEC.MFST.CONST[^info_model]).
This means that they must be stored in __internal storage__.


## References

[^info_model]: IETF RFC 9124 - A Manifest Information Model for Firmware Updates in Internet of Things (IoT) Devices - https://datatracker.ietf.org/doc/rfc9124/
[^manifest]: IETF draft-ietf-suit-manifest-21 - A Concise Binary Object Representation (CBOR)-based Serialization Format for the Software Updates for Internet of Things (SUIT) Manifest - https://datatracker.ietf.org/doc/html/draft-ietf-suit-manifest-21
