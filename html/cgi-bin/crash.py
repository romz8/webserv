#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()

def main():
	arr = [0, 1]
	print(arr[2])

if __name__ == '__main__':
	main()	
	
	