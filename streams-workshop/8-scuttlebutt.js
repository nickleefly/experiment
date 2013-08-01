var split = require('split')
var net = require('net')
var opts = require('optimist').argv
var through = require('through')
var Model = require('scuttlebutt/model')
var chat  = new Model()

var name = opts.name

if(!name)
  throw new Error('must provide --name yourname')

//read from stdin and cat.set(name, message)
process.stdin
  .pipe(split()) //split into lines
  .on('error', function () {
    stream.destroy()
  })
  .pipe(through(function (message) {
    chat.set(Date.now() + ' ' + name, message)
  })).on('error', function () {
    stream.destroy()
  })

chat.on('change', function (key, value) {
  console.log(key+':', value)
})


if(opts.server)
  net.createServer(function (stream) {
    var addr = stream.remoteAddress
    stream.pipe(chat.createStream()).on('error', function () {
    stream.destroy()
  }).pipe(stream)
  }).listen(opts.port || 8989)
else {
  //HMM, is there some way to reconnect without chat breaking?

  var stream = net.connect(opts.port || 8989, opts.host)
  stream.pipe(chat.createStream()).on('error', function () {
    stream.destroy()
  }).pipe(stream)
}