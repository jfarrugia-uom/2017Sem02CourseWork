#!/usr/bin/env python

import sys
import re
from collections import Counter
from itertools import combinations


# Function that checks whether line indicates change of character
def checkForCharacter(line):
	returnStr = ""
	# if line contains one, UPPER CASE word with no period then it's probably a character
	if (len(line.split()) <= 2 and re.match(r'^[^.\[]+$', line) and line.isupper()):		
		returnStr = re.sub(r'[^A-Z ]', '', line)
	else:
		returnStr = ""
	return returnStr

# Function that checks whether line indicates change of scene by detecting INT/EXT instructions
# which is code for a new indoor or outdoor setting
def checkForSceneChange(line):
	sceneChange = False	
	if (re.match(r'(EXT[\.]* +)|(INT[\.]* +)', line)):		
		sceneChange = True
	return sceneChange
	
# If the line is skipped by previous two functions, then it's probably a spoken line;
# this is not precise for that there set instructions, and general comments that are not necessarily
# discernible from normal lines.  The assumption is these instructions are evenly distributed throughout
# the script so additional bogus lines will not be added to a specific character
def checkForScriptLine(line):
	newLine = ""
	if (not line.startswith("Daily Transcripts") and not line.isupper()):
		newLine = line
	return newLine

# Episode is also parsed
def checkForEpisode(line):
	episode = 0
	epMatch = re.match(r'.*episode.+\(([0-9]+)\)', line, re.IGNORECASE)
	if epMatch:
		episode = epMatch.group(1)
	return int(episode)


def main():
	# Scene counter
	sceneCount = 0
	# Flag indicating whether a new scene was detected
	newScene = False
	# Number of lines spoken by a character in a scene
	lines = 0
	# episode counter
	episode = 0
	# list of characters in scene based on frequency of their appearance in scene
	charsInScene = []
	#  list of lists where each element is a collection of characters in a scene	
	scenes = []

	
	for line in sys.stdin:
		# remove white space
		line = line.strip()	
		# set episode if not yet set
		if (episode == 0):
			episode = checkForEpisode(line)	
		# if new scene detected, set flag					
		sceneChange = checkForSceneChange(line)
		if (sceneChange):
			newScene = True					
		character = checkForCharacter(line)		
		# new character speaking
		if character:
			#if (sceneCount > 0):
				#print "Line count: %d" %lines
			# if first character of scene add character collection of previous scene to scene collection	
			if (newScene):
				sceneCount += 1
				#print "========> Episode %d; New Scene %d" %(episode, sceneCount)
				if (len(charsInScene) > 1):
					scenes.append(charsInScene)
				# reset current scene
				charsInScene = []
				newScene = False

			#print character
			# add character to character in scene collection
			charsInScene.append(character)
			# reset lines
			lines = 0
		
		scriptLine = checkForScriptLine(line)
		if (scriptLine):
			#print scriptLine
			lines += 1
	# print last last count after end of loop
	#print "Line count: %d" %lines

	# append last scene to list of scenes	
	if (len(charsInScene) > 1):
		scenes.append(charsInScene)
	
	# for every scene: i) aggregate character mentions in scene
	#				  ii) generate combinations of sorted pairs of characters
	# 				 iii) emit combinatation of character and product of scene mentiones
	# product of screen mentions serve as a weight to quantify relative screen time by the character
	# with respect to others.  Therefore, a servant appearing once in an intense scene between two prominent
	# characters will not be given the same importance  simply based on appearance
	for scene in scenes:
		#print scene	
		aggLines = Counter(scene)			
		#print aggLines
		for pair in combinations(sorted(aggLines.iteritems()),2):
			print "%s\t%s\t%d" %(pair[0][0], pair[1][0], pair[0][1] * pair[1][1])
				
			
if (__name__== "__main__"):
	main()	
    	    



