#!/bin/sh

./exynos-usbdl e payloads/Exynos8890_unsecure_boot_usb.bin
sleep 1
./exynos-usbdl n $1.1.bin
sleep 1
./exynos-usbdl n $1.2.bin
sleep 1
./exynos-usbdl n $1.3.bin
sleep 1
./exynos-usbdl n $1.4.bin