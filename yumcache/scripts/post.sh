#!/bin/sh

PATH=/usr/local/sbin:/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin

# start supervising!
rm -f $DSTDIR/service/yumcache
ln -sf $DSTDIR/etc/service/yumcache $DSTDIR/service/yumcache
INIT_SCRIPT="/usr/local/yumcache/bin/init_cache.sh"
$INIT_SCRIPT
