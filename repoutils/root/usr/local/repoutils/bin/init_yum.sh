#!/bin/sh

conf="/usr/local/repoutils/conf/repoutils.conf"
if ! [ -f "$conf" ]
then
	echo "can not find file '$conf'"
	exit 1
fi
domain=`cat $conf  | awk -F '=' '{if($1=="domainname")print $2}'`
company=`cat $conf  | awk -F '=' '{if($1=="company")print $2}'`

if  [ -z $domain ] || [ -z $company ]
then
		echo "invalid domain name or company name in file '$conf'"
		exit 1
fi


#generate for develop

bin="/usr/local/repoutils/bin/genrepo_dev"

if [ -f "$bin" ]
then
	if [ -f "/etc/yum.repos.d/develop.repo" ]
	then
		ts=`date +%s`
		/bin/mv "/etc/yum.repos.d/develop.repo" "/usr/local/repoutils/back/develop.repo".$ts
	fi
	$bin $domain $company
fi

#generate for extras

bin="/usr/local/repoutils/bin/genrepo_extras"

if [ -f "$bin" ]
then
	if [ -f "/etc/yum.repos.d/extras.repo" ]
	then
		ts=`date +%s`
		/bin/mv "/etc/yum.repos.d/extras.repo" "/usr/local/repoutils/back/extras.repo".$ts
	fi
	$bin $domain 
fi

#generate for redhat

bin="/usr/local/repoutils/bin/genrepo_rhel"

if [ -f "$bin" ]
then
	if [ -f "/etc/yum.repos.d/RHEL.repo" ]
	then
		ts=`date +%s`
		/bin/mv "/etc/yum.repos.d/RHEL.repo" "/usr/local/repoutils/back/RHEL.repo".$ts
	fi
	$bin $domain
fi
