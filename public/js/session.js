var storage = sessionStorage;

function keepSession(sessionID, userID, state){
	var val = userID+":"+state;
	/* สร้าง key ชื่อ 'textSize' เพื่อเอามาเก็บค่า 'large' */
	storage.setItem(sessionID, val); 


	/* แสดงค่าของ key ที่ชื่อ 'textSize' */
	var textSize = storage.getItem(sessionID); 
	console.log(textSize); /* = large */

	/* แสดงชื่อของ key ตัวแรกที่ได้เก็บไว้ */
	var firstKey = storage.key(0); 
	console.log(firstKey); /* = textSize */

	/* แสดงจำนวนข้อมูลที่จัดเก็บไว้ทั้งหมด */
	var total = storage.length;
	console.log(total); /* = 2 */

	/* ลบข้อมูลของ key ที่ชื่อ 'view' */
	// storage.removeItem('view'); 
	// var view = storage.getItem('view');
	// console.log(view); /* = null */

	/* ลบข้อมูลทั้งหมด */
	// storage.clear(); 
	// var textSize = storage.getItem('textSize');
	// console.log(textSize); /* = null */
}
function destroySession(id){
	storage.removeItem(id);
	var total = storage.length;
	console.log(total);
}

function guid() {
	function s4() {
		return Math.floor((1 + Math.random()) * 0x10000)
		.toString(16)
		.substring(1);
	}
	return s4() + s4() + '-' + s4() + '-' + s4() + '-' +
	s4() + '-' + s4() + s4() + s4();
}