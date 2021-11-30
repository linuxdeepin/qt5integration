#!/bin/bash

set -e
BUILD_DIR=build
REPORT_DIR=report
cd ../
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
qmake ../ CONFIG+=debug BASED_DTK_DIR=based-dtk
make -j$(nproc)
cd ../tests/

rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
qmake ../ CONFIG+=debug BASED_DTK_DIR=based-dtk
export ASAN_OPTIONS=halt_on_error=0
TESTARGS="--gtest_output=xml:dde_test_report_qt5integration.xml" make check -j$(nproc)

lcov -d ./ -c -o coverage_all.info
#lcov --extract coverage_all.info $EXTRACT_ARGS --output-file coverage.info
lcov --remove coverage_all.info "*/tests/*" "*/usr/include*" "*build/src*" --output-file coverage.info
cd ..
genhtml -o $REPORT_DIR $BUILD_DIR/coverage.info

test -e ./build/asan.log* && mv ./build/asan.log* ./build/asan_qt5integration.log || touch ./build/asan.log

#rm -rf $BUILD_DIR
#rm -rf ../$BUILD_DIR
