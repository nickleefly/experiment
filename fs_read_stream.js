var fs = require('fs');

var readStream = fs.createReadStream('/Users/asus/Documents/work/script.js');
readStream.on('data', function(data) {
  console.log('got some data:', data);
});

readStream.on('end', function() {
  console.log('ended');
});

readStream.setEncoding('utf-8');