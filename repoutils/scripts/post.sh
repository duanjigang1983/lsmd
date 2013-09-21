#!/bin/bash
###by duanjigang <duanjigang1983@gmail.com> 2013-09-21
init_script="/usr/local/repoutils/bin/init_yum.sh"

if [ -f "$init_script" ]
then
	/bin/bash $init_script
fi
