#!/bin/bash
program=$1
station_file=$2
expected_file=$3
tmpfile=$(mktemp)
$program $station_file > $tmpfile
diff $tmpfile $expected_file
