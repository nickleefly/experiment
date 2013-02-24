var http = require('http');

var options = {
  host: 'localhost',
  port: 8080,
  path: '/path',
  headers: {
   'Transfer-Encoding': 'chunked'
  }
};

var req = http.request(options);

req.on('response', function(res) {
console.log('status code: %d', res.statusCode);
console.log('response headers: %j', res.headers);
});

req.write('body part 1');
req.write('body part 2');
req.end('body part 3');
