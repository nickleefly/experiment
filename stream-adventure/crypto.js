var crypto = require('crypto')
var st = crypto.createDecipher('aes256', process.argv[2])
process.stdin.pipe(st).pipe(process.stdout)
