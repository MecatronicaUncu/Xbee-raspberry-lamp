var ip = location.host;
//alert(ip);

var divid = 1;
var wsUri = 'ws://'+ip+'/xbee';
var output;

function Conect() { 
	output = document.getElementById("conect"); ConectWebSocket();
	}  
function ConectWebSocket() { 
	conect = new WebSocket(wsUri); 
	conect.onopen = function(evt) { output.innerHTML = '<span style="color: black;">CONECTADO</span>' }; 
	conect.onclose = function(evt) {output.innerHTML = '<span style="color: orange;">DESCONECTADO: '+evt+'</span>'};
	conect.onmessage = function(evt) {onMessage(evt.data)}; //get a message
	conect.onerror = function(evt) { output.innerHTML = '<span style="color: red;">ERROR</span>' }; //error.
} 
function Discontect(){
	conect.close(); //close websocket
}
function onMessage(evt){
		
	var print
	var aux
	var chaine = evt;
	chaine = chaine.split(':');
	aux = 'div'+chaine[0]+'0';
	print = document.getElementById(aux);	
	//print.innerHTML = chaine[1];
	if (chaine[1] == 'OFF') {print.innerHTML ='<span style="color: red;">OFF</span>' }
	else {print.innerHTML = '<span style="color: green;">ON</span>'}
		
	
}
function sendmessage(id){			
	conect.send(id);
}
	