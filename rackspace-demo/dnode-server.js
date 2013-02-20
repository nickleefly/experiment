var http = require('http');
var dnode = require('dnode');
var qs = require('querystring');

dnode.connect(8090, function(remote) {
    http.createServer(function(req, res) {
    if(req.url.match(/^\/login/)) {
      var param = qs.parse(req.url.split('?')[1]);
      remote.auth(param.user, param.pass, function(err) {
       res.end(err ? err : 'OK!');
      });
    }
    else {
      res.write('#' + process.argv[3] + '\n');
      res.end('beep boop!');
    }
    }).listen(parseInt(process.argv[2], 10));
});
