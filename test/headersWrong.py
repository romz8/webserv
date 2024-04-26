# !/usr/bin/python
# coding=utf-8
import socket
from collections import OrderedDict

ip = "localhost"
port = 4242
url = 'http://'+ ip + ":" + str(port)
host = ip + ":" + str(port)


# should be ok (but trimmed) 
	# ("Horizontal Tabs After Value Not Trimmed", "X-Frame-Options: DENY\t\t\r\n"),  # Tabs after value, correctly terminated
# this should be changed : ok to be empt, but content-lenght should be numerical or not empty for POST only
# ("Empty Header Value with Semicolon", "Content-Type:;\r\n"),  # Header value is empty but ends with a semicolon, correctly terminated
# should be fine 
	# ("No Space After Colon", "X-Custom-Header:This is a test value\r\n"),  # No space after colon, correctly terminated

#list of bad headers to test 
bad_headers = OrderedDict([
    ("Missing Colon", "X-Forwarded-For 192.168.1.1\r\n"),  # Missing colon after header name
    ("No CRLF at End", "User-Agent: Mozilla/5.0 Windows NT 10.0; Win64; x64"),
	("Incorrect Line Termination \\n only", "Accept-Language: en-US,en;q=0.5\nUser-Agent: Mozilla/5.0 Windows NT 10.0; Win64; x64\r\n"),  # Intentionally missing \r\n to test incorrect termination
    ("Incorrect Line Termination \\n only", "Accept-Language: en-US,en;q=0.5\n"),  # Incorrectly uses only \n instead of \r\n
    ("Improper Whitespace Before Colon", " Host : example.com\r\n"),  # White spaces before and after colon
    ("Missing Header Name", ": No-Header-Name value\r\n"),  # Colon without header name
    ("Missing Colon and Improper Whitespace Usage", "Set-Cookie \t sessionId=abc123; Path=/;\r\n"),  # Mixed issues: tab and spaces around keys and values, correctly terminated
	("Improper Whitespace Usage", "Set-Cookie \t: sessionId=abc123; Path=/;\r\n"),  # Mixed issues: tab and spaces around keys and values, correctly terminated
	("Improper Whitespace Usage", "Set-Cookie \n: sessionId=abc123; Path=/;\r\n"),  # Mixed issues: tab and spaces around keys and values, correctly terminated
    ("Incorrect CR Usage", "Server: Apache\rApache/2.4.41 (Unix)\r\n"),  # Incorrect usage of CR within the value
    ("Double CR Before LF", "Cache-Control: no-cache\r\r\n"),  # Double CR before LF, testing specific improper CR usage
    ("Missing LF", "Accept-Encoding: gzip, deflate\r"),  # Missing LF, ends with CR only
    ("No Value, Just Whitespace Before CRLF", "Content-Length: \r\n"),  # No value, just whitespace, correctly terminated
    ("Missing Colon", "Authorization Bearer sometoken\r\n"),  # Another example with missing colon
	("Missing Colon", "Authorization Bearer sometoken\r\n"),  # Another example with missing colon
	("Double CR Before LF", "emptynoSF:\r\nCache-Control: no-cache\r\r\n"), # Double CR before LF, testing specific improper CR usage but no sf in key-val
	("Header Value Too Large", "Content-Length: 9999999999999999999999999999999999\r\n"),
	("Header Content-Lenght non-numeric", "Content-type: text/plain\r\nContent-Length: hello\r\n"),  # Excessively large value, correctly terminated
	#("Header Value Too Large by 1byes", "Content-Length: 10001\r\n"),  # Excessively large value, correctly terminated
])


def print_result(response):
	if "400 Bad Request" in response:
		print("\033[32m✅ PASS\033[0m: test return bad request as expected")
		return(1);
	else:
		print("\033[31m❌ FAIL\033[0m: test passed and should have failed")
		return(0);
		
total = 0
total_test = len(bad_headers)
print("======================   TESTING (BAD) HEADER PARSING ON  GET REQUEST =============================")
for test_name, test_header in bad_headers.items():
		request ="GET / HTTP/1.1\r\nHost: {}\r\n{}\r\n".format(host, test_header)
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((ip, port))
			s.sendall(request.encode())
			response = s.recv(4096)
			s.close()
			print("Test: {}".format(test_name))
			total += print_result(response)
		except Exception as e:
			print("\033[31m❌❌❌ FAIL\033[0m: {} - Exception occurred: {}".format(test_name, e))

print("===========================================================================")

if (total == total_test):
	print("\033[32m✅✅✅ All tests passed ✅✅✅\033[0m")
else:
	print("\033[31m total is {} tests over {}\033[0m".format(total, total_test))


print("======================   TESTING (GOOD) HEADER PARSING ON POST REQUEST =============================")


print("======================   TESTING (BAD) HEADER PARSING ON POST REQUEST =============================")

bad_headers = OrderedDict([
	("Empty Header Value with Semicolon", "Content-Type:;\r\n"),  # Header value is empty but ends with a semicolon, correctly terminated
	("Empty Content-Type Value with Semicolon", "Content-Type: text/html;\r\nContent-length: abc\r\n"),  # Header value is empty but ends with a semicolon, correctly terminated
	("Empty Content-Type Value with Semicolon", "Content-Type: text/html;\r\nContent-length:\r\n"),  # Header value is empty but ends with a semicolon, correctly terminated

])