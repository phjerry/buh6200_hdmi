#!/bin/sh
##########################################################
serverdate=`date +%Y-%m-%d`
servertime=`date +%H:%M:%S`
echo "---------------------------"
echo $serverdate
echo $servertime
echo "---------------------------"

##########################################################

find . -name "*.c" -exec sed -i 's/\r//' {} \;
find . -name "*.c" | xargs sed -i 's/\t/    /g'
find . -name "*.c" -exec sed -i 's/ *$//g' {} \;

find . -name "*.h" -exec sed -i 's/\r//' {} \;
find . -name "*.h" | xargs sed -i 's/\t/    /g'
find . -name "*.h" -exec sed -i 's/ *$//g' {} \;
##########################################################

find ./ -name "Makefile" |xargs chmod 644
find ./ -name "*.c" |xargs chmod 644
find ./ -name "*.h" |xargs chmod 644

pwd

##########################################################

echo "==============$0 finish=============="

