#!/bin/bash

for file in ../data/*.txt; do  
  echo Processing $file... 

  cat $file | ./GoTPairsMapper.py >> mapper.out
  
done

