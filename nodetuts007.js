var async = require('async');

function insert(obj, callback) {
  setTimeout(function() {
    // var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    console.log('inserted');
    callback();
  }, Math.random() * 1000);
}

async.series([
  function(callback) {
    insert(1, callback);
  },
  function(callback) {
    insert(2, callback);
  },
  function(callback) {
    insert(3, callback);
  }],function(err) {
    if(err) { return console.error(err);}
    console.log('Finished');
  }
)