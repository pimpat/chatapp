var storage = sessionStorage;

function keepSession(sessionID, userID, state){
	var val = userID+":"+state;
	storage.setItem(sessionID, val); 
	var value = storage.getItem(sessionID); 
	console.log(value);

}
function destroySession(id){
	storage.removeItem(id);
	var total = storage.length;
	console.log(total);
}
