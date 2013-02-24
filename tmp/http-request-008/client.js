var http = require('http');

var options = {
  host: 'localhost',
  port: 8080,
  path: '/path',
  headers: {
   'Transfer-Encoding': 'chunked'
  }
};

var client = http.request(options);

client.on('response', function(res) {
console.log('status code: %d', res.statusCode);
console.log('response headers: %j', res.headers);
});

client.write('body part 1');
client.write('body part 2');
client.end('body part 3');
