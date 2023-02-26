#!/bin/bash
program=$1
station_file=$2
join_file=$3
expected_file=$4
tmpfile=$(mktemp)
$program $station_file $join_file > $tmpfile
diff $tmpfile $expected_file
