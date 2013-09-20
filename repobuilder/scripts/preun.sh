#!/bin/bash
if [ "$1" = "0" ]; then
        rm /service/repobuilder || exit 1
        /usr/local/bin/svc -dx /etc/service/repobuilder /etc/service/repobuilder/log || exit 1
fi

