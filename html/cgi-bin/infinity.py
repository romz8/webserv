import cgi
import cgitb

cgitb.enable()

def main():
	x = 1
	while x < 10:
		print("infinity loop madness")

if __name__ == '__main__':
	main()