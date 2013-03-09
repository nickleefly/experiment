var http = require('http');

http.createServer(function(req, res) {
  res.writeHead(200, {'content-type': 'text/plain'});
  res.end('Hello World\n');
}).listen(3000);

console.log('Server listening on port 3000');