var url = "http://localhost:3000";
var socket = io.connect(url);
var myname ="";
var friendname ="";
var mygroupname ={};

setInterval(function(){
  for(var key in mygroupname){
    socket.emit('fetch_mb_group',myname,mygroupname[key]);
  }
},5000);

socket.on('connection', function() {
  console.log("user conected");
});

socket.on('get_mb_group',function(x){
  if(x[1]!=myname){
    return;
  }
  else{
    // var strid = "mblist-"+x[2];
    // alert(strid);
    
    var elem = document.getElementById(x[2]+"-ul");
    //var elem = document.getElementById(strid);
    if(elem==undefined){
      //$("."+x[2]+" ul").id = strid;
      console.log("no element to insert subgrouplist");
      // for(var i=4;i<x.length;i++){
      //   var ulparent = document.getElementById(x[2]+"-ul");
      //   var desc = document.createElement("li");
      //   desc.innerHTML = x[i];
      //   ulparent.appendChild(desc);
      // }

    }
    else{
      var listcheck = false;
      var select = document.getElementById(x[2]+"-ul");
      var num = parseInt(x[3]);
      var countLi = $("#"+x[2]+"-ul").children().length;
      //console.log("cmp(n/o): "+x[3]+" "+select.options.length.toString());
      if(countLi!=num){
        while(countLi > 0){
          select.remove(0);
        }
        // alert("remove");
        for(var i=4;i<x.length;i++){
          var ulparent = document.getElementById(x[2]+"-ul");
          var desc = document.createElement("li");
          desc.innerHTML = "<span class='glyphicon glyphicon-user icon-custom subgroup-icon' aria-hidden='true'></span>"+x[i];
          ulparent.appendChild(desc);
          listcheck = true;
        }
      }
      else{
        // alert("equal");
      }
    }
  }
});

socket.on("get_history",function(x){
  if(x[1]!=myname){
    return;
  }
  else{
    var i;
    for(i=x.length-1;i>1;i=i-3){
      if(x[i-2]==myname){
        var para = document.createElement("P");
        para.setAttribute("id",x[i]);
        para.className='me';
        var newText = document.createTextNode(x[i-1]);
        para.appendChild(newText);
        document.getElementById("recv").appendChild(para);
      }
        else{ // friendname
          var para = document.createElement("P");
          para.setAttribute("id",x[i]);
          para.className='msg';
          var newText = document.createTextNode(x[i-2]+": "+x[i-1]);
          para.appendChild(newText);
          document.getElementById("recv").appendChild(para);
        }
      }
      //  update scrolltop
      var elem_recv = document.getElementById("recv");
      elem_recv.scrollTop = elem_recv.scrollHeight;
    }
  });


socket.on("fetchmsg",function(forme,fromuser,touser,msg,tstamp){
  if(mygroupname[touser]!=undefined && forme==myname && touser==friendname){
    if(fromuser!=myname){
      var para = document.createElement("P");
      para.className='msg';
      var newText = document.createTextNode(fromuser+": "+msg);
      para.appendChild(newText);
      document.getElementById("recv").appendChild(para);

      //  update scrolltop
      var elem_recv = document.getElementById("recv");
      elem_recv.scrollTop = elem_recv.scrollHeight;
    }
    return;

  }
  if(forme==myname && fromuser==friendname){
    //  if res = 0 
    //  don't show a msg
    var res = 0;
    var elem = document.getElementsByClassName('msg');
    //  res = 1
    //  show a msg
    if(elem.length==0){
      res = 1;
      console.log("not found element")
    }
    else{
      if(tstamp != elem[elem.length-1].id){
        res = 1;
        console.log("found element");
      }
    }

    if(res==1){
      var para = document.createElement("P");
      para.className='msg';
      var newText = document.createTextNode(fromuser+": "+msg);
      para.appendChild(newText);
      document.getElementById("recv").appendChild(para);

      //  update scrolltop
      var elem_recv = document.getElementById("recv");
      elem_recv.scrollTop = elem_recv.scrollHeight;
    }
  }
});

socket.on("req_join_group",function(forme,fromuser,msg){
  // console.log(forme+":"+fromuser+":"+msg);
  if(forme!=myname){
    //  not found me
    return;
  }

  if(mygroupname[msg]!=undefined)
    console.log("join group: "+msg);
  else
    console.log("not found req group");

  socket.emit("rep_join_group",forme,msg);
});

socket.on("req_add_group_name",function(x){
  if(x.indexOf(myname)==-1){
    //  not found me
    return;
  }
  //  for check when fetch msg from group
  mygroupname[x[1]]=x[1];
  // alert(mygroupname);
  console.log("mygroupname: "+mygroupname[x[1]]);

  var elem = document.getElementById(x[1]);

  if(elem==undefined){
    var acc = document.getElementById("subgrouplist");
    var div1 = document.createElement("div");
    div1.className = "accordion-section";
    div1.id = x[1]+"-section";
    
    var a1 = document.createElement("a");
    a1.className = ("accordion-section-title");
    a1.href = "#"+x[1];
    a1.onclick = setGroupName(x[1]);
    a1.innerHTML = "<span class='glyphicon glyphicon-home icon-custom group-icon' aria-hidden='true'></span>"+x[1];
    
    var div2 = document.createElement("div");
    div2.className = "accordion-section-content";
    div2.id = x[1];

    var ul1 = document.createElement("ul");
    ul1.id = x[1]+"-ul";

    //leave btn
    // var li1 = document.createElement("li");
    // var bt_leave = document.createElement("button");
    // bt_leave.type = "button";
    // bt_leave.className = "btn btn-danger btn-xs";
    // bt_leave.innerHTML = "Leave Group";
    // bt_leave.onclick = leaveThisGroup(x[1]);
    
    // li1.appendChild(bt_leave);
    // ul1.appendChild(li1);

    // var li1 = document.createElement("li");
    // var leave_btn = document.createElement("a");
    // leave_btn.src = "#";
    // leave_btn.onclick = leaveThisGroup(x[1]);
    // leave_btn.innerHTML = "Leave Group";

    // li1.appendChild(leave_btn);
    // ul1.appendChild(li1);
    
    // var li1 = document.createElement("li");
    // var bt_leave = document.createElement("input");
    // bt_leave.type = "button";
    // bt_leave.style.cssText ="color:red";
    // bt_leave.value = "Leave Group";
    // bt_leave.onclick = leaveThisGroup(x[1]);
    // li1.appendChild(bt_leave);
    // ul1.appendChild(li1);

    div2.appendChild(ul1);
    div1.appendChild(a1);
    div1.appendChild(div2);
    acc.appendChild(div1);

    openAccordion();
    mygroupname[x[1]]=x[1];

  }
  else{
    // alert("you have this group");
    console.log("you have this group");
  }
  // //  update scrolltop
  // var elem_online = document.getElementById("onlinelist");
  // elem_online.scrollTop = elem_online.scrollHeight;    
});

var setGroupName = function(friendName){
  return function() {
    friendname=friendName;
    document.getElementById("msgInput").placeholder = "Input messages to "+friendname+" group";
    var elem = document.getElementById("chat-title");
    elem.innerHTML="Group room: <span class='cur-friend'>"+friendname+"</span>";
    //  list chat history group
    socket.emit("update_chat_group",friendname);
    alert(friendname);

    var cMsg = document.getElementsByClassName("msg");
    var cMe = document.getElementsByClassName("me");
    while(cMsg[0]){
      cMsg[0].parentNode.removeChild(cMsg[0]);
    }
    while(cMe[0]){
      cMe[0].parentNode.removeChild(cMe[0]);
    }
    var contain_bt = document.getElementById("chat-title");
    var bt_leave = document.createElement("button");
    bt_leave.type = "button";
    bt_leave.className = "btn btn-danger btn-xs leave_btn";
    bt_leave.id = friendname+"-leave-btn";
    bt_leave.innerHTML = "Leave Group";
    bt_leave.onclick = leaveThisGroup(friendname);
    
    contain_bt.appendChild(bt_leave);
  };
}

var leaveThisGroup = function(grName){
  return function() {
    var ret = confirm("Do you want to leave '"+grName+"' group?")
    if(ret==true){
      socket.emit('leavegroup',myname,grName);

      var del_elem = document.getElementById(grName+"-section");
      del_elem.parentNode.removeChild(del_elem);
      $("#"+grName+"-leave-btn").remove();
      delete mygroupname[grName];

      document.getElementById("chat-title").innerHTML = "Chat room: messages";
      document.getElementById("msgInput").placeholder = "Select your friend first.";

      var cMsg = document.getElementsByClassName("msg");
      var cMe = document.getElementsByClassName("me");
      while(cMsg[0]){
        cMsg[0].parentNode.removeChild(cMsg[0]);
      }
      while(cMe[0]){
        cMe[0].parentNode.removeChild(cMe[0]);
      }

      // socket.emit('leavegroup',myname,grName);
    }
  };
}

socket.on("updategroups",function(x){
  // alert(myname+":"+x[1]);
  if(x[1]!=myname){
    return;
  }
  for(var i=3;i<x.length;i++){
    var acc = document.getElementById("subgrouplist");
    var div1 = document.createElement("div");
    div1.className = "accordion-section";
    div1.id = x[i]+"-section";
    
    var a1 = document.createElement("a");
    a1.href = "#"+x[i];
    a1.onclick = setGroupName(x[i]);
    a1.innerHTML = "<span class='glyphicon glyphicon-home icon-custom group-icon' aria-hidden='true'></span>"+x[i];
    
    var div2 = document.createElement("div");
    div2.className = "accordion-section-content";
    div2.id = x[i];

    var ul1 = document.createElement("ul");
    ul1.id = x[i]+"-ul";

    //leave btn
    // var li1 = document.createElement("li");
    // var bt_leave = document.createElement("button");
    // bt_leave.type = "button";
    // bt_leave.className = "btn btn-danger btn-xs";
    // bt_leave.innerHTML = "Leave Group";
    // bt_leave.onclick = leaveThisGroup(x[i]);
    
    // li1.appendChild(bt_leave);
    // ul1.appendChild(li1);

    // var li1 = document.createElement("li");
    // var leave_btn = document.createElement("a");
    // leave_btn.src = "#";
    // leave_btn.onclick = leaveThisGroup(x[1]);
    // leave_btn.innerHTML = "Leave Group";

    // li1.appendChild(leave_btn);
    // ul1.appendChild(li1);

    // var li1 = document.createElement("li");
    // var bt_leave = document.createElement("input");
    // bt_leave.type = "button";
    // bt_leave.style.cssText ="color:red";
    // bt_leave.value = "Leave Group";
    // bt_leave.onclick = leaveThisGroup(x[i]);
    // li1.appendChild(bt_leave);
    // ul1.appendChild(li1);

    div2.appendChild(ul1);
    div1.appendChild(a1);
    div1.appendChild(div2);
    acc.appendChild(div1);

    openAccordion();
    mygroupname[x[i]]=x[i];
  }

});

socket.on("updateusers",function(data){

  var myNode = document.getElementById("userlist");
  var items = myNode.getElementsByTagName("li");
  for(var i=0;i<items.length;i++){
    // console.log(items[i].id+":"+items[i].value);
    //  user in mylist not found in newlist
    if(data.indexOf(items[i].id)==-1){
      //console.log("delete");
      var del_elem = document.getElementById(items[i].id);
      del_elem.parentNode.removeChild(del_elem);
    }
    else{
      //console.log("exist");
    }
  }


  var colorcount = 1;
  for(var key in data){
    if(key!=0){
      var elem = document.getElementById(data[key]);
      if(elem==undefined && data[key]!=myname){
        var para2 = document.createElement("LI");
        para2.setAttribute("id",data[key]);
        var bt = document.createElement("a");
        if (colorcount % 2 == 1) {
          bt.className="onlineusr even";
        }
        else{
          bt.className="onlineusr ood";
        }
        bt.href="#";
        bt.innerHTML = "<span class='glyphicon glyphicon-comment icon-custom' aria-hidden='true'></span>"+data[key];
        var frd = data[key];
        bt.onclick = setFriendName(data[key]);
        para2.appendChild(bt);
        myNode.appendChild(para2);
        colorcount++;
      }
    }
  }

  //  update scrolltop
  var elem_online = document.getElementById("onlinelist");
  elem_online.scrollTop = elem_online.scrollHeight;

});

var setFriendName = function(friendName){
  return function() {
    friendname=friendName;
    document.getElementById("chat-title").innerHTML = "Chat room: <span class='cur-friend'>"+friendname+"</span>";
    document.getElementById("msgInput").placeholder = "Input messages to "+friendname;
    socket.emit("update_private_frd",friendname);
    //alert(friendname);

    var cMsg = document.getElementsByClassName("msg");
    var cMe = document.getElementsByClassName("me");
    while(cMsg[0]){
      cMsg[0].parentNode.removeChild(cMsg[0]);
    }
    while(cMe[0]){
      cMe[0].parentNode.removeChild(cMe[0]);
    }
  };
}

function _createGroup(newgroup,friendsname){
  // alert(newgroup+"::"+friendsname);
  socket.emit('new_group',newgroup,myname+":"+friendsname);
}

function createGroup(){

  var newgroup = $("#groupname").val();
  //document.getElementById("btGroup").disabled = true;

  if(newgroup.trim()=="" || newgroup.indexOf(' ')>-1){
    alert("Please input group name.\n(group name should not include space.)");
    return;
  }

  var userchecked = "";
  //console.log(gname);
  $("#mbody .checkbox label input").each(function(idx, input) {
    var elem = $(input);
    var list = elem.attr('value'); 
    if (elem.is(':checked')) {
      userchecked += list+':';
    };
  });

  if (userchecked == "") {
    alert("Please select your friend");
    return;
  }
  else{
    //userchecked = userchecked.slice(0,-1);
  }
  console.log("gname: "+newgroup);
  console.log(userchecked);
  //document.getElementById("btGroup").disabled = false;
  _createGroup(newgroup,userchecked);
  $('#group-modal').modal('hide');

}

function sendMsg(){
  if(friendname==""){
    alert("Please select your friend.");
    return;
  }
  if(myname==""){
    alert("Please login before send a msg.");
    return;
  }

  var msg = document.getElementById("msgInput").value;

  //  show msg (me) ----------------------------------
  var para = document.createElement("P");
  para.className='me';
  var newText = document.createTextNode(msg);
  para.appendChild(newText);
  document.getElementById("recv").appendChild(para);

  //  update scrolltop
  var elem_recv = document.getElementById("recv");
  elem_recv.scrollTop = elem_recv.scrollHeight;

  //  clear textfield
  document.getElementById("msgInput").value="";
  // -------------------------------------------------

  if(mygroupname[friendname]==undefined){
    //  send private room
    socket.emit('sendchat', myname, friendname, msg);
  }
  else{
    //  send to group
    socket.emit('send_to_group',myname,friendname,msg);
  }
}

function sendName(){
  var newname = document.getElementById("loginName").value
  //  notice (join)

  if(newname.trim()=="" || newname.indexOf(' ')>-1){
    alert("Please input your name.\n(your name should not include space.)");
    return;
  }

  var elem = document.getElementById(newname);

  if(elem != null) {
    alert("This name is used.\nPlease input new name.");
    document.getElementById("loginName").value = "";
    return;
  }

  else { 
    document.getElementById("loginName").disabled = true;
    myname = newname;

    var elem = document.getElementById("yourname");
    elem.innerHTML="Login as <span class='online'>"+myname+"</span>";
    socket.emit("adduser",myname);
  }
}

// ------------------------ DataModel ------------------------ //

function search(){
  var key = $("#searchBox").val();
  console.log(key);
}