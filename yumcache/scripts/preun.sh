#!/bin/sh

if [ $1 = 0 ] ; then
    /usr/local/bin/svc -dx /service/yumcache
    rm -f /service/yumcache
fi

