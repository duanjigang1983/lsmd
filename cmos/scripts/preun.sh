#!/bin/bash
exec > /tmp/cmos_uninstall.log 2>&1
if [ "$1" = "0" ]; then
        rm /service/cmos || exit 1
        /usr/local/bin/svc -dx /etc/service/cmos /etc/service/cmos/log || exit 1
fi

