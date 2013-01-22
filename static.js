var http = require('http');
var path = require('path');
var fs = require('fs');

http.createServer(function(request, response) {
  var lookup = path.basename(decodeURL(request.url)) || 'index.html';
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