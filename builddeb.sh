#!/bin/bash

cd service
make
cd -
dpkg-buildpackage -us -uc
