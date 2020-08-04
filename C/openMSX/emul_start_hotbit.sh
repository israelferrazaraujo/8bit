#!/bin/bash	
# OpenMSX Start Script
# The emulator will be started only if it's not already active


xopenmsx=`ps x | grep "openmsx " | grep -v grep`
 
if [ "${xopenmsx}" == "" ]; then
  echo "...Now Starting OpenMsx "
  openmsx -script ../../openMSX/emul_start_config_hotbit.txt
else
  echo "...openMSX already running "
fi
