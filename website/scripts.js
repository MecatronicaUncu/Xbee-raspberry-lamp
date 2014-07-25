//------------------------------------------------
//-------- JQuery scripts

// Array con el estado de los botones
var buttonStateArray = new Array(false,false, false, false);

$(document).ready(function(){
		
	$('#button10').on('click', function(){
		if(isConnected){
			sendmessage(1);
			$(this).toggleClass('on');
			buttonStateArray[0] = !buttonStateArray[0];
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button20').on('click', function(){
		if(isConnected){
			sendmessage(2);
			$(this).toggleClass('on');
			buttonStateArray[1] = !buttonStateArray[1];
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button30').on('click', function(){
		if(isConnected){
			sendmessage(3);
			$(this).toggleClass('on');
			buttonStateArray[2] = !buttonStateArray[2];
		}
		else{
			alert('Debe conectarse para utilizar los interruptores');
		}
	});
	$('#button40').on('click', function(){
		if(isConnected){
			sendmessage(4);
			$(this).toggleClass('on');
			buttonStateArray[3] = !buttonStateArray[3];
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
	try{
		conect = new WebSocket(wsUri);
	}
	catch(err){
		console.log("Error creando WebSocket: "+err);
		return false;
	}
	conect.onopen = function(evt) {
		output.innerHTML = '<span style="color: white;">CONECTADO</span>';
		setConnected(true);
		sendmessage('E');
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
	for(var i=0; i< msgs.length-1;i++){
		var chaine = msgs[i].split(':');
		//alert(chaine[0])
		var buttonId = chaine[0];
		var buttonState = chaine[1];
		switch(buttonId){
			case '1':
			case '2':
			case '3':
			case '4':
				//alert("Buena Cadena "+chaine[0]+"   "+chaine[1]);
				var aux = 'button'+buttonId+'0';
				var button = document.getElementById(aux);	
				//print.innerHTML = chaine[1];
				if(buttonState == 'F'){
					$(button).removeClass('ison');
					//button.innerHTML ='<span style="color: red;"></span>'
					// Cambiar el estado del boton tambien si no ha sido cambiado
					if(buttonStateArray[buttonId-1]==true){
						$('#button'+buttonId+'0').removeClass('on');
						buttonStateArray[buttonId-1] = false;
					}
				}
				else if(buttonState == 'N'){
					$(button).addClass('ison');
					//button.innerHTML = '<span style="color: green;">ON</span>'
					if(buttonStateArray[buttonId-1]==false){
						$('#button'+buttonId+'0').addClass('on');
						buttonStateArray[buttonId-1] = true;
					}
				}
				else{
					console.log("Mala cadena (else): "+buttonId+":"+buttonState);
				}
				break;
			default:
				console.log("Mala cadena (default): "+buttonId+":"+buttonState);
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
function toggleIsConnected(){
	if(isConnected)
		setConnected(false);
	else
		setConnected(true);
	console.log('isConnected: '+isConnected);
}
