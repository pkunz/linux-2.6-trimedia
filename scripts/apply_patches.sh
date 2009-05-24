#!/bin/sh

patch -p1 < patches/ndk-bslCore.patch
patch -p1 < patches/mdsmbe.patch
patch -p1 < patches/tmbslNull.patch

