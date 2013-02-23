var http = require('http');

var options = {
  host: 'localhost',
  port: 8080,
  path: '/path',
  headers: {
    'X-Powered-By': 'Node',
    'X-Author': 'Nick'
  }
};

http.globalAgent.maxSockets = 10;

function fireRequest() {
  var client = http.request(options);

  client.on('response', function(res) {
    console.log('status code: %d', res.statusCode);
    console.log('response headers: %j', res.headers);
  });

  client.end();
}

for(var i = 0; i < 20; i++) {
  fireRequest();
}
