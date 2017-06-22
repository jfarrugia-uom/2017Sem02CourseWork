#!/bin/bash

prefix="pdf/";
for file in pdf/*.pdf; do  
  echo Processing $file...
  output_file="${file%.pdf}.txt"    
  output_file=${output_file#$prefix}; #Remove prefix  
  #echo $output_file
  gs -sDEVICE=txtwrite -o $output_file $file
done

