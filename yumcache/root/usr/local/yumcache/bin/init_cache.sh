#!/bin/bash

#****************************************************************#
# ScriptName: init_cache.sh
# Author: duanjigang1983@gmail.com
# Create Date: 2013-09-22 07:41
# Modify Author: duanjigang1983@gmail.com
# Modify Date: 2013-09-22 07:41
# Function:init config and dirs for yumcache node
#***************************************************************#
srcfile="/usr/local/yumcache/conf/squid.conf.template"

function get_var_value()
{
	var_name=$1
	conf_dir="/usr/local/yumcache/conf"
	#conf_dir="../conf"
	conf_file="$conf_dir/$var_name"
	if ! [  -f "$conf_file" ]
	then
		echo "NONE"
		return
	fi
	res=`head -1 $conf_file`
	if ! [ -z $res ]
	then
		echo  "$res"
	else
		echo "NONE"
	fi
}
function gen_conf()
{
	disk_cache_size=`get_var_value disk_cache_size`
	domain_name=`get_var_value domain_name`
	listen_port=`get_var_value listen_port`
	max_obj_size=`get_var_value max_obj_size`
	mem_cache_size=`get_var_value mem_cache_size`
	origin_port=`get_var_value origin_port`
	origin_site=`get_var_value origin_site`
	run_now=`get_var_value run_now`
	store_dir=`get_var_value store_dir`
	visible_name=`get_var_value visible_name`
	eth=`get_var_value eth`
	ipaddr=`ifconfig $eth | grep "inet addr" | awk '{print $2}' | awk -F ':' '{print $2}'`
	echo "### configure file by duanjigang <duanjigang1983@gmail.com> `date`"
	while read line; do eval echo $line; done < $srcfile
}
dstfile="/etc/squid/squid.conf"
if [ -f "$dstfile" ]
then
	ts=`date +%s`
	newfile="$dstfile".$ts
	mv $dstfile $newfile
fi
gen_conf > $dstfile

store_dir=`get_var_value store_dir`
if ! [ -d "$store_dir" ]
then
		mkdir -p $store_dir
fi
mkdir -p $store_dir/log
mkdir -p $store_dir/cache
mkdir -p $store_dir/core
chown -R squid:squid $store_dir
chmod -R 750 $store_dir
exe=`which squid`
if ! [ -z "$exe" ]
then
	$exe -z
fi
