var fs = require('fs');
var stdin = process.stdin;
var stdout = process.stdout;

fs.readdir(process.cwd(), function(err, files) {
  console.log('');

  if(!files.length) {
    return console.log('  \033[31m No files to show! \033[39m \n');
  }

  console.log('  Select which file or directory you want to see \n');

  //called for each file walked in the directory

  var stats = [];
  function file(i) {
    var filename = files[i];

    fs.stat(__dirname + '/' + filename, function(err, stat) {
      stats[i] = stat;
      if(stat.isDirectory()) {
        console.log('  + ' + i + ' \033[36m' + filename + '\033[39m');
      } else {
        console.log('  + ' + i + ' \033[90m' + filename + '\033[39m');
      }

      if(++i == files.length) {
        console.log('');
	    read();
      } else {
        file(i);
      }
    });
  }
  file(0);
});

//read user input when files are shown
function read() {
  console.log('');
  stdout.write('\033[33m Enter your choice: \033[39m');

  stdin.resume();
  stdin.setEncoding('utf8');
  stdin.on('data', option);
}

//called with the option supplied by the user
function option(data) {
  if(stats[Number(data)].isDirectory()) {
    fs.readdir(__dirname + '/' + filename, function (err, files) {
      console.log(''); 
      console.log('(' + files.length + 'files)');
      files.forEach(function(files) {
       console.log('-' + file);
      });
      console.log('');
    });
  } else {
    stdin.pause();
    fs.readFile(__dirname + '/' + filename, 'utf8', function(err, data) {
      console.log('');
      console.log('\033[90m' + data.replace(/(.*)/g, '$1') + '\033[39m');
    });
  }
}
