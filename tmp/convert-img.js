var http = require('http');
var spawn = require('child_process').spawn;
var config = require('./config');
var s3 = require('knox');

http.createServer(function(req, res) {
  var parts = req.url.split('/');

  s3.get('/' + parts[1]).on('response', function(s3Res) {
    var args = ['-', '-resize', parts[2], '-'];
    var convert = spawn('convert', args);

    s3Res.pipe(convert.stdin);

    convert.stdout.pipe(res);
    convert.stderr.pipe(process.stderr);
  }).end();
}).listen(8000);
