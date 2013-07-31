// echo hack the planet | curl -d@- http://localhost:8000
var http = require('http');
var fs = require('fs');
var server = http.createServer(function (req, res) {
    if (req.method === 'POST') {
       req.pipe(fs.createWriteStream('post.txt'));
    }
    res.end('beep boop\n');
});
server.listen(8000);
