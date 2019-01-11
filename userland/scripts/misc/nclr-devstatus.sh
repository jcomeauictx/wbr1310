#!/bin/sh

[ "$1" != "" ] && sleep "$1"
rgdb -i -s "/runtime/dev_status" ""