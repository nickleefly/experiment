var http = require('http');
var spawn = require('child_process').spawn;

http.createServer(function(req, res) {
  var parts = req.url.split('/');
  var path = __dirname + '/img/' + parts[1];
  var args = [path, '-resize', parts[2], '-'];
  var convert = spawn('convert', args);

  convert.stdout.pipe(res);
  convert.stderr.pipe(process.stderr);
}).listen(8000);
