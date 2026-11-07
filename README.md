# RK3568 SCSI/UFS NAND Fault Injection Simulator
Project for JiangboLong IC driver interview.

## Hardware
- ATK-DLRK3568
- IMX415 Camera
- USB-TTL HW597B

## Features
- NAND simulator: page/block management, bad block, ECC error, bit flip fault injection
- Lightweight SCSI target state machine: TestUnitReady / Inquiry / Read10 / Write10
- V4L2 video capture, save YUV frame into simulated NAND
- Fault tolerance strategy for video recording
- Qt GUI: local preview, fault inject control

## Build
```shell
make clean
make
