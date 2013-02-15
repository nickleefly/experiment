/** 
insert 10 objects in the database, and when all are done, invoke this callback
**/

function clone(o) {
  return JSON.parse(JSON.stringify(o));
}

function insert(obj, callback) {
  setTimeout(function() {
    // var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    // callback(error);
    callback();
  }, Math.random() * 3000);
}

function insertAll(collection, callback) {

  var queue = clone(collection);
  var finished = 0;
  var calledback = false;

  (function insertOne() {
    var elem = queue.shift();
    if(elem) {
      insert(elem, function(err) {
        if(err) { return callback(err);}
        console.log('elem %s got inserted', elem);
        insertOne();
      });
    } else {
      callback();
    }
  }());
}

var objs = [1,2,3,4,5,6,7,8,9,10];

insertAll(objs, function(err) {
  if(err) { return console.error(err);}
  console.log('All are finished');
});