var http = require('http');

http.createServer(function(req, res) {
  res.write('#' + process.argv[3] + '\n');
  res.end('beep boop!');
}).listen(parseInt(process.argv[2], 10));
