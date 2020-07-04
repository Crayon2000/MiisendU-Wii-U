#!/bin/bash

# Install required packages
dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wiiu-dev ppc-jansson

# Download and install master version of wut
wget --no-verbose --output-document=/tmp/wut.tar.gz https://github.com/devkitPro/wut/archive/master.tar.gz
tar --extract --gzip --file=/tmp/wut.tar.gz --directory=/tmp
(cd /tmp/wut-master && make install)
rm -r /tmp/wut*
