// Require the stuff we need
var connect = require("connect");
var http = require("http");

// Build the app
var app = connect();

// Add some middleware
app.use(function(request, response) {
  response.writeHead(200, { "Content-Type": "text/plain" });
  response.end("Hello world!\n");
});

// Start it up!
http.createServer(app).listen(1337);