var net = require('net');
var count = 0;
var users = {};
 
var server = net.createServer(function (conn) {
  conn.write(
  '\n > welcome to \033[92mnode-chat\033[39m!' 
  + '\n > ' + count + ' other people are connnected at this time.'
  + '\n> please write your name and press enter: ');
  count++;
  
  var nickname;
  conn.on('data', function(data){
    if(!nickname) {
      if(users[data]) {
        console.log('\033[93m> nickname already in use. try again:\033[39m');
        return;
      } else {
        nickname = data;
        users[nickname] = conn;

        for(var i in users) {
          if(i != nickname) {
            users[i].write('\033[90m >' + nickname + ' joined the room \033[39m\n');
          }
        }
      }
    }
    data = data.replace('\r\n', '');
    console.log(data)  ;
  });
  conn.on('close', function() {
    count--;
  });
  conn.setEncoding('utf8');
}).listen(3000);
console.log('\033[96m server listening on :3000\033[39m');
