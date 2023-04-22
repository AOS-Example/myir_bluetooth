#!/bin/bash

HTMLDIR=${HTMLDIR:-$HOME/unittest_linux}

if [ ! -d $HTMLDIR ]; then
  mkdir -p $HTMLDIR;
fi

date

make -f Makefile_unittest.linux clean
make -f Makefile_unittest.linux
make_result=$?

if [ $make_result -ne 0 ]; then
    echo ERROR - build failure, check logs above.
    returnval=$make_result

    exit $returnval
fi

PROJECTS="android_nativeservice android_bt_hal android_log NET_Bluetooth libbt_if libbtven libbtosal libbt libbtplat btfilter libbluetooth libbtaudio libbtres libbtvcardparser"
PROJECTS_DISABLED="btservice libbttools"
UNITSUFFIX="_unit"
COVERAGESUFFIX="_coverage.html"

# TODO - check for interactive shell before running web browser:  PS1 / $- / tty -s
isinteractive=0
returnval=0

for nextproj in $PROJECTS; do
    cd $nextproj

    ./$nextproj$UNITSUFFIX
    unittest_result=$?

    coverage_html=$HTMLDIR/$nextproj$COVERAGESUFFIX
    echo NOTE - gcovr cmd:  gcovr -r . -e ".*(mock|generated|unittest|test|unit_test|stubs).*" --html --html-details -o $coverage_html
    gcovr -r . -e ".*(mock|generated|unittest|test|unit_test|stubs).*" --html --html-details -o $coverage_html
    gcovr_result=$?
    if [ $unittest_result -eq 0  ] && [ $gcovr_result -eq 0 ]; then
        echo Unit-test / gcovr SUCCESS!!!

        if [ $isinteractive -ne 0 ]; then
            which google-chrome >/dev/null
            if [ $? -eq 0 ]; then
                google-chrome $coverage_html
            else
                firefox $coverage_html
            fi
        fi
    else
        echo Unit-test or gcovr FAILED!!! - returned $unittest_result / $gcovr_result
        if [ $unittest_result -eq 0 ]; then
            returnval=$gcovr_result
        else
            returnval=$unittest_result
        fi
    fi

    cd ..
done

date

exit $returnval