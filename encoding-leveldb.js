var levelup = require('levelup')
var path = require('path')
var db_dir = path.join(__dirname, '../tmp/dprk.db')
var db = levelup(db_dir, { valueEncoding: 'json' })

db.put(
    'dprk'
  , {
        name       : 'Kim Jong-un'
      , spouse     : 'Ri Sol-ju'
      , dob        : '8 January 1983'
      , occupation : 'Clown'
    }
  , function (err) {
      db.get('dprk', function (err, value) {
        console.log('dprk:', value)
        db.close()
      })
    }
)