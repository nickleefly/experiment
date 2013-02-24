var http = require('http');
var fs = require('fs');

var concurrentRequests = 0;

http.createServer(function(req, res) {
  var ws = fs.createWriteStream('./cat_copy.jpg');

  console.log('new request -> ');
  console.log(' url: %s', req.url);
  console.log(' headers: %j', req.headers);

  req.pipe(ws);

  req.on('end', function() {
    res.end('Hello');
  });

}).listen(8080);
