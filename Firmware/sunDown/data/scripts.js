function fixInversion(val)
{
	return 1023-Math.ceil(val)
}

function postLightingSettings(intensity, temperature, mode)
{
	var Cmax = 1000 // max color temp

	if(mode == 0){
		return //do nothing, the device should be in sync mode
	}
	//mode 1 is "MAX" intensity mode
	if(mode == 1){
		if(temperature>0){
			var warm = intensity*((Cmax-Math.abs(temperature))/Cmax)
			var cool = intensity
		}
		if(temperature<0){
			var warm = intensity
			var cool = intensity*((Cmax-Math.abs(temperature))/Cmax)
		}
		if(temperature==0){
			var warm = intensity
			var cool = intensity
		}
	}
	//mode 2 is "normed" intensity mode
	console.log("temp")
	console.log(temperature)
	if(mode == 2){
		var warm = (((temperature+Cmax))/(2*Cmax)) * intensity
		var cool = ((2*Cmax-(temperature+Cmax))/(2*Cmax)) * intensity
	}

	console.log("preval")
	console.log(warm)
	console.log(cool)
	warm = fixInversion(warm)
	cool = fixInversion(cool)
	console.log("vals")
	console.log(warm)
	console.log(cool)

	let xhr = new XMLHttpRequest()
	xhr.open("POST",'',true)
	xhr.setRequestHeader("Content-Type","text/plain")
	xhr.send(JSON.stringify({"cmd":"set", "warm":warm, "cool":cool}))
	
}

function postLightingMode(mode)
{
	let xhr = new XMLHttpRequest()
	xhr.open("POST",'',true)
	xhr.setRequestHeader("Content-Type","text/plain")
	xhr.send(JSON.stringify({"cmd":"mode", "mode":mode}))
}


var modeSlider = document.getElementById("mode")
modeSlider.oninput = function() {
	console.log(this.value)
	//eventually send this to the esp

}

var intensitySlider = document.getElementById("intensity")
intensitySlider.onchange = function() {
	var int =  parseInt(document.getElementById('intensity').value, 10)
	var temp = parseInt(document.getElementById("temperature").value, 10)
	var mode = parseInt(document.getElementById("mode").value, 10)
	postLightingSettings(int, temp, mode)
}

var temperatureSlider = document.getElementById("temperature")
temperatureSlider.onchange = function() {
	var int =  parseInt(document.getElementById('intensity').value, 10)
	var temp = parseInt(document.getElementById("temperature").value, 10)
	var mode = parseInt(document.getElementById("mode").value, 10)
	postLightingSettings(int, temp, mode)
}

intensitySlider.ondblclick = function(){
	this.value=512
}

temperatureSlider.ondblclick = function(){
	this.value=0
}