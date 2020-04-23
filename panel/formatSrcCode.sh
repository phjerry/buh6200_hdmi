#!/bin/bash
#===========formate all the source file================
versionfile=./panel_version.h
definition_type="unsigned char*"
commit_date=`date +g_panel_date=\"%Y-%m-%d,%H:%M:%S\"\;`
serverdate=`date +%Y-%m-%d`
servertime=`date +%H:%M:%S`

function formate_file(){
find . -name "*.c" -exec sed -i 's/\r//' {} \;
find . -name "*.c" | xargs sed -i 's/\t/    /g'
find . -name "*.c" -exec sed -i 's/ *$//g' {} \;
find . -name "*.h" -exec sed -i 's/\r//' {} \;
find . -name "*.h" | xargs sed -i 's/\t/    /g'
find . -name "*.h" -exec sed -i 's/ *$//g' {} \;
find ./ -name "Makefile" |xargs chmod 644
find ./ -name "*.c" |xargs chmod 644
find ./ -name "*.h" |xargs chmod 644
}
#================main==================================
echo "now we formate the soutce file for update...... $serverdate $servertime"
echo "/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: panel panel_version.h
 * Author: sdk
 * Create: 2019-04-13
 */
" > $versionfile

echo "#ifndef __PANEL_VERSION_H__" >> $versionfile
echo "#define __PANEL_VERSION_H__" >> $versionfile
echo $definition_type $commit_date >> $versionfile
echo "#endif" >> $versionfile

sed -i 's/\"\ /\"/g' ./panel_version.h
sed -i 's/\ \"/\"/g' ./panel_version.h
formate_file

##########################################################

echo "==============$0 finish=============="
