#!/usr/bin/env python
# Import necessary libraries
import cgi
import cgitb
import os


cgitb.enable()

HTML = """
<!DOCTYPE html>
<html>
<head>
	<title>Hello new user !</title>
</head>
<body>
	{}
</body>
</html>
"""

ERROR_HTML = """
<p style="color:red;"> please enter firstame, lastname and address</p>
"""

form = cgi.FieldStorage()

firstname = form.getfirst("firstname","")
lastname = form.getfirst("lastname","")
address = form.getfirst("address","")


# Generate output
def main():
	# Check if the user has entered the required fields
	if not firstname or not lastname or not address:
		body = HTML.format(ERROR_HTML)
	else:
		body = HTML.format("Hello {} {} from {}, enjoy the best quesadillas".format(firstname, lastname, address))
	set_headers(body)
	print(body)										

def print_environment():
    print("Content-Type: text/plain\n")  # Use plain text for output to make it easy to read
    for key, value in os.environ.items():
        print("{}: {}".format(key, value))

def set_headers(content):
	print("Content-Type: text/html \r")
	print("Content-Length: {} \r".format(len(content)))
	print("\r")

if __name__ == "__main__":
	main()
