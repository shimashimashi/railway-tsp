#!/bin/bash
program=$1
source_dir=$2
tmpfile=$(mktemp)
$program $source_dir/test/data/station.csv $source_dir/test/expected/node.csv $source_dir/test/expected/railway.lkh $source_dir/test/expected/shortest_path.csv > $tmpfile
diff $tmpfile $source_dir/test/expected/tour.json
