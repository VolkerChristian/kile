#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

./createPackage.sh --svnroot "https://svn.kde.org/home/kde" -ab trunk/extragear/office -a kile \
--notoplevel --package --logfile $0.log --noi18n -av $1 --tag
