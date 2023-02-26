#!/bin/bash
program=$1
station_file=$2
node_file=$3
tour_file=$4
path_file=$5
expected_file=$6
tmpfile=$(mktemp)
$program $station_file $node_file $tour_file $path_file > $tmpfile
diff $tmpfile $expected_file
