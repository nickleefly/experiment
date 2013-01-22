var fs = require('fs');

var readStream = fs.createReadStream('/Users/asus/Documents/work/script.js');

readStream.pause();

readStream.on('data', function(data) {
  console.log('got some data:', data);
  readStream.pause();

  setTimeout(function() {
    readStream.resume();
  }, 100);
});

readStream.on('end', function() {
  console.log('ended');
});

//readStream.setEncoding('base64');

setTimeout(function() {
  readStream.resume();
}, 100);