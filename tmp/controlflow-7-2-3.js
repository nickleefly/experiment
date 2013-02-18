function async(arg, callback) {
  console.log('do something with \''+arg+'\', return 1 sec later');
  setTimeout(function() { callback(arg * 2); }, 1000);
}
function final() { console.log('Done', results); }

var items = [ 1, 2, 3, 4, 5, 6 ];
var results = [];
var running = 0;
var limit = 2;

function launcher() {
  while(running < limit && items.length > 0) {
    var item = items.shift();
    console.log('item value is', item);
    async(item, function(result) {
      results.push(result);
      console.log('running before -- is', running);
      running--;
      console.log('running value is', running);
      if(items.length > 0) {
        launcher();
      } else if(running == 0) {
        final();
      }
    });
    running++;
    console.log('running value after ++ is', running);
  }
}

launcher();
