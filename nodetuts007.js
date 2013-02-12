var async = require('async');

function insert(obj, callback) {
  setTimeout(function() {
    //var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    console.log('inserted');
    callback();
  }, Math.random() * 3000);
}

var itemCount = 10;

var objects = [];
for(var i = 0; i < itemCount; i++) {
  objects.push(i);
}

async.forEach(objects, insert, function(err) {
  if(err) { return console.error(err);}
  console.log('Finished');
})