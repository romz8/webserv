
import cgi
import cgitb

cgitb.enable()

# Bad delim using one newline (\n) instead of two (\n\n or \r\n\r\n).
print("Content-Type: text/html\nContent-Length: 100")
print("<html><body>Hello!</body></html>")  # Only single newlines