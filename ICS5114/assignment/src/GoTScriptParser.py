#!/usr/bin/env python

import sys
import re
from collections import Counter
from itertools import combinations
from itertools import permutations

def checkForCharacter(line):
	returnStr = ""
	# if line contains one, UPPER CASE word with no period then it's probably a character
	if (len(line.split()) <= 2 and re.match(r'^[^.\[]+$', line) and line.isupper()):		
		returnStr = re.sub(r'[^A-Z ]', '', line)
	else:
		returnStr = ""
	return returnStr

def checkForSceneChange(line):
	sceneChange = False	
	if (re.match(r'(EXT[\.]* +)|(INT[\.]* +)', line)):		
		sceneChange = True
	return sceneChange
	
def checkForScriptLine(line):
	newLine = ""
	if (not line.startswith("Daily Transcripts") and not line.isupper()):
		newLine = line
	return newLine

def checkForEpisode(line):
	episode = 0
	epMatch = re.match(r'.*episode.+\(([0-9]+)\)', line, re.IGNORECASE)
	if epMatch:
		episode = epMatch.group(1)
	return int(episode)


def main():
	sceneCount = 0
	newScene = False
	
	lines = 0
	episode = 0
	scenes = []
	charsInScene = []
	DIALOGUE_DISTANCE = 2
	characterPair = {}
	
	for line in sys.stdin:
		line = line.strip()	
		
		if (episode == 0):
			episode = checkForEpisode(line)	
					
		sceneChange = checkForSceneChange(line)
		if (sceneChange):
			newScene = True			
			
		character = checkForCharacter(line)		
		if character:
			if (sceneCount > 0):
				print "Line count: %d" %lines
				
			if (newScene):
				sceneCount += 1
				print "========> Episode %d; New Scene %d" %(episode, sceneCount)
				if (len(charsInScene) > 1):
					scenes.append(charsInScene)
				charsInScene = []
				newScene = False

			print character
			charsInScene.append(character)
			lines = 0
		
		scriptLine = checkForScriptLine(line)
		if (scriptLine):
			#print scriptLine
			lines += 1
	# print last last count after end of loop
	print "Line count: %d" %lines
	# append last scene to list of scenes
	if (len(charsInScene) > 1):
		scenes.append(charsInScene)
	
	print("===========================> OK, we're breaking it up now")
	print len(scenes)

	for scene in scenes:
		print scene	
		aggLines = Counter(scene)			
		print aggLines
		for pair in combinations(sorted(aggLines.iteritems()),2):
			print "%s\t%s\t%d" %(pair[0][0], pair[1][0], pair[0][1] * pair[1][1])
				
			
if (__name__== "__main__"):
	main()	
    	    
    # remove leading and trailing whitespace    
    #line = line.strip()
    # split the line into words
    #words = line.split()
    # increase counters
    #for word in words:
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # tab-delimited; the trivial word count is 1
       # print '%s\t%s' % (word, 1)


