#!/usr/bin/env node

const querystring = require('querystring');

function getCookie(cookieName) {
    const cookies = process.env.HTTP_COOKIE || '';
    const cookiesArray = cookies.split(';');
    for (const cookie in cookiesArray)
    {
        const [key, value] = cookiesArray[cookie].trim().split('=');
        if (key === cookieName)
            return decodeURIComponent(value);
    }
    return "";
}

function hasCookie()
{
  return process.env.HTTP_COOKIE !== undefined && process.env.HTTP_COOKIE.trim().length > 0;
}

function respond(headers, content)
{
    headers['Content-Length'] = Buffer.byteLength(content, 'utf8');
    console.log('Content-Type: text/html');
    for (const header in headers)
        console.log(`${header}: ${headers[header]}`);
    console.log("\r");
    console.log(content);
}

function calculateSessionDuration(sessionTime) {
  if (!sessionTime)
      return 'N/A';
  const sessionStart = new Date(sessionTime);
  const now = new Date();
  const diff = Math.floor((now - sessionStart) / 1000); // in seconds

  const hours = Math.floor(diff / 3600);
  const minutes = Math.floor((diff % 3600) / 60);
  const seconds = diff % 60;

  return `${hours}h ${minutes}m ${seconds}s`;
}

function generateSessionPage(username, sessionInfo) 
{
    return `
      <!DOCTYPE html>
      <html lang="en">
      <head>
          <meta charset="UTF-8">
          <title>Session Management</title>
      </head>
      <body>
          <h1>Welcome, ${username}</h1>
          <p>Time of Connection: ${sessionInfo.time}</p>
          <p>You have been connected for : ${sessionInfo.duration}</p>
          <p>Cookie Expires At: ${sessionInfo.expiration}</p>
          <a href="/cgi-bin/cookie.js?logout=true">Logout</a>
      </body>
      </html>
    `;
  }

function generateLoginPage() 
{
    return `
      <!DOCTYPE html>
      <html lang="en">
      <head>
          <meta charset="UTF-8">
          <title>Login</title>
          <link rel="stylesheet" href="/css/style.css">
      </head>
      <body>
        <div>
          <h1>Login to setup Session Management</h1>
          <form method="GET" action="/cgi-bin/cookie.js">
              <label for="username">Username:</label>
              <input type="text" id="username" name="username" required>
              <button type="submit">Login</button>
          </form>
        </div>
      </body>
      </html>
    `;
  }

function main() 
{
    let headers = {};
    if (hasCookie())
    {
        const username = getCookie('username') || 'N/A';
        const sessionTime = getCookie('sessionTime');
        const cookieExpiration = getCookie('cookieExpiration');
        
        const sessionInfo = { time: sessionTime || 'N/A', 
        duration: calculateSessionDuration(sessionTime),          
        expiration: cookieExpiration || 'N/A' };
        
        respond(headers, generateSessionPage(username, sessionInfo));
    }
    else
        respond(headers, generateLoginPage());
}

main();