//	initializes app to be a function handler that you can supply to an HTTP server
var app = require('express')();
var http = require('http').Server(app);

//	new instance of socket.io
var io = require('socket.io')(http);

// var users = {};
// var rooms = ['Default','Room1','Room2'];
// var rDefault = {};
// var rRoom1 = {};
// var rRoom2 = {}; 

// var private_room = {};
var usernames={};

//	define a route handler / that gets called when we hit our website home
app.get('/', function(req, res){
  res.sendFile('/Users/pimpat/Downloads/multi_room/index.html');
});

//	listen on the connection event for incoming sockets
io.on('connection', function(socket){
	var address = socket.handshake.address;
	console.log(address);

	// 	console.log(socket.handshake.address);
 	//  console.log(socket.client.conn.remoteAddress);
  // var guest_name = "guest"+counter;
  // console.log(guest_name+' connected');

  	var sockId = socket.id;
  	console.log(sockId);

  	// users[sockId]=guest_name;
  	// console.log(users);

    console.log('New user connected.');

  //-----------------------------------------------

    var zmq = require('zmq');
    console.log("Connecting to hello node-c server…");
    var requester = zmq.socket('req');

    requester.on("message", function(reply) {
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
            // io.to(socket.id).emit('fetchmsg',x[1],x[2],x[3]);
            // socket.emit('fetchmsg',x[1],x[2],x[3]);
            // var temp_tstamp = parseInt(x[4]);
            io.emit('fetchmsg',x[1],x[2],x[3],x[4]);
            // io.emit('fetchmsg',x[1],x[2],x[3]);
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
      }
    });

    requester.connect("tcp://localhost:5555");
    console.log("Sending request …");
    // requester.send("0");
    setInterval(function(){
      // console.log("setinterval: 4");
      requester.send("4");
    },5000);

    setInterval(function(){
      // console.log("setinterval: 3");
      console.log(usernames);
      for(var key in usernames){
        requester.send("3::"+key);
      }

    },5000);

  //-----------------------------------------------

    socket.on("adduser",function(username){
      console.log("[adduser]-----------------------");
      console.log(username);

      // private_room[socket.username]="";
      socket.username = username;
      // socket.room ='Room1';
      // socket.join('Room1');
      // socket.room = 'Default';    // add
      usernames[username] = username;
      console.log(socket.id);
      // socket.join('Default');     //  add
      // io.to(socket.id).emit("noti_me_new_con",'!!! You have joined Default. !!!', username);
      // socket.broadcast.to('Default').emit("noti_frd_new_con", '!!! '+username+' has joined Default. !!!');  //  add
      console.log(usernames);
      // consol.log(mySocket);  ###
      // io.to(socket.id).emit('updaterooms',rooms,'Default');
      // io.emit('updaterooms',rooms,'Default');
      // rDefault[username]=username;
      requester.send("1::"+socket.username);
    });

    socket.on("update_private_frd",function(frdname){
      console.log("[update_private_frd]-----------------------");
      console.log(socket.username);

      // private_room[socket.username]=frdname;
      requester.send("5::"+socket.username+"::"+frdname);
    });

    socket.on("sendchat",function(myname,friendname,msg){
      console.log("[sendchat]-----------------------");
      console.log(socket.username);

      console.log('sender: '+myname+'\nreceiver: '+friendname+'\nmsg: '+msg);
      requester.send("2::"+myname+"::"+friendname+"::"+msg);
      // socket.broadcast.to(socket.room).emit("updatechat",username,msg);
      // io.in(socket.room).emit("updatechat",username,msg);
      // socket.broadcast.emit('updatechat', username, msg);
    });

    // socket.on("sendchat",function(username,msg){
    //   console.log('>>> chat from ...\nuser: '+username+'\tmsg: '+msg);
    //   socket.broadcast.emit('updatechat', username, msg);
    // });

    socket.on("disconnect",function(){
      console.log("[disconnect]-----------------------");
      console.log(socket.username);

      // delete private_room[socket.username];
      console.log('\''+socket.username+'\' disconnected.');

      // var address = socket.handshake.address;
      // console.log(address);

      // var sockId = socket.id;
      // console.log(sockId);
      // console.log(users[sockId]+' disconnected');
      
      var tempname = socket.username;
      delete usernames[tempname];
      console.log(usernames);
      requester.send("0::"+tempname);

      // delete users[sockId];
      // console.log(users);
    });

    // socket.on("switchroom",function(newroom){
    //   var oldroom = socket.room
    //   if(oldroom==newroom){
    //     console.log('*** '+socket.username+':'+oldroom+':'+newroom);

    //     // alert you are in this room
    //     io.to(socket.id).emit('updaterooms',oldroom,newroom);
    //   }
    //   else{

    //     socket.broadcast.to(socket.room).emit("noti_frd_discon", '!!! '+socket.username+' has left from '+socket.room+'. !!!');

    //     switch (oldroom){
    //       case "Default":
    //         delete rDefault[socket.username];
    //         break;
    //       case "Room1":
    //         delete rRoom1[socket.username];
    //         break;
    //       case "Room2":
    //         delete rRoom2[socket.username];
    //         break;
    //     }

    //     socket.leave(socket.room);

    //     switch (newroom){
    //     case "Default":
    //       rDefault[socket.username] = socket.username;
    //       break;
    //     case "Room1":
    //       rRoom1[socket.username] = socket.username;
    //       break;
    //     case "Room2":
    //       rRoom2[socket.username] = socket.username;
    //       break;
    //     }

    //     socket.join(newroom);

    //     // clear chat history before noti me
    //     io.to(socket.id).emit('updaterooms',oldroom,newroom);

    //     io.to(socket.id).emit("noti_me_new_con",'!!! You have joined '+newroom+'. !!!', socket.username);
        
    //     socket.room = newroom;
    //     socket.broadcast.to(socket.room).emit("noti_frd_new_con", '!!! '+socket.username+' has joined '+newroom+'. !!!');
    //     // io.emit('updaterooms',rooms,newroom);
    //     console.log('*** '+socket.username+':'+oldroom+':'+socket.room);
    //     console.log(rDefault);
    //     console.log(rRoom1);
    //     console.log(rRoom2);
    //     console.log("----------------------");
    //   }
    // });

  	//	update header my guest name (me only)
  	// io.to(socket.id).emit('guest_login',{name: guest_name});

  	//	send new_user noti (all user)
  	// io.emit('new_user_con',{name: guest_name});

  	//	send 'user_list' (me only)
  	// io.to(socket.id).emit('user_list-me',{u: users});

  	//	update new_user in 'user_list' (except me)
  	// socket.broadcast.emit('update_list',{name: guest_name});

  	//-----------------------------------------------

  	// var zmq = require('zmq');
  	// console.log("Connecting to hello node-c server…");
  	// var requester = zmq.socket('req');

  	// requester.on("message", function(reply) {
  	//   console.log("Received reply: " +reply.toString());
  	// });

  	// requester.connect("tcp://localhost:5555");
  	// console.log("Sending request …");
  	// requester.send("1"+"::"+counter.toString()+"::"+guest_name);

	//-----------------------------------------------

/*
  socket.on('disconnect',function(){
  	// console.log('disconnected');
  	var address = socket.handshake.address;
  	console.log(address);

  	var sockId = socket.id;
  	console.log(sockId);
  	console.log(users[sockId]+' disconnected');
  	
  	//	delete this user from 'user_list' (except me)
  	socket.broadcast.emit('remove_list',{name: users[sockId]});
  	delete users[sockId];
  	console.log(users);

  	//	zmq test
  	requester.close();
  });

  socket.on('user',function(data){
  	console.log('>>> chat from ...\nuser: '+data.username+'\nmsg: '+data.test);
	socket.broadcast.emit('user',data);
	//	zmq test
	requester.send("2"+"::"+counter.toString()+"::"+"123");
  });

  socket.on('new_user_con',function(data){
  	console.log('new user has joined ['+data.name+'].');
  	io.emit('new_user_con',data);
  });

  socket.on('change_name',function(data){
  	console.log('user \''+data.oldname+'\' is now known as \''+data.newname+'\'.');
  	var sockId = socket.id;
  	users[sockId]=data.newname;
  	io.emit('change_name',data);

  	//	zmq test
  	// requester.send("2"+"::"+counter.toString()+"::"+data.newname);
  });
*/

});

//	make the http server listen on port 3000
http.listen(3000, function(){
  console.log('listening on *:3000');
});