#!/bin/bash
name=`hostname`
tt=`date "+%Y-%m-%d %H:%M:%S"`
title="cmos on $name was uninstalled at $tt"
exec > /tmp/cmos_uninstall.log 2>&1

if [ "$1" = "0" ]; then
        rm /service/tops-cmos || exit 1
        /usr/local/bin/svc -dx /etc/service/tops-cmos /etc/service/tops-cmos/log || exit 1
fi

#if [ -f "/bin/mailx" ]
#then
#	echo $title | /bin/mailx -s "[pkg]$title" newscoo@alibaba-inc.com  -c jigang.djg@taobao.com  || exit 0
#fi
