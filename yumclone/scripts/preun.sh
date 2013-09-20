#!/bin/sh

if [ $1 = 0 ] ; then
    /usr/local/bin/svc -dx /service/yumclone
    rm -f /service/yumclone
fi

