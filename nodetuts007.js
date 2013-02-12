var async = require('async');

function retrieve(id, callback) {
  setTimeout(function() {
    // var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    callback(null, {
      id: id,
      value: Math.round(Math.random() * 1000)
    });
  }, Math.random() * 1000);
}

var objects = [1,2,3,4];

async.map(objects, retrieve, function(err, results) {
  if(err) { return console.error(err);}
  console.log('Finished results %j', results);
});