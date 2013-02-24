var http = require('http');

var concurrentRequests = 0;

http.createServer(function(req, res) {
  //console.log('concurrent requests: %d', ++concurrentRequests);

  console.log('new request -> ');
  console.log(' url: %s', req.url);
  console.log(' headers: %j', req.headers);

  req.setEncoding('utf8');

  req.on('data', function(data) {
    console.log(' data: %s', data); 
  });

  req.on('end', function() {
  });

  res.end('Hello');
}).listen(8080);
