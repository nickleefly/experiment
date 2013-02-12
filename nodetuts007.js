var async = require('async');

function insert(obj, callback) {
  setTimeout(function() {
    //var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    console.log('inserted');
    callback();
  }, Math.random() * 1000);
}

var itemCount = 100;

var objects = [];
for(var i = 0; i < itemCount; i++) {
  objects.push(i);
}

async.forEachLimit(objects, 10, insert, function(err) {
  if(err) { return console.error(err);}
  console.log('Finished');
})