#!/bin/sh
. /etc/config/defines

version=`scut -p BUILD_VERSION= $layout_config`
build=`scut -p BUILD_NUMBER= $layout_config`
echo "v$version build $build"
