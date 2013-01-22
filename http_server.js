var http = require('http');
var fs = require('fs');

function randomFileName() {
  return '/' + Date.now() + '_' + Math.random() * 1000000 + '.txt';
}

http.createServer(function(req, res) {
  var fileName = randomFileName();
  console.log('writing request to', fileName);
  req.pipe(fs.createWriteStream(fileName));

  res.writeHead(200, {'content-type': 'text/plain'});
  fs.createReadStream(__filename).pipe(res);
}).listen(8080);