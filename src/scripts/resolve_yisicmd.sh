#!/bin/bash

# @file resolve_yisicmd.sh
# @brief Convert YiSi config file to YiSi command arguments.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

source <(cat $1 | sed -e 's/-\(.*=\)/\1/g;')

if [[ $reffile != "" ]]; then
    yisiflags="$yisiflags --ref-file $reffile"
fi

if [[ $hypfile != "" ]]; then
    yisiflags="$yisiflags --hyp-file $hypfile"
fi

if [[ $inpfile != "" ]]; then
    yisiflags="$yisiflags --inp-file $inpfile"
fi

if [[ $sntscorefile != "" ]]; then
    yisiflags="$yisiflags --sntscore-file $sntscorefile"
fi

if [[ $docscorefile != "" ]]; then
    yisiflags="$yisiflags --docscore-file $docscorefile"
fi

if [[ $outsrltype != "" ]]; then
    yisiflags="$yisiflags --outsrl-type $outsrltype"
fi

if [[ $outsrlpath != "" ]]; then
    yisiflags="$yisiflags --outsrl-path $outsrlpath"
fi

if [[ $inpsrltype != "" ]]; then
    yisiflags="$yisiflags --inpsrl-type $inpsrltype"
fi

if [[ $inpsrlpath != "" ]]; then
    yisiflags="$yisiflags --inpsrl-path $inpsrlpath"
fi

if [[ $labelconfig != "" ]]; then
    yisiflags="$yisiflags --labelconfig-path $labelconfig"
fi

if [[ $weightconfig != "" ]]; then
    yisiflags="$yisiflags --weightconfig-path $weightconfig"
fi

if [[ $frameweighttype != "" ]]; then
    yisiflags="$yisiflags --frameweight-type $frameweighttype"
fi

if [[ $lexsimtype != "" ]]; then
    yisiflags="$yisiflags --lexsim-type $lexsimtype"
fi

if [[ $inplexsimpath != "" ]]; then
    yisiflags="$yisiflags --inplexsim-path $inplexsimpath"
fi

if [[ $outlexsimpath != "" ]]; then
    yisiflags="$yisiflags --outlexsim-path $outlexsimpath"
fi

if [[ $reflexweighttype != "" ]]; then
    yisiflags="$yisiflags --reflexweight-type $reflexweighttype"
fi

if [[ $reflexweightpath != "" ]]; then
    yisiflags="$yisiflags --reflexweight-path $reflexweightpath"
fi

if [[ $hyplexweighttype != "" ]]; then
    yisiflags="$yisiflags --hyplexweight-type $hyplexweighttype"
fi

if [[ $hyplexweightpath != "" ]]; then
    yisiflags="$yisiflags --hyplexweight-path $hyplexweightpath"
fi

if [[ $inplexweighttype != "" ]]; then
    yisiflags="$yisiflags --inplexweight-type $inplexweighttype"
fi

if [[ $inplexweightpath != "" ]]; then
    yisiflags="$yisiflags --inplexweight-path $inplexweightpath"
fi

if [[ $phrasesimtype != "" ]]; then
    yisiflags="$yisiflags --phrasesim-type $phrasesimtype"
fi

if [[ $alpha != "" ]]; then
    yisiflags="$yisiflags --alpha $alpha"
fi

if [[ $beta != "" ]]; then
    yisiflags="$yisiflags --beta $beta"
fi

if [[ $mode != "" ]]; then
    yisiflags="$yisiflags --mode $mode"
fi

if [[ $ngramsize != "" ]]; then
    yisiflags="$yisiflags --ngram-size $ngramsize"
elif [[ $n != "" ]]; then
    yisiflags="$yisiflags --n $n"
fi

echo "$yisiflags" | sed "s/^ //"
