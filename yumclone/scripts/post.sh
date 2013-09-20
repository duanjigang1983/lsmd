#!/bin/sh

PATH=/usr/local/sbin:/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
rm -f $DSTDIR/service/yumclone
ln -s $DESTDIR/etc/service/yumclone $DESTDIR/service/yumclone
chown -R nobody $DESTDIR/service/yumclone/log/main
