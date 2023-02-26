#!/bin/bash
program=$1
line_file=$2
expected_file=$3
tmpfile=$(mktemp)
$program $line_file > $tmpfile
diff $tmpfile $expected_file
