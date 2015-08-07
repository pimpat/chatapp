// crate group modal
var alluserlist = 0;
function createGroupModal() {
  if(myname==""){
    alert("Please login before create group.");
    return;
  }
  $('#group-modal').modal('show');
  var i = 1;  
  var value = [];
  var child = $("#mbody").children("div");
  if (child){
    child.remove();
  }
    // Textbox
    $("#mbody").append("<div class='form-inline'><label>Group name: </label><input type='text' id='groupname' class='form-control'></div>");
    // Userlist
    $("#mbody").append("<div><br><label>Select your friend</label></div>");
    $("#userlist li").each(function(idx, li) {   
      var product = $(li);
      value[i] = product.attr('id')
      if (value[i] != myname) {
        $("#mbody").append("<div class='checkbox'><label><input type='checkbox' id='u"+i+"' value='"+value[i]+"'>"+value[i]+"</label></div>"); 
      };
      i++;
    });
    alluserlist = i-1;
  }

// Create group
function creategroupData() {
  var userchecked = '';
  var i = 1;
  var gname = $("#groupname").val();
  $("#mbody .checkbox label input").each(function(idx, input) {
    var elem = $(input);
    var list = elem.attr('value'); 
    if (elem.is(':checked')) {
      userchecked += list+':';
    };
  });
  userchecked = userchecked.slice(0,-1);
  console.log(userchecked);
  $('#group-modal').modal('hide');
}

// Dropdown grouplists 
function close_accordion_section() {
  $('.accordion .accordion-section-title').removeClass('active');
  $('.accordion .accordion-section-content').slideUp(300).removeClass('open');
}

function openAccordion() {
  $('.accordion-section-title').click(function(e) {
    // Grab current anchor value
    var currentAttrValue = $(this).attr('href');
    console.log(currentAttrValue);

    if($(e.target).is('.active')) {
      close_accordion_section();
    }else {
      close_accordion_section();

      // Add active class to section title
      $(this).addClass('active');
      // Open up the hidden content panel
      $('.accordion ' + currentAttrValue).slideDown(300).addClass('open'); 
    }

    e.preventDefault();
  });
}

function Keyload(){
  console.log("Enter key enable");
  // Input with enter key
  $('#loginName').keypress(function (e) {
   var key = e.which;
   if(key == 13){
    sendName();
    return false;  
  }
});  
  $('#msgInput').keypress(function (e) {
   var key = e.which;
   if(key == 13){
    sendMsg();
    return false;  
  }
});  
  $('#searchBox').keypress(function (e) {
   var key = e.which;
   if(key == 13){
     search();
     return false;  
   }
 });  
}

window.onbeforeunload = function (e) {
    destroySession(sessionId);
  };