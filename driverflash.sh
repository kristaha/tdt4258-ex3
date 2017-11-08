#!/bin/bash
ptxdist targetinstall driver-gamepad
ptxdist targetinstall kernel
ptxdist image root.romfs
ptxdist test flash-rootfs
