#!/bin/bash
program=$1
source_dir=$2
output_dir=$3
mkdir -p $output_dir
$program $source_dir/test/data/station.csv $source_dir/test/data/join.csv $source_dir/test/expected/group.csv $output_dir
diff $output_dir/shortest_path.csv $source_dir/test/expected/shortest_path.csv
diff $output_dir/railway.tsp $source_dir/test/expected/railway.tsp
diff $output_dir/node.csv $source_dir/test/expected/node.csv
