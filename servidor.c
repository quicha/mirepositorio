

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


/*definicion de funciones*/


int abrir_socket();
void enlazar_a_puerto(int , int);




int main(){

	/*char * advice[]={"hola\n\r"};*/
	const int puertodefinido=8880; /*de momento, esto es una constante*/
	struct sockaddr_in client_addr;
	unsigned int long_client_addr;
        int socket2;
	int listener= abrir_socket();  /*llamamos a la funcion que crea el socket que escuchara las petis*/

	if(listener==-1){
	printf("No se puede escuchar\n");
	return ; // acabamos la ejecucion del programa

	}

//una vez creamos el socket, lo enlazamos con el puerto que queremos que escuche
	enlazar_a_puerto(listener,puertodefinido); //BIND

//una vez enlazado el puerto, invocamos a la funcion listen, pasandole el listener y el numero tope de clientes.
	


        if(listen(listener,10) == -1){
	fprintf(stderr,"No es posible escuchar en ese puerto\n\r"); //saltara cuando llegue el cliente 11
        close(listener);
        return 1;
}
	printf("Enlazado a puerto\n");



	while(1){
	fprintf(stderr,"Esperando conexión en el puerto 8880...\n\r");
	long_client_addr= sizeof (client_addr);
	socket2= accept(listener, (struct sockaddr *) &client_addr, &long_client_addr); //segundo socket necesario para comunicarse 
	//el cliente, recuerda, se necesitan 2, uno para estar escuchando y el otro para contestar 
		if(socket2==-1){
		fprintf(stderr, "No es posible conectar socket secundario\n\r");
 		}
        printf("Atendiendo al cliente\n");
	/*char* msg=advice[rand() %5];
	send(socket2,msg,strlen(msg),0);
	msg=NULL;*/
	printf("Cerrando el socket\n");
	close(socket2);
	}

return 0;
}
	
//envio de consejos



//FUNCION DE APERTURA DEL SOCKET//

int abrir_socket(){

	int sock=socket(PF_INET,SOCK_STREAM,0); //creo el socket //PF_INET es el dominio del socket,
     
	if(sock==-1){
	fprintf(stderr,"Error de apertura de socket\n\r");//socket de stream flujo de conexion, protocolo
	 }              
	return sock;
}

//FUNCION DE ENLACE AL PUERTO, establecemos la dirección al puerto de escucha//


void enlazar_a_puerto(int listener, int puertodefinido){

	 //va a contener toda la informacion del puerto y dominio
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=(in_port_t)htons(8880);//lo tengo distinto a adri porque lo mio es un int, no un char
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY); /*significa cualquier ip del servidor*/

//para evitar problemas a la hora de reutilizar el puerto...

/*	int reusar=1;

	if(setsockopt(socket,SOL_SOCKET,SO_REUSEADOR,(char*)&reuse,sizeof(int)) == -1){
	printf("No es posible reusar\n");
	}
*/

	int c= bind(listener, (struct sockaddr *) &server_addr, sizeof(server_addr)); //Le pasamos el socket, direccion de memoria de la 	estructura, tamanyo
 
	if(c==-1){
         fprintf(stderr,"No se puede enlazar al puerto: dirección ya está en uso\n\r");
         close(listener);
	}

	printf("Establecido el puerto de escucha\n\r");
}

















