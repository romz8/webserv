import cgi
import cgitb

cgitb.enable()

# Bad delim using one newline (\r\n) instead of (\n\n or \r\n\r\n).
print("Content-Type: text/html\nContent-Length: 100\r")
print("<html><body>Hello!</body></html>")  # Only single newlines