#!/bin/sh

PATH=/usr/local/sbin:/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
rm -f $DSTDIR/service/pkgaudit
ln -s $DESTDIR/etc/service/pkgaudit $DESTDIR/service/pkgaudit
chown -R nobody $DESTDIR/service/pkgaudit/log/main
