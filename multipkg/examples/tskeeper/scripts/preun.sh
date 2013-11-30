#!/bin/sh

if [ $1 = 0 ] ; then
    /usr/local/bin/svc -dx /service/tskeeper
    rm -f /service/tskeeper
fi

