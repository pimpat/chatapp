//	initializes app to be a function handler that you can supply to an HTTP server
var app = require('express')();
var http = require('http').Server(app);

//	new instance of socket.io
var io = require('socket.io')(http);
var usernames={};

//	define a route handler / that gets called when we hit our website home
app.get('/', function(req, res){
  res.sendFile('/Users/pimpat/Downloads/multi_room/index.html');
});

//	listen on the connection event for incoming sockets
io.on('connection', function(socket){
	var address = socket.handshake.address;
	console.log(address);
  // console.log(socket.handshake.address);
  // console.log(socket.client.conn.remoteAddress);
  // var sockId = socket.id;
  // console.log(sockId);
  console.log('New user connected.');

  //---------------------------------------------------------

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
            console.log('fetch by..'+x[1]+"\nfrom: "+x[2]+"\nmsg: "+x[3]+"\ntime: "+x[4]);
            console.log(socket.username);
            io.emit('fetchmsg',x[1],x[2],x[3],x[4]);
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
          console.log('req join group \''+x[1]+'\'...');
          io.emit('req_join_group',x);
      }
    });

    requester.connect("tcp://localhost:5555");
    console.log("Sending request...");

    setInterval(function(){
      // console.log("setinterval: 4");
      requester.send("4");
    },2000);

    setInterval(function(){
      // console.log("setinterval: 3");
      console.log(usernames);
      for(var key in usernames){
        requester.send("3:"+key);
      }
    },2000);

  //-----------------------------------------------

    socket.on("adduser",function(username){
      console.log("[adduser]-----------------------");
      console.log(username);

      socket.username = username;
      usernames[username] = username;

      console.log(socket.id);
      console.log(usernames);

      requester.send("1:"+socket.username);
    });

    // socket.on("updategroups"),function(myname){
    //   console.log("[updategroups]-----------------------");
    //   console.log(myname);
    //   requester.send("7:"+myname);
    // });

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
      console.log(socket.username);
      console.log('sender: '+myname+'\ngroup: '+groupname+'\nmsg: '+msg);
      
      requester.send("7:"+myname+":"+groupname+":"+msg);
    });

    socket.on("new_group",function(groupname,friendsname){
      console.log("[new_group]-----------------------");
      console.log('groupname: '+groupname+'\nfriendsname: '+friendsname);

      requester.send("6:"+groupname+":"+friendsname);

    });

    socket.on("disconnect",function(){
      if(usernames[socket.username] != undefined){
        console.log("[disconnect]-----------------------");
        console.log(socket.username);
        console.log('\''+socket.username+'\' disconnected.');

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

//	make the http server listen on port 3000
http.listen(3000, function(){
  console.log('listening on *:3000');
});
