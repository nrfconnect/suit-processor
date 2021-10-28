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
 * Fetching of payloads, since this needs to be performed on whatever protcols are available on the platform.
 * Booting of images, because this is dependent on the architecture of the platform, as well as the OS.

The platform is implemented either in this repository or out-of-tree.
An out-of-tree implementation requires no change to this repository.

A platform must implement the API found in [include/suit_platform.h](include/suit_platform.h)

If a platform action (e.g. suit_plat_fetch) is offloaded to another execution unit (thread/core/chip) the platform implementation can choose to return with the return code SUIT_ERR_WAIT to indicate that the operation should be retried later, typically after receiving a notification from the execution unit performing the operation.
