#! usr/bin/env node

const querystring=require('querystring');

function setCookie(name, value, expiration)
{
	return (`${name}=${encodeURIComponent(value)}; Expires=${expiration.toUTCString()}; Path=/`);
}

function respond(headers, content)
{
	headers['Content-Length']=Buffer.byteLength(content, 'utf8');
	console.log('Content-Type: text/html');
	for(const header in headers)
		console.log(`${header}: ${headers[header]}`);
	console.log("\r\n");
	console.log(content);
}

function generateLoggedInPage(username) 
{
	return `
	  <!DOCTYPE html>
	  <html lang="en">
	  <head>
		  <meta charset="UTF-8">
		  <title>Login Successful</title>
	  </head>
	  <body>
		  <h1>You are now logged in, ${username}!</h1>
		  <a href="/cgi-bin/session.js">Go to Session Management</a>
	  </body>
	  </html>
	`;
  }

  function generateLogoutPage() 
  {
	return `
	  <!DOCTYPE html>
	  <html lang="en">
	  <head>
		  <meta charset="UTF-8">
		  <title>Logout</title>
	  </head>
	  <body>
		  <h1>You have logged out successfully!</h1>
		  <a href="/cgi-bin/session.js">Go to Login Page</a>
	  </body>
	  </html>
	`;
  }

  function generateBadLoginPage() 
  {
	return `
	  <!DOCTYPE html>
	  <html lang="en">
	  <head>
		  <meta charset="UTF-8">
		  <title>Login Unsuccessful</title>
	  </head>
	  <body>
		  <h1>Login failed !</h1>
		  <a href="/cgi-bin/session.js">Go to Login Page</a>
	  </body>
	  </html>
	`;
  }

  function main()
  {
	const query=querystring.parse(process.env.QUERY_STRING);
	const username=query.username;

	let headers = {};

	if (query.logout)
	{
		headers['Set-Cookie'] = setCookie('username', '', new Date(0));
		console.log(`Set-Cookie: ${setCookie('sessionTime', '', new Date(0))}`);
		console.log(`Set-Cookie: ${setCookie('cookieExpiration', '', new Date(0))}`);
		respond(headers, generateLogoutPage());
	}
	else if (username)
	{
		const sessionTime = new Date().toUTCString();
		const cookieExpiration = new Date(Date.now() + 3600 * 1000);
		headers['Set-Cookie'] = setCookie('username', username, cookieExpiration);
		console.log(`Set-Cookie: ${setCookie('sessionTime', sessionTime, cookieExpiration)}`);
        console.log(`Set-Cookie: ${setCookie('cookieExpiration', cookieExpiration.toUTCString(), cookieExpiration)}`);
		respond(headers, generateLoggedInPage(username));
	}
	else
		respond(headers, generateBadLoginPage());
  }

  main();