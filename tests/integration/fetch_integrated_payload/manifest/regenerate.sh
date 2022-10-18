#!/bin/bash

### Key generation ###
if [ ! -f key_private.pem ]; then
  echo "Generating private key..."
  suit-tool keygen -t secp256r1 -o key_private.pem -f pem
fi
if [ ! -f key_public.pem ]; then
  echo "Generating public key..."
  suit-tool pubkey -k key_private.pem -f pem -o key_public.pem
fi
if [ ! -f key_public.c ]; then
  echo "Generating public key as C source file..."
  suit-tool pubkey -k key_private.pem -f uecc -o key_public.c
fi

### Manifest generation ###
echo "Generating SUIT envelope..."
suit-tool create -i sample.json -o sample.suit
suit-tool sign -m sample.suit -k key_private.pem -o sample_signed.suit
zcbor convert -c ../../../../cddl/manifest.cddl -i ./sample_signed.suit --input-as cbor -t SUIT_Envelope_Tagged -o sample_signed.suit.c --c-code-var-name manifest --c-code-columns 8

echo "Done."
