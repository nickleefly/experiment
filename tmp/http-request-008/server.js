var http = require('http');
var fs = require('fs');

http.createServer(function(req, res) {

  console.log('new request -> ');
  console.log(' method: %s', req.method);
  console.log(' url: %s', req.url);
  console.log(' headers: %j', req.headers);

  req.pipe(res);
}).listen(8080);
