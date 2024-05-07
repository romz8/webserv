#!/usr/bin/env python3

import cgi
import cgitb
import http.cookies

cgitb.enable()

# Get the CGI data
form = cgi.FieldStorage()

# Create a new cookie
cookie = http.cookies.SimpleCookie()

# Check if the 'name' parameter is provided
username = form.getfirst('username', '')
if username:
    # Set the cookie value
    cookie['username'] = username

    # Set cookie expiration time (in seconds)
    cookie['username']['expires'] = 3600  # Cookie expires in 1 hour

# Set the Content-Type header to 'text/html'
print("Content-Type: text/html")
print(cookie)
print()

# Print the HTML content
print("<html>")
print("<head>")
print("<title>Cookie Example</title>")
print("</head>")
print("<body>")
print("<h1>Cookie Example</h1>")

# Print the cookie value if it exists
if 'username' in cookie:
    print("<p>Welcome back, {}!</p>".format(username))
else:
    print("<p>Please enter your name:</p>")
    print('<form method="post" action="">')
    print('<input type="text" name="name" required>')
    print('<input type="submit" value="Submit">')
    print('</form>')

print("</body>")
print("</html>")
print()
print()

def set_cookie(username):
	cookie = http.cookies.SimpleCookie()
	cookie['username'] = username
	if expires:
		cookie[name]['expires'] = expires
	print(cookie)