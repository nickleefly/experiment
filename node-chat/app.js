var express = require('express'),
    http = require('http'),
    app = express(),
    server = http.createServer(app),
    io = require('socket.io').listen(server),
    routes = require('./routes'),
    path = require('path')

    users = [],
    instances = [];

app.configure(function(){
  app.set('port', 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  //app.use("#{__dirname}/public/images/favicon.ico");
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function () {
  app.use(express.errorHandler());
});

app.get('/', routes.index);

server.listen(app.get('port'));

io.sockets.on('connection', function (socket) {
  var ip = socket.handshake.address.address;
  instances[ip] >= 0 ? instances[ip]++ : instances[ip] = 0;
  if (instances[ip] == 1) users.push(ip);

  socket.emit('welcome', {user: ip, users: users});
  socket.broadcast.emit('newConnection', {users: users });

  socket.on('msg', function (msg, user) {
    socket.broadcast.emit('newMessage', {msg: msg, user: user});
  });

  socket.on('disconnect', function () {
    instances[ip] > 0 ? instances[ip]-- : instances[ip] = 0;

    if (instances[ip]==0) {
      if (users.indexOf(ip) != -1) users.splice(users.indexOf(ip),1);
      socket.broadcast.emit('disconnected', {users: users});
    }
  });
});

console.log('Chat running, port:'+app.get('port'));