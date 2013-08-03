function variance (db, prefix, callback) {
  var n = 0, m2 = 0, mean = 0

  db.createReadStream({
        start : prefix          // jump to first key with the prefix
      , end   : prefix + '\xFF' // stop at the last key with the prefix
    })
    .on('data', function (data) {
      var delta = data.value - mean
      mean += delta / ++n
      m2 = m2 + delta * (data.value - mean)
    })
    .on('error', callback)
    .on('close', function () {
      callback(null, m2 / (n - 1))
    })
}

variance(db, 'au_nsw_southcoast~', function (err, v) {
  //v = variance
})