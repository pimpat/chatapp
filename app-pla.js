//	initializes app to be a function handler that you can supply to an HTTP server
var express = require('express');
var app = express();
var http = require('http').Server(app);
var path = require('path');

//	new instance of socket.io
var io = require('socket.io')(http);
var SessionId = "";

// var users = {};
// var rooms = ['Default','Room1','Room2'];
// var rDefault = {};
// var rRoom1 = {};
// var rRoom2 = {}; 
app.use(express.static('/'));
app.use(express.static('views'));
app.use(express.static('public'));

// var private_room = {};
var usernames={};

//	define a route handler / that gets called when we hit our website home
app.get('/', function(req, res){
  res.sendFile('index.html');
});
app.get('/datamodel', function(req, res){
  res.sendFile('datamodel.html');
});

//	listen on the connection event for incoming sockets
io.on('connection', function(socket){
	var address = socket.handshake.address;
	console.log(address);

	// console.log(socket.handshake.address);
  // console.log(socket.client.conn.remoteAddress);
  // var sockId = socket.id;
  // console.log(sockId)

    console.log('New user connected.');

  //-----------------------------------------------

    var zmq = require('zmq');
    console.log("Connecting to ZMQ-node server...");
    var requester = zmq.socket('req');

    requester.on("message", function(reply){
      // console.log("test reply");
      console.log(reply.toString());
      var x = reply.toString().split(":");
      console.log("["+x[0]+"]................................");
      switch (x[0]){
        case "0":
          console.log(x[2]+"-->"+x[1]);
          break;
        case "1":
          console.log(x[2]+"-->"+x[1]);
          break;
        case "2":
          console.log(x[2]+"-->"+x[1]);
          break;
        case "3":
          if(x[2]=="none"){
            console.log('fetch by..'+x[1]+'\nno msg');
          }
          else{
            console.log('fetch by..'+x[1]+"\nfrom: "+x[2]+"\nto: "+x[3]+"\nmsg: "+x[4]+"\ntime: "+x[5]);
            console.log(socket.username);
            io.emit('fetchmsg',x[1],x[2],x[3],x[4],x[5]);
          }
          break;
        case "4":
          console.log("send \'onlineList\'to web...");
          io.emit('updateusers',x);
          break;
        case "5":
          console.log('send \'chatHist\'to web...');
          io.emit('get_history',x);
          break;
        case "6":
          console.log('req add group name \''+x[1]+'\'...');
          io.emit('req_add_group_name',x);
          break;
        case "7":
          console.log(x[1]);
          break;
        case "8":
          console.log(x[2]+"-->"+x[1]);
          break;
        case "9":
          if(x[2]=="none"){
            console.log('fetch invite msg by..'+x[1]+'\nnot found invite msg');
          }
          else{
            console.log('fetch invite msg by..'+x[1]+"\nfrom: "+x[2]+"\nmsg: "+x[3]);
            // console.log(socket.username);
            io.emit('req_join_group',x[1],x[2],x[3]);
          }
          break;
        case "10":
          io.emit('get_history',x);
          break;
        case "11":
          if(x[2]!="0"){
            for(var i=3;i<x.length;i++){
              if(groupnames[x[i]]==undefined)
                groupnames[x[i]]=x[i];
            }
            for(var key in groupnames){
              console.log("gname: "+groupnames[key]);
            }
            io.emit('updategroups',x);
          }
          break;
        case "12":
          console.log("memberList--"+x[1]+":"+x[2]+":"+x[3]);
          io.emit('get_mb_group',x);
          break;
        case "13":
          console.log(x[1]);
          break;
      }
    });

    requester.connect("tcp://localhost:5555");
    console.log("Sending request …");

    // online list
    setInterval(function(){
      // console.log("setinterval: 4");
      requester.send("4");
    },2000);

    //  read msg
    setInterval(function(){
      // console.log("setinterval: 3");
      console.log(usernames);
      for(var key in usernames){
        requester.send("3:"+key);
      }
    },2000);

    //  read invite msg
    setInterval(function(){
      console.log(usernames);
      for(var key in usernames){
        requester.send("9:"+key);
      }
    },2000);

  //-----------------------------------------------

    socket.on("adduser",function(username){
      console.log("[adduser]-----------------------");
      console.log(username);

      socket.username = username;
      usernames[username] = username;

      SessionId = socket.id;

      console.log("socketID: "+SessionId);
      console.log(usernames);

      socket.emit('getSessionId', socket.id);

      requester.send("1:"+socket.username);
    });

    socket.on('fetch_mb_group',function(username,groupname){
      console.log("[fetch_mb_group]-----------------------");
      console.log("username: "+username+"\tgroupname: "+groupname);

      requester.send("12:"+username+":"+groupname);
    });

    socket.on("grouplist",function(username){
      console.log("[grouplist]-----------------------");
      console.log(username);

      requester.send("11:"+username);
    });

    socket.on("rep_join_group",function(username,groupname){
      console.log("[rep_join_group]-----------------------");
      console.log("name: "+username+"\ngroupname: "+groupname);

      requester.send("7:"+username+":"+groupname);
    });

    socket.on("update_chat_group",function(groupname){
      console.log("[update_chat_group]-----------------------");
      console.log(socket.username);

      requester.send("10:"+socket.username+":"+groupname);
    });

    socket.on("update_private_frd",function(frdname){
      console.log("[update_private_frd]-----------------------");
      console.log(socket.username);

      requester.send("5:"+socket.username+":"+frdname);
    });

    socket.on("sendchat",function(myname,friendname,msg){
      console.log("[sendchat]-----------------------");
      console.log(socket.username);
      console.log('sender: '+myname+'\nreceiver: '+friendname+'\nmsg: '+msg);

      requester.send("2:"+myname+":"+friendname+":"+msg);
    });

    socket.on('send_to_group',function(myname,groupname,msg){
      console.log("[send_to_group]-----------------------");
      // console.log(socket.username);
      console.log('sender: '+myname+'\ngroup: '+groupname+'\nmsg: '+msg);
      
      requester.send("8:"+myname+":"+groupname+":"+msg);
    });

    socket.on("new_group",function(groupname,friendsname){
      console.log("[new_group]-----------------------");
      console.log('groupname: '+groupname+'\nfriendsname: '+friendsname);

      requester.send("6:"+groupname+":"+friendsname);

    });

    socket.on("leavegroup",function(username,groupname){
      console.log("[leavegroup]-----------------------");
      console.log('groupname: '+groupname+'\nusername: '+username);

      requester.send("13:"+username+":"+groupname);
    });

    socket.on("disconnect",function(){
      if(usernames[socket.username] != undefined){
        console.log("[disconnect]-----------------------");
        console.log(socket.username);
        console.log('\''+socket.username+'\' disconnected.');

        //socket.emit('closeSession', SessionId);

        var tempname = socket.username;
        delete usernames[tempname];

        console.log(usernames);

        requester.send("0:"+tempname);
      }
      // else {
      //   console.log("undefined user");
      // }
    });

    // (to me only)
    // io.to(socket.id).emit(_,_);

    // (to all user)
    // io.emit(_,_);

    // (to everyone except me--broadcast)
    // socket.broadcast.emit(_,_);

});

//  make the http server listen on port 3000
http.listen(3000, function(){
  console.log('listening on *:3000');
});
