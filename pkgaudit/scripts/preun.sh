#!/bin/sh

if [ $1 = 0 ] ; then
    /usr/local/bin/svc -dx /service/pkgaudit
    rm -f /service/pkgaudit
fi

