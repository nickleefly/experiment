/** 
insert 10 objects in the database, and when all are done, invoke this callback
**/

function insert(obj, callback) {
  setTimeout(function() {
    //var error = Math.random() > 0.5 ? new Error('error!!!'):undefined;
    //callback(error);
    callback();
  }, Math.random() * 3000);
}

function insertAll(collection, callback) {

  var finished = 0;
  var calledback = false;

  function done() {
    if(!calledback) {
      calledback = true;
      callback.apply({}, arguments);
    }
  }

  for(var i = 0; i < collection.length; i++) {
    (function(i){
      insert(collection[i], function(err) {
        if(err) { return done(err);}
        console.log('insert %d is finished', i);

        if(++finished === collection.length) {
          done();
        }
      });
    }(i));
  }
}

var objs = [1,2,3,4,5,6,7,8,9,10];

insertAll(objs, function(err) {
  if(err) { return console.error(err);}
  console.log('All are finished');
});