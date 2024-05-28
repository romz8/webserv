# !/usr/bin/python
# coding=utf-8
import socket
from collections import OrderedDict

ip = "localhost"
port = 4242
url = 'http://'+ ip + ":" + str(port)
host = ip + ":" + str(port)


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
	("Header Value Too Large", "Content-Length: 9999999999999999999999999999999999\r\n"), # Excessively large value, correctly terminated
	("Header Content-Lenght non-numeric", "Content-type: text/plain\r\nContent-Length: hello\r\n") 
])

Error_headers = OrderedDict([
	
		("Header Value Too Large by 1byes", "Content-Length: 100000001\r\n"),  # Excessively large value, correctly terminated
		("Infinite Loop waiting for Body", "Content-type: text/plain\r\nContent-Length: 100\r\n"),  # infinite loop waiting for body bytes

])

def print_result(response, expected_response):
	if expected_response in response:
		print("\033[32m✅ PASS\033[0m: test returned " + expected_response +" as expected")
		return(1);
	elif "200" in response:
		print("\033[31m❌❌ FAIL\033[0m: test passed and should have failed")
	else :
		print("\033[31m❌ FAIL\033[0m: test failed but not expected response :" + response.decode())
		return(0);
		
total = 0
total_test = len(bad_headers)
print("==================================================================================================")
print("======================   TESTING (BAD) HEADER PARSING ON GET REQUEST =============================")
print("==================================================================================================")
for test_name, test_header in bad_headers.items():
		request ="GET / HTTP/1.1\r\nHost: {}\r\n{}\r\n".format(host, test_header)
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((ip, port))
			s.sendall(request.encode())
			response = s.recv(4096)
			s.close()
			print("Test: {}".format(test_name))
			total += print_result(response, "400 Bad Request")
		except Exception as e:
			print("\033[31m❌❌❌ FAIL\033[0m: {} - Exception occurred: {}".format(test_name, e))

print("===========================================================================")

if (total == total_test):
	print("\033[32m✅✅✅ All tests passed ✅✅✅\033[0m")
else:
	print("\033[31m total is {} tests over {}\033[0m".format(total, total_test))



print("==================================================================================================")
print("======================   TESTING ERROR CONTENT_LENGTH POST REQUEST =============================")
print("==================================================================================================")
for test_name, test_header in Error_headers.items():
		request ="POST / HTTP/1.1\r\nHost: {}\r\n{}\r\n".format(host, test_header)
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((ip, port))
			s.sendall(request.encode())
			response = s.recv(4096)
			s.close()
			print("Test: {}".format(test_name))
			if (test_name == "Header Value Too Large by 1byes"):
				total += print_result(response, "413")
			else:
				total += print_result(response, "408")
		except Exception as e:
			print("\033[31m❌❌❌ FAIL\033[0m: {} - Exception occurred: {}".format(test_name, e))

print("===============================================================================")
print("======================   TESTING BAD REQUEST LINE =============================")
print("===============================================================================")

bad_request_lines = OrderedDict([
    ("Empty Request Line", ""),
    ("Missing Path and Version", "GET /"),
    ("Missing Method", "/index.html HTTP/1.1"),
    ("Missing Version", "GET HTTP/1.1"),
    ("Invalid HTTP Version", "GET /index.html HTTP/1"),
    ("Invalid HTTP Version with Extra Digit", "GET /index.html HTTP/1.1.1"),
    ("Invalid HTTP Version Letters", "GET /index.html HTTP/x.y"),
    ("Spaces in Method G E T", "G E T /index.html HTTP/1.1"),
	("Spaces in Method P O S T", "P O ST /index.html HTTP/1.1"),
	("Spaces in Method", "P O ST /index.html HTTP/1.1"),
    ("Spaces in Version", "GET /index.html HTTP /1.1"),
    ("Trailing Spaces", "GET /index.html HTTP/1.1 "),
    ("No CRLF at End", "GET /index.html HTTP/1.1\r"),
    ("Incorrect Line Termination CR Only", "GET /index.html HTTP/1.1\r"),
    ("Invalid HTTP Version character HTT#P/1.1", "GET /index.html HTT#P/1.1"),
    ("Invalid Method Characters G@T", "G@T /index.html HTTP/1.1"),
	("Invalid Method Characters P@ST", "P@ST /index.html HTTP/1.1"),
	("Invalid Method UNKNOWM", "UNKNOWN /index.html HTTP/1.1")
])

for test_name, test_header in bad_request_lines.items():
	request = "{}\r\nHost: {}\r\n\r\n".format(test_header, host)
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((ip, port))
		s.sendall(request.encode())
		response = s.recv(4096)
		s.close()
		print("Test: {}".format(test_name))
		if ("Invalid Method"  in test_name):
			total += print_result(response, "405")
		elif ("Invalid HTTP Version" in test_name):
			total += print_result(response, "505")
		else:
			total += print_result(response, "400")
	except Exception as e:
		print("\033[31m❌❌❌ FAIL\033[0m: {} - Exception occurred: {}".format(test_name, e))


print("=============================================================================================================")
print("======================   TESTING ABSURD BUT GOOD HEADER PARSING ON POST REQUEST =============================")
print("=============================================================================================================")
correct_headers = OrderedDict([
	("Empty Content-Type Value with Semicolon", "Content-Type:\r\nContent-length: abc\r\n"),  # Header value is empty but ends with a semicolon, correctly terminated -> OK FOR GET (n content-Len) but not for POST
	("Empty Content-Type Value with Semicolon", "Content-Type:\r\nContent-length:\r\n")  # Header value is empty but ends with a semicolon, correctly terminated
])

for test_name, test_header in correct_headers.items():
	request = "GET / HTTP/1.1\r\nHost: {}\r\n{}\r\n".format(host, test_header)
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((ip, port))
		s.sendall(request.encode())
		response = s.recv(4096)
		s.close()
		print("Test: {}".format(test_name))
		total += print_result(response, "200 OK")
	except Exception as e:
		print("\033[31m❌❌❌ FAIL\033[0m: {} - Exception occurred: {}".format(test_name, e))