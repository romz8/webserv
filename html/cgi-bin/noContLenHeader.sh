#!/bin/bash
# This script is a simple CGI script that sends a response without a Content-Length header.
# CGI system should pick this up and us EOF as body Content-Lenght.

echo 'Content-Type: text/html'
echo 
echo '<!DOCTYPE html>'
echo '<html>'
echo '   <head>'
echo '       <title>Hello there!</title>'
echo '   </head>'
echo '   <body>'
echo '       <h1>Hello from a CGI script!</h1>'
echo '   </body>'
echo '</html>'