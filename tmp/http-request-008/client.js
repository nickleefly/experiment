var http = require('http');
var fs = require('fs');

var readStream = fs.createReadStream('./cat.jpg');
var ws = fs.createWriteStream('./cat_back.jpg');

var options = {
  host: 'localhost',
  port: 8080,
  method: 'POST',
  path: '/path',
  headers: {
   'Transfer-Encoding': 'chunked'
  }
};

var req = http.request(options);

req.on('response', function(res) {
  console.log('status code: %d', res.statusCode);
  console.log('response headers: %j', res.headers);
  res.pipe(ws);
});

readStream.pipe(req);
