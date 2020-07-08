# input file argument : sboot.bin - G950FXXU1AQJ5 - sha1sum: 648a3e2c4de149250c575b4f14de096e147cc799
dd if=$1 of=$1.1.bin skip=0 bs=512 count=16		# fwbl1 0x2000 @ 0x0
dd if=$1 of=$1.2.bin skip=16 bs=512 count=320	# bl31 0x28000 @ 0x2000
dd if=$1 of=$1.3.bin skip=336 bs=512 count=384  # bl2 0x30000 @ 0x2A000
cp $1.1.bin $1.4.bin # i have no idea what im doing. stolen from https://github.com/astarasikov/exynos9610-usb-emergency-recovery/blob/c4d33b943a9d59e69613c796afdfc38b4057a6a9/dltool/dltool.c#L315
dd if=$1 of=$1.5.bin skip=912 bs=512 count=1672
dd if=$1 of=$1.6.bin skip=2584 bs=512 count=1024