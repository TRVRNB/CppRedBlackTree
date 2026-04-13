"""
This Python script will generate a file (numbers.txt) that can be used in main.cpp
"""
import sys, random

sys.stdout = open("numbers.txt", "w") # print directly to numbers.txt
data = ""


for i in range(100):
	# generate 100 numbers
	num = random.randint(1, 999)
	data += str(num) # add this to the line
	if i != 99: # add space if it's not the last number
		data += " "

print(data)
