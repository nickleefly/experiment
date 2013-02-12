var async = require('async');

async.waterfall([
  function(callback) {
    callback(null, 1,2,3);
  },

  function(a,b,c, callback) {
    console.log('%s, %s, %s', a, b, c);
    callback(null, 4,5);
  },

  function(a,b, callback) {
    console.log('%s, %s', a, b);
    callback(null, 6,7);
  }
], function(err, result) {
  if(err) { return console.error(err);}
  console.log('Finished result:', result);
});