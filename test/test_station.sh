#!/bin/bash
program=$1
source_dir=$2
tmpfile=$(mktemp)
$program $source_dir/test/data/station.csv > $tmpfile
diff $tmpfile $source_dir/test/expected/station.json
