on({id: "0_userdata.0.timer.now", change: "any"}, function(obj)
{

var state = getState("0_userdata.0.timer.now").val;
var now_time = new Date(state);

function time(){
	var timetoend = getState("0_userdata.0.timer.timetoend").val;
	var alexa_time = new Date(timetoend);

	var diff = alexa_time.getTime() - now_time.getTime();

	var days = Math.floor(diff / (1000*60*60*24));
	diff = diff % (1000*60*60*24);
	var hrs = Math.floor(diff / (1000*60*60));
	diff = diff % (1000*60*60);
	var min = Math.floor(diff / (1000*60));
	diff = diff % (1000*60);
	var sec = Math.floor(diff / 1000);

var together = sec + 60 * min + 60 * 60 * hrs + 60 * 60 * 24 * days;

	setState("0_userdata.0.timer.sectoend", together);
}
time();

});