#!/bin/bash
program=$1
station_file=$2
join_file=$3
group_file=$4
output_dir=$5
expected_shortest_path=$6
expected_tsp=$7
expected_node=$8
mkdir -p $5
$program $station_file $join_file $group_file $output_dir
diff $output_dir/shortest_path.csv $expected_shortest_path
diff $output_dir/railway.tsp $expected_tsp
diff $output_dir/node.csv $expected_node
