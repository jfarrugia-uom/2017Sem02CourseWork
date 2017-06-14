#!/usr/bin/env python

import sys

# input comes from STDIN (standard input)
for line in sys.stdin:
    wordDictionary = {}
    
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split()
    
    # increase counters
    for word in words:
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # pipe-delimited; the trivial word count is 1
        if (wordDictionary.has_key(word)):
            wordDictionary[word] = wordDictionary[word] + 1
        else:
            wordDictionary[word] = 1
        	
    for dictValue in wordDictionary.keys():    	
    	print '%s|%s' % (dictValue, wordDictionary[dictValue])

