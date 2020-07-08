# input file argument : sboot.bin - G930W8VLS6CSH1 - sha1sum: 9322ccb4e9b382b8cc67ff9ef989c459a763621f
dd if=$1 of=$1.1.bin skip=0 bs=512 count=16 # 0x2000 @ 0x0
dd if=$1 of=$1.2.bin skip=16 bs=512 count=288 # 0x24000 @ 0x2000
dd if=$1 of=$1.3.bin skip=155648 bs=1 count=158992 # 0x26d10 @ 0x26000
dd if=$1 of=$1.4.bin skip=776 bs=512 count=1672 # 0xD1000 @ 0x61000
