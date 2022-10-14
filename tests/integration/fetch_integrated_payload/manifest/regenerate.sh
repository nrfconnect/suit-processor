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
./generate_ctab.py sample_signed.suit

echo "Done."