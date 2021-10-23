#!/bin/bash

# Install / update required packages
if [ -z ${GITHUB_WORKSPACE} ]; then
    dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wiiu-dev ppc-jansson
fi

# Download and install master version of wut
wget https://github.com/devkitPro/wut/archive/master.tar.gz -O - | tar --extract --gzip --verbose --directory=/tmp/
(cd /tmp/wut-* && make install)
rm -rf /tmp/wut-*
