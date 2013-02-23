var http = require('http');

var concurrentRequests = 0;

http.createServer(function(req, res) {
  console.log('concurrent requests: %d', ++concurrentRequests);
  console.log('new request -> ');
  console.log(' url: %s', req.url);
  console.log(' headers: %j', req.headers);

  req.setEncoding('urf8');
  req.on('data', function(data) {
    console.log(' data: %s', data); 
  });

  setTimeout(function() {
    console.log('concurrent requests: %d', --concurrentRequests);
    res.end('Hello');
  }, 1000);
}).listen(8080);
