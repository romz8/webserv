<?php phpinfo(); ?>
<?php
// Start the session and register the session variables
session_start();

// Check if the username is received from the GET request
if (isset($_GET['username'])) {
    // Sanitize the username input to prevent security issues
    $username = filter_input(INPUT_GET, 'username', FILTER_SANITIZE_SPECIAL_CHARS);

    // Set a session variable for the username
    $_SESSION['username'] = $username;

    // Set a cookie for the username that expires in 1 hour
    setcookie('username', $username, time() + 3600, "/");

    // Display a welcome message
    echo "<!DOCTYPE html>
    <html lang='en'>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Welcome</title>
        <link rel='stylesheet' href='/css/style.css'>
    </head>
    <body>
        <div>
            <h1>Welcome, " . htmlspecialchars($username) . "!</h1>
            <p>You are now logged in.</p>
        </div>
    </body>
    </html>";
} 
else 
{
    // Redirect back to the form if the username is not provided
    header("Location: /index.html");
    exit;
}
?>
