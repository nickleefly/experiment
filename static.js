var http = require('http');
var path = require('path');
var fs = require('fs');
var mimeTypes = {
  '.js': 'text/javascript',
  '.html': 'text/html',
  '.css': 'text/css'
};

http.createServer(function(request, response) {
  var lookup = path.basename(decodeURI(request.url)) || 'index.html',
  f = 'content/' + lookup;
  fs.exists(f, function(exists) {
    if(exists) {
      fs.readFile(f, function(err, data) {
        if(err) {
          response.writeHead(500);
          response.end('Server Error!');
          return;
        }

        var headers = {'Content-Type': mimeTypes[path.extname(lookup)]};
        response.writeHead(200, headers);
        response.end(data);
      });
      return;
    }
    response.writeHead(400);
    response.end();
  });
}).listen(8080);