//------------------------------------------------
//-------- JQuery scripts

$(document).ready(function(){
		
	$('#button10').on('click', function(){
		sendmessage(1);
		$(this).toggleClass('on');
	});
	$('#button20').on('click', function(){
		sendmessage(2);
		$(this).toggleClass('on');
	});
	$('#button30').on('click', function(){
		sendmessage(3);
		$(this).toggleClass('on');
	});
	$('#button40').on('click', function(){
		sendmessage(4);
		$(this).toggleClass('on');
	});
	Conect();
});

//------------------------------------------------
//-------- WebSocket scripts

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

	conect.onopen = function(evt) {
		output.innerHTML = '<span style="color: white;">CONECTADO</span>';
		sendmessage('E');
	};
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
	var cadena = evt;
	var msgs = cadena.split(';');
	for(var i=0; i< msgs.length;i++){
		//alert(msgs[i]);
		var msg = msgs[i];
		chaine = msg.split(':');
		//alert(chaine[0])
		switch(chaine[0]){
			case '1':
			case '2':
			case '3':
			case '4':
				//alert("Buena Cadena "+chaine[0]+"   "+chaine[1]);
				aux = 'button'+chaine[0]+'0';
				print = document.getElementById(aux);	
				//print.innerHTML = chaine[1];
				if (chaine[1] == 'OFF'){
					$(print).removeClass('ison');
					//print.innerHTML ='<span style="color: red;">OFF</span>' 
				}
				else {
					$(print).addClass('ison');
					//print.innerHTML = '<span style="color: green;">ON</span>'
				}
				break;
			default:
				//alert("Mala cadena");
				break;
		}
			
	}
}
function sendmessage(id){			
	conect.send(id);
}
