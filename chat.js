var net = require('net');

var server = net.createServer(function (conn) {
  conn.write(
  '\n > welcome to \033[92mnode-chat\033[39m!' 
  + '\n > ' + count + ' other people are connnected at this time.'
  + '\n> please write your name and press enter: ');
  count++;
  
  conn.on('data', function(data){
    console.log(data)  ;
  });
  conn.on('close', function() {
    count--;
  });
}).listen(3000);
console.log('\033[96m server listening on :3000\033[39m');
