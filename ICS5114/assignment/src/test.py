#!/usr/bin/env python

import sys
import string

# iterate over every line of input
for line in sys.stdin:
	# remove white space characters
	line = line.strip()
	# tokenise line using tab as separator
	splits = line.split("\t")
	# first element is our "facebook user"
	for index in range(1, len(splits)):
		# for each friend pairs, get order pair
		# ex. [B, A, D, E] -> [A, B], [B, D], [B, E]
		# perform pairwise comparison and sort accordingly
		fList = sorted([splits[0], splits[index]])
		# append friends to this initial pairs
		# ex. [A, B, A, D, E], [B, D, A, D, E]...
		for index2 in range(1, len(splits)):
			fList.append(splits[index2])
		# finally emit list, separated by tab
		print("\t".join(fList))


