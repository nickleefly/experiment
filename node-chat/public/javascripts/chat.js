var socket = io.connect('//n.lava.bz:80'), user;

socket.on('welcome', function (data) {
  $('#msgs').append('<div class="msg"><span>Welcome ' + data.user + '</span></div>');
  user = data.user;
  $('#userList').empty();
  data.users.forEach(function (u) {
    $('#userList').append('<div>' + u + '</div>');  
  });
  setTimeout("window.scrollTo(0, 0);");
});

socket.on('newConnection', function (data) {
  $('#userList').empty();
  data.users.forEach(function (u) {
    $('#userList').append('<div>' + u + '</div>');  
  });
});

socket.on('newMessage', function (data) {
  var date = new Date();
  timestamp = date.toISOString();
  $('#msgs').append('<div class="msg">' + data.msg.msg + '<br /><span class="subtleMsg smIndent">' + data.msg.user + ' </span> <abbr class="timestamp" title="' + timestamp + '"></abbr></div>');
  $('.timestamp').timeago();
  setTimeout("window.scrollTo(0, document.getElementById('msgs').scrollHeight);",0);
});

socket.on('disconnected', function (data) {
  $('#userList').empty();
  data.users.forEach(function (u) {
    $('#userList').append('<div>' + u + '</div>');  
  });
});

function sendMessage() {
    var msg = $('#msgInput').attr('value');
    if (msg) {
        var date = new Date();
        timestamp = date.toISOString();
        $('#msgs').append('<div class="msg">' + msg + '<br /><span class="subtleMsg smIndent">' + user + ' </span><abbr class="timestamp" title="' + timestamp + '"></abbr></div>');
        $('.timestamp').timeago();
        socket.emit('msg', {msg: msg, user: user});
        $('#msgInput').attr('value', '');
        setTimeout("window.scrollTo(0, document.getElementById('msgs').scrollHeight);",0);
    }
};

var mWidth, mHeight;

function fluidElements() {
    mWidth = $(window).width();
    mHeight = $(window).height();
    
    $('#shell').css('height', mHeight);

    if (mWidth >= 800) {
    } else {
    }
};

function focusInput() {
  $('#msgInput').focus();
}

$(window).resize(function() {
    fluidElements();
    focusInput();
});

$(document).ready(function() {
    fluidElements();
    focusInput();
    $('#msgs').css('display', 'block');
});

function toggleView() {
  if ($('#msgs').css('display') == 'none') {
    $('#msgs').css('display','block');
    $('#toggleViewButton').html('users');
    setTimeout("window.scrollTo(0, document.getElementById('msgs').scrollHeight);",0);
  } else {
    $('#msgs').css('display','none');
  }

  if ($('#userList').css('display') == 'none') {
    $('#toggleViewButton').html('chat');
    $('#userList').css('display','block');
    setTimeout("window.scrollTo(0, 0);");
  } else {
    $('#userList').css('display','none');
  }
}