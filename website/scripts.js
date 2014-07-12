//------------------------------------------------
//-------- JQuery scripts

$(document).ready(function(){
		
	$('#button10').on('click', function(){
		if(isConnected){
			sendmessage(1);
			$(this).toggleClass('on');
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button20').on('click', function(){
		if(isConnected){
			sendmessage(2);
			$(this).toggleClass('on');
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button30').on('click', function(){
		if(isConnected){
			sendmessage(3);
			$(this).toggleClass('on');
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button40').on('click', function(){
		if(isConnected){
			sendmessage(4);
			$(this).toggleClass('on');
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	Connect();
});

//------------------------------------------------
//-------- WebSocket scripts

// Obtener dirección del servidor websocket
var ip = location.host;
var wsUri = 'ws://'+ip+'/xbee';

// Div para imprimir mensajes de la conexión
var output;

// isConnected indica si hay una conexión activa
// setConnected cambia el estado de isConnected
var isConnected = false;
function setConnected(isConn){
	isConnected = isConn;
	
	if(isConnected){
		// Conectados, activar botones
		$(".Sensores").animate({opacity:1},500);
	}
	else{
		// Desconectados, desactivar botones y mostrar mensaje
		$(".Sensores").animate({opacity:0.4},500);
	}
}

// Connect, función llamada por html5 para solicitar una nueva conexión
function Connect() {
	if(isConnected==false){
		output = document.getElementById("conect"); 
		ConectWebSocket();
	}
}

// ConectWebSocket, crea websocket. Callbacks.
function ConectWebSocket() { 
	conect = new WebSocket(wsUri); 
	conect.onopen = function(evt) {
		output.innerHTML = '<span style="color: white;">CONECTADO</span>';
		sendmessage('E');
		setConnected(true);
		console.log('isConnected: '+isConnected);
	};
	conect.onclose = function(evt) {
		output.innerHTML = '<span style="color: orange;">DESCONECTADO: '+evt+'</span>';
		setConnected(false);
		console.log('isConnected: '+isConnected);
	};
	conect.onmessage = function(evt) {onMessage(evt.data)}; //get a message
	conect.onerror = function(evt) {
		output.innerHTML = '<span style="color: red;">ERROR</span>'; 
		setConnected(false);
		console.log('isConnected: '+isConnected);
	}; //error.
}

// Disconnect, función llamada por html5 para cerrar conexión
function Disconnect(){
	conect.close(); //close websocket
}

// Callback cuando se recibe algo por el websocket
function onMessage(evt){
	var cadena = evt;
	var msgs = cadena.split(';');
	for(var i=0; i< msgs.length;i++){
		//alert(msgs[i]);
		var chaine = msgs[i].split(':');
		//alert(chaine[0])
		switch(chaine[0]){
			case '1':
			case '2':
			case '3':
			case '4':
				//alert("Buena Cadena "+chaine[0]+"   "+chaine[1]);
				var aux = 'button'+chaine[0]+'0';
				var button = document.getElementById(aux);	
				//print.innerHTML = chaine[1];
				if (chaine[1] == 'OFF'){
					$(button).removeClass('ison');
					//button.innerHTML ='<span style="color: red;"></span>' 
				}
				else {
					$(button).addClass('ison');
					//button.innerHTML = '<span style="color: green;">ON</span>'
				}
				break;
			default:
				//alert("Mala cadena");
				break;
		}
			
	}
}

// Callback, enviar un mensaje por el websocket
function sendmessage(id){
	if(isConnected){
		conect.send(id);
	}
}

// Función de test, cambia el estado de isConnected
function test(){
	if(isConnected)
		setConnected(false);
	else
		setConnected(true);
	console.log('isConnected: '+isConnected);
}
