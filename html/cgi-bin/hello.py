#!/usr/bin/env python
# Import necessary libraries
import cgi
import cgitb
import os


cgitb.enable()
# Get the query string from the environment variable

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
firstname = form.getvalue("firstname")
lastname = form.getvalue("lastname")
address = form.getvalue("address")


# Generate output
def main():
	# Check if the user has entered the required fields
	if not firstname or not lastname or not address:
		print(HTML.format(ERROR_HTML))
	else:
		print(HTML.format("Hello {} {} from {}, enjoy the best quesadillas".format(firstname, lastname, address)))
											
def print_environment():
    print("Content-Type: text/plain\n")  # Use plain text for output to make it easy to read
    for key, value in os.environ.items():
        print("{}: {}".format(key, value))

if __name__ == "__main__":
	main()
