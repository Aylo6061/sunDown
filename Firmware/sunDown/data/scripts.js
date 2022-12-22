function fixInversion(val)
{
	return 1023-Math.ceil(val)
}

function postLightingSettings()
{
	var intensity =  parseInt(document.getElementById('intensity').value, 10)
	var temperature = parseInt(document.getElementById("temperature").value, 10)
	var mode = parseInt(document.getElementById("mode").value, 10)

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
	if(mode == 2){
		var cool = (((temperature+Cmax))/(2*Cmax)) * intensity
		var warm = ((2*Cmax-(temperature+Cmax))/(2*Cmax)) * intensity
	}

	warm = fixInversion(warm)
	cool = fixInversion(cool)

	console.log(warm)
	console.log(cool)

	let xhr = new XMLHttpRequest()
	xhr.open("POST",'',true)
	xhr.setRequestHeader("Content-Type","text/plain")
	xhr.send(JSON.stringify({"cmd":"set", "warm":warm, "cool":cool, "mode":mode, "intensity":intensity, "temperature":temperature}))
	
}

function postLightingMode(mode)
{
	let xhr = new XMLHttpRequest()
	xhr.open("POST",'',true)
	xhr.setRequestHeader("Content-Type","text/plain")
	xhr.send(JSON.stringify({"cmd":"mode", "mode":mode}))
}

var intensitySlider = document.getElementById("intensity")
intensitySlider.onchange = function() {
	postLightingSettings()
}

var temperatureSlider = document.getElementById("temperature")
temperatureSlider.onchange = function() {
	postLightingSettings()
}

var modeSlider = document.getElementById("mode")
modeSlider.onchange = function() {
	postLightingSettings()
}

intensitySlider.ondblclick = function(){
	this.value=512
}

temperatureSlider.ondblclick = function(){
	this.value=0
}

function loadState()
{
	let xhr = new XMLHttpRequest()
	xhr.open("GET",'/status.js',false)
	xhr.send()
	if(xhr.readyState == 4 && xhr.status == 200)
	{
		console.log(xhr.responseText)
		state = JSON.parse(xhr.responseText)
	}

	document.getElementById('mode').value = parseInt(state["mode"],10)
	document.getElementById('intensity').value = parseInt(state["intensity"],10)
	document.getElementById('temperature').value = parseInt(state["temperature"],10)
}

document.addEventListener("load",loadState())