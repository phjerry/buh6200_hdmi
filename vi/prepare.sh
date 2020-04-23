#!/bin/bash
##########################################################
serverdate=`date +%Y-%m-%d`
servertime=`date +%H:%M:%S`
echo "---------------------------"
echo $serverdate
echo $servertime
echo "---------------------------"

##########################################################

VER_FILE=version.h
COMMIT_CMD_DATA="git log --date=short |grep "Date" |head -n 1"
COMMIT_CMD_CIID="git log --abbrev-commit |grep "commit" |head -n 1"

DRV_VAR="static unsigned char *g_c_drv_verion ="

# get sdk version last commit
str1=`eval $COMMIT_CMD_DATA`
str2=`eval $COMMIT_CMD_CIID`
DRV_LAST_COMMIT="${str2#* } ${str1#*:}"

echo "/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: vi version
 * Author: sdk
 * Create: 2019-04-13
 */
" > $VER_FILE
echo "#ifndef __VI_VERSION_H_" >> $VER_FILE
echo "#define __VI_VERSION_H_" >> $VER_FILE
echo $DRV_VAR " \"" $DRV_LAST_COMMIT "\";" >> $VER_FILE
echo "#endif" >> $VER_FILE

sed -i 's/\"\ /\"/g' ./version.h
sed -i 's/\ \"/\"/g' ./version.h

pwd

##########################################################

echo "==============$0 finish=============="

