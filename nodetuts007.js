var async = require('async');

function filter(obj, callback) {
  setTimeout(function() {
    // var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    callback(obj >=3);
  }, Math.random() * 1000);
}

var objects = [1,2,3,4];

async.reject(objects, filter, function(err, results) {
  if(err) { return console.error(err);}
  console.log('Finished results %j', results);
});