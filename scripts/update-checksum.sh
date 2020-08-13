#!/bin/bash
# update checksum of BL1 / BL31 bootloaders

SUM=$(dd if=$1 bs=1 skip=16 | sha256sum | xxd -r -p | head -c 4)

echo -n $SUM | dd of=$1 bs=4 count=1 seek=1 conv=notrunc
