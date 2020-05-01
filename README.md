# [exynos-usbdl](https://github.com/frederic/exynos-usbdl) : unsigned code loader for USB mode of Exynos Bootrom

## Disclaimer
You will be solely responsible for any damage caused to your hardware/software/warranty/data/cat/etc...

## Description
Exynos bootrom supports booting from USB. This method of boot requires an USB host to send a signed bootloader to the bootrom via USB port.

This tool exploits a [vulnerability](https://fredericb.info/) in the USB download mode to load and run unsigned code in Secure World.

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

Once first boot method fails, USB download mode can be accessed by pressing and holding power button.

## Usage
```
./exynos-usbdl <payloads/anything.bin>
```

## Payloads
Payloads are raw binary AArch64 executables. Some are provided in directory **payloads/**.