var fs = require('fs');

var writeStream = fs.createWriteStream('/Users/asus/Documents/work/writeStream.txt');

var interval = setInterval(function() {
  var flushed = writeStream.write((new Date()).toString() + '\n');
  console.log('flushed: ', flushed);
}, 100);

setTimeout(function() {
  clearInterval(interval);
  writeStream.end();
},5000);

writeStream.on('drain', function() {
  console.log('drained');
})