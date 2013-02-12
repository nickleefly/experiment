var async = require('async');

function fun(callback) {
  var args = Array.prototype.slice.call(arguments);
  args.shift();
  args.unshift(null);
  setTimeout(function() {
    callback.apply({}, args);
  }, Math.random() * 2000);
}

async.waterfall([
  function(callback) {
    fun(callback, 1,2,3);
  },

  function(a,b,c, callback) {
    console.log('%s, %s, %s', a, b, c);
    fun(callback, 4,5);
  },

  function(a,b, callback) {
    console.log('%s, %s', a, b);
    fun(callback, 6,7);
  }
], function(err, result) {
  if(err) { return console.error(err);}
  console.log('Finished result:', result);
});