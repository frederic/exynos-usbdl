# [exynos-usbdl](https://github.com/frederic/exynos-usbdl) : unsigned code loader for Exynos bootrom

## Disclaimer
You will be solely responsible for any damage caused to your hardware/software/warranty/data/cat/etc...

## Description
Exynos bootrom supports booting from USB. This method of boot requires an USB host to send a signed bootloader to the bootrom via USB port.

This tool exploits a [vulnerability](https://fredericb.info/2020/06/exynos-usbdl-unsigned-code-loader-for-exynos-bootrom.html) in the USB download mode to load and run unsigned code in Secure World.

## Supported targets
* Exynos 8890
* Exynos 8895

## Access to USB download mode
Among all the booting methods supported by this chipset, two are configured in fuses to be attempted on cold boot.
If the first boot method fails (including failed authentication), then second boot method is attempted.

On retail smartphones, the usual boot configuration is internal storage (UFS chip) first, then USB download mode as fallback.
This means **USB download mode is only accessible if first boot method has failed**.

First boot method sabotage can be achieved either through software or hardware modification.
The approach used for this project was to dig a hole in UFS chip with dental scraper.

For Exynos 8890, @astarasikov shared a better approach to corrupt bootloader without opening the device : from Download mode, flash *cm.bin* (from a firmware image) onto *BOOTLOADER* partition using [Heimdall tool](https://gitlab.com/BenjaminDobell/Heimdall). Please keep in mind that it will brick your device until you restore the *BOOTLOADER* partition.

Once the first boot method fails, USB download mode can be accessed by pressing and holding power button.

## Usage
```
./exynos-usbdl <mode> <input_file> [<output_file>]
	mode: mode of operation
		n: normal
		e: exploit
	input_file: payload binary to load and execute
	output_file: file to write data returned by payload (exploit mode only)
```

## Payloads
Payloads are raw binary AArch64 executables. Some are provided in directory **payloads/**.

## License
Please see [LICENSE](/LICENSE).
