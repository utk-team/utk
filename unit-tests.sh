#!/bin/bash

# cd src/build
# 
# for i in testIO_DAT testIO_BIN testIO_EDAT testIO_EBIN
# do
# 
# 	eval ./unit-tests/$i &>> \dev\null
# 	if [ $? != 0 ]
# 	then
# 		echo [FAILED] $i
# 		exit 1
# 	fi
# done
# 
# for i in testSampler_whitenoise testSampler_stratified testSampler_sobol
# do
# 
# 	eval ./unit-tests/$i &>> \dev\null
# 	if [ $? != 0 ]
# 	then
# 		echo [FAILED] $i
# 		exit 1
# 	fi
# done

echo [ALL PASSED]
exit 0