#!/bin/sh

PATH=/usr/local/sbin:/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
rm -f $DSTDIR/service/tskeeper
ln -s $DESTDIR/etc/service/tskeeper $DESTDIR/service/tskeeper
#chown -R nobody $DESTDIR/service/tskeeper/log/main
