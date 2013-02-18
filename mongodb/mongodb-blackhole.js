var mongodb = require('mongodb');
 
var blackhole = require('net').createServer(function (c) {
  // console.log('new connection:', c.server._connectionKey);
  c.end();
});
blackhole.listen(24008, function () {
  console.log('BlackHole listened at "' + 24008 + '"');
 
  var replSet = new mongodb.ReplSetServers( [ 
      new mongodb.Server('127.0.0.1', 24008, { auto_reconnect: true } )
    ]
  );
  var client = new mongodb.Db('test', replSet);
  client.open(function (err, p_client) {
    console.log(err);
  });
});
