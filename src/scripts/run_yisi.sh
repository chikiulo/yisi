#!/bin/bash

# @file run_yisi.sh
# @brief Run YiSi, given a config file.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

################

YISIBIN=/path/to/your/yisi/bin

# Check if YiSi is on the PATH.
if [[ $(which yisi 2> /dev/null) ]]; then
   YISIBIN_PFX=""
else
   YISIBIN_PFX="$YISIBIN/"
fi

TESTCODE=`${YISIBIN_PFX}testbin`
if [[ $TESTCODE != "0" ]]; then
    echo "ERROR: fail to run YiSi test program. Exiting..."
    exit 1
fi

yisiflags=`${YISIBIN_PFX}resolve_yisicmd.sh $1`
echo "Running: ${YISIBIN_PFX}yisi $yisiflags"
# date +%s > $docscorefile.bt 
${YISIBIN_PFX}yisi $yisiflags
# date +%s > $docscorefile.et
