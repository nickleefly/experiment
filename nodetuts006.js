/** 
insert 10 objects in the database, and when all are done, invoke this callback
**/

function insert(obj, callback) {
  setTimeout(callback, Math.random() * 3000);
}

function insertAll(collection, callback) {

  var finished = 0;

  for(var i = 0; i < collection.length; i++) {
    (function(i){
      insert(collection[i], function() {
        console.log('insert %d is finished', i);

        if(++finished === collection.length) {
          callback();
        }
      });
    }(i));
  }
}

var objs = [1,2,3,4,5,6,7,8,9,10];

insertAll(objs, function() {
  console.log('All are finished');
});