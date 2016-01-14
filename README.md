This repository contains the source codes of MSP430-Tools which are used by the Android JB for various purposes. Thsere are the available MSP430-Tools.

1. getMSP430Version - Returns the currenly running MSP430 firmware version. Added to system/usr/bin/ in the Android file system
2. getAllFWVersions - Returns firmware versions of all available MSP430 codes. Currently running code is on the first line and other available code is on the second line. Added to system/usr/bin/ in the Android file system
3. cp430_fw_updater - This is the MSP430 firmware updating agent on the Android side. This is bundled with an OTA update. Firmware updating documentation https://github.com/b2gdev/MSP430-CCS/wiki/Firmware-Updating-Documentation

#### Build Instructions

1. Unzip socrcery g++ lite (sourcery_g++_lite.tar.bz2 is provided in the release v1.0)
```
tar xvf sourcery_g++_lite.tar.bz2
```

2. Update path variable
```
export PATH=$PATH:[sourcery_g++_lite.tar.bz2 location]/sourcery_g++_lite/bin
```
3. Then it is possible to
```
make clean
```
and
```
make
```
the binaries.
