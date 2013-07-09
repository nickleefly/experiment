var through = require('through');
var split = require('split');
var fs = require('fs');

var lineCount = 0;
var tr = through(function (buf) {
  var line = buf.toString();
  this.queue(lineCount % 2 === 0
             ? line.toLowerCase() + '\n'
             : line.toUpperCase() + '\n'
  );
  lineCount ++;
});
fs.createReadStream('lines.js').pipe(split()).pipe(tr).pipe(fs.createWriteStream('out.txt'));
