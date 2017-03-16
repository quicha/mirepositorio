

  #include <stdio.h>
  #include <string.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <unistd.h>

  /*definicion de funciones*/


  int abrir_socket();
  void enlazar_a_puerto(int , int);
  int escuchando(int, int);
  void aceptando_conexion(int);
  


    int main(){

	  /*char * advice[]={"hola\n\r"};*/
	  const int puertodefinido=8880; /*de momento, esto es una constante*/
	  const int tope;
	  
	  int socket2;
	  int listener= abrir_socket();  /*llamamos a la funcion que crea el socket que escuchara las petis*/
	  

  //una vez creamos el socket, lo enlazamos con el puerto que queremos que escuche
	  enlazar_a_puerto(listener,puertodefinido); //BIND

  //una vez enlazado el puerto, invocamos a la funcion listen, pasandole el listener y el numero tope de clientes.
	  
	  escuchando(listener,tope);

	  aceptando_conexion(listener);

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


  //FUNCIÓN LISTEN

  int escuchando( int listener, int numero){
  
    int c= listen(listener,numero);
    
      if(c==-1){
	fprintf(stderr,"No es posible escuchar en ese puerto\n\r"); //saltara cuando llegue el cliente 11
	close(listener);
	return 1;
    }
	    
	  printf("Enlazado a puerto\n");
  }
	  

  //FUNCION ACEPTANDO CONEXION


    void aceptando_conexion(int listener){
	    struct sockaddr_in client_addr;
	    unsigned int long_client_addr;
	    char mensaje[1024], respuesta[1024], date[80], tamanyo[100];
	    char *metodo, *uri, *version, *document_root, *document;
	    int n, recibidos,enviados,size;
	    FILE *asset;
      
	      while(1){
		    fprintf(stderr,"Esperando conexión en el puerto 8880...\n\r");
		    long_client_addr= sizeof (client_addr);
		    int socket2= accept(listener, (struct sockaddr *) &client_addr, &long_client_addr); //segundo socket necesario para comunicarse 
		    //el cliente, recuerda, se necesitan 2, uno para estar escuchando y el otro para contestar 
		      if(socket2==-1){
			    fprintf(stderr, "No es posible conectar socket secundario\n\r");
		      }
		      printf("Atendiendo al cliente\n");

		      /*char* msg=advice[rand() %5]; 
		      send(socket2,msg,strlen(msg),0);
		      msg=NULL;*/
		      printf("Cerrando el socket\n");
		      /*close(socket2);*/
	    
			  n = sizeof(mensaje);
			  recibidos = read(socket2, mensaje, n);
			  if (recibidos == -1){
				    fprintf(stderr, "Error de lectura del mensaje\n\r"); //strcat lo que hace es concatenar y va guardando el resultado en respuesta
				    strcat(respuesta,"HTTP/1.1 500 Internal Server Error\n");/*
					    strcat(respuesta, "Connection: close\n\r");
					    strcat(respuesta, "Content-Length: 96");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Content-Type: txt/html\n\r");
					    strcat(respuesta, "Server: Servidor SD\n\r");
					    strcat(respuesta, "Date: ");
					    strcat(respuesta, date);
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "<html> <title>Error 500</title>\n<h1> Error 500: Error Interno. </h1> </html>");*/
					    n = strlen(respuesta); //calcula el numero de caracteres 
				   
			enviados = write(socket2, respuesta, n); //escribimos la respuesta
				    
			
			    if (enviados == -1 || enviados < n){
					    fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
					    close(listener);
					   
				    }
			close(socket2);
			}
			mensaje[recibidos] = '\0'; /* pongo el final de cadena */
			printf("Mensaje [%d]: %s\n\r", recibidos, mensaje); 
			metodo=strtok(mensaje, " "); /* Comprobamos el metodo HTTP*/ // la funcion strtok divide en tokens el mensaje 
			uri=strtok(NULL, " ");
			version=strtok(NULL," ");
			
			if(strcmp(metodo, "GET")==0){
				strcat(document_root, uri); //anyade un bloque de memoria a otro, aqui anyado la ruta interna del servidor con la externa del cliente
				printf("%s\n",document_root);
				     printf("%s\n",uri);
				if(strcmp(version,"HTTP/1.1")>=0){
					asset=fopen(document_root, "r"); //buscamos en la ruta 					
					if(asset==NULL){ //no lo encontramos
						strcpy(respuesta, "HTTP/1.1 404 not found\n\r");
						
					}
					else{	//lo encontramos			 
						strcat(respuesta, "HTTP/1.1 200 OK\n\r");
						fseek(asset,0L,SEEK_END);
						size=ftell(asset);
						sprintf(tamanyo,"%d",size);
						fseek(asset,0L,SEEK_SET);
						document=malloc(size);
						if(document){
						  fread(document,1,size,asset);
						}

						
						if(document){
						  strcat(respuesta,document);
						}
						strcat(respuesta, "\n\r");
						
					}
				}
			
			n=strlen(respuesta);
		enviados = write(socket2, respuesta, n);
			if (enviados == -1 || enviados < n)
			{
				fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
				close(listener);
				
			}
			close(socket2);

		}
		else{
		  close(socket2);
		}
		
		

      }
    }








