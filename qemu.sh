#!/bin/bash
qemu-system-x86_64 -L . -m 64 -M pc -drive file=target/Disk.img,format=raw,index=0,media=disk
