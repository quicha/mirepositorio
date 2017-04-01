

  #include <stdio.h>
  #include <string.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <time.h>


int abrir_socket();

void enlazar_a_puerto(int , int);

int escuchando(int, int);




int main (int argc, char *argv[])
{  
	int puerto=8880;
	char mensaje[1024], date[80], tamanyo[100];
	struct sockaddr_in dir_servidor, dir_cliente;
	unsigned int long_dir_cliente;
	int socket2;
	int n, enviados, recibidos,listener,size;
	int proceso,i;
	time_t tiempo;
	struct tm *tm1; 
	
 	char respuesta[1024]="\n\r";
	FILE *archivoconf, *asset;
	char *metodo, *version,*document,*document_root, *uri;
        int topeclientes=10;
	
	document_root=malloc(1024);
	strcpy(document_root,"/home/p1/sd/sd1");
	uri=malloc(1024);
	strcpy(uri,"/index.html");
        
	/* Comprobar los argumentos */
	
	//GESTION DE LECTURA DEL FICHERO DE CONFIGURACION//
	
	if(argc>1){
	  /*printf("holaaaaaaaaaa\n\r");*/
	if(strcmp(argv[1],"-c")==0){ //aqui controlamos esta ./servidor  [argv[0]] -c argv[1] archivoconf argv[2]
	  archivoconf=fopen(argv[2], "r"); // lo abre únicamente para lectura, de ahí viene la r 
	  if(archivoconf==NULL){
	    printf("No está abriendo el archivito\n\r");
	  }
	    if(archivoconf!=NULL){
	      char* linea;
	      size_t length=0;
	      ssize_t read;
	      
	      
	      
	      while((read=getline(&linea, &length, archivoconf))!=-1){
		char *almacen;
		almacen=strtok(linea," ");
		if(strcmp(almacen, "DocumentRoot")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Document_root:%s\n\r",almacen);
		  strcpy(document_root,almacen);
		}else if(strcmp(almacen, "Listen")==0){
		  almacen=strtok(NULL,"\n");
		  puerto=atoi(almacen);
		}
		else if(strcmp(almacen, "MaxClients")==0){
		  almacen=strtok(NULL,"\n");
		  topeclientes=atoi(almacen);
		}
		else if(strcmp(almacen, "DirectoryIndex")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Uri:%s\n\r",almacen);
		  strcpy(uri,almacen);
		}
	      }
		
	      fclose(archivoconf);
	      if(linea){
		free(linea);
	      }
	    }
	  }
	  else{
	    puerto=atoi(argv[1]);   
	   
	      //aqui controlamos estas ./servidor puerto y ./servidor puerto -c archivoconf
	    if(argc>2){
	      if(strcmp(argv[2], "-c")==0){
		 
	    archivoconf=fopen(argv[3], "r"); // lo abre únicamente para lectura, de ahí viene la r 
	  if(archivoconf==NULL){
	    printf("No está abriendo el archivito\n\r");
	  }
	    if(archivoconf!=NULL){
	      char* linea;
	      size_t length=0;
	      ssize_t read;
	      
	      
	      
	      while((read=getline(&linea, &length, archivoconf))!=-1){
		char *almacen;
		almacen=strtok(linea," ");
		if(strcmp(almacen, "DocumentRoot")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Document_root:%s\n\r",almacen);
		  strcpy(document_root,almacen);
		}else if(strcmp(almacen, "Listen")==0){
		  almacen=strtok(NULL,"\n");
		  puerto=atoi(almacen);
		}
		else if(strcmp(almacen, "MaxClients")==0){
		  almacen=strtok(NULL,"\n");
		  topeclientes=atoi(almacen);
		}
		else if(strcmp(almacen, "DirectoryIndex")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Document_root:%s\n\r",almacen);
		  strcpy(uri,almacen);
		}
	      }
		
	      fclose(archivoconf);
	      if(linea){
		free(linea);
	      }
	    }
	  
	  
	    }
	    }
	}
	/**** Paso 1: Abrir el socket ****/

	listener = abrir_socket();

	/**** Paso 2: Establecer la dirección (puerto) de escucha ****/

	enlazar_a_puerto(listener,puerto); 

	/**** Paso 3: Preparar el servidor para escuchar ****/

	escuchando(listener,topeclientes);

	/**** Paso 4: Esperar conexiones ****/


	while (1)
	{
		fprintf(stderr, "Esperando conexión en el puerto %d...\n\r", puerto);
		long_dir_cliente = sizeof (dir_cliente);
		socket2 = accept (listener, (struct sockaddr *)&dir_cliente, &long_dir_cliente);
		/* s2 es el socket para comunicarse con el cliente */
		/* s puede seguir siendo usado para comunicarse con otros clientes */
		if (socket2 == -1)
		{
			break; /* salir del bucle */
		}
		/* crear un nuevo proceso para que se pueda atender varias peticiones en paralelo */
		proceso = fork();
		if (proceso == -1) exit(1);
		if (proceso == 0) /* soy el hijo */
		{
			close(listener); /* el hijo no necesita el socket general */

			/**** Paso 5: Leer el mensaje ****/
			

			n = sizeof(mensaje);
			recibidos = read(socket2, mensaje, n);
			if (recibidos == -1)
			{
				fprintf(stderr, "Error de lectura del mensaje\n\r"); //strcat lo que hace es concatenar y va guardando el resultado en respuesta
				    strcat(respuesta,"HTTP/1.1 500 Internal Server Error\n");
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
					    strcat(respuesta, "<html> <title>Error 500</title>\n<h1> Error 500: Error Interno. </h1> </html>");
					    n = strlen(respuesta); //calcula el numero de caracteres 
			
			enviados = write(socket2, respuesta, n); //escribimos la respuesta
				    
			
			    if (enviados == -1 || enviados < n){
					    fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
					    close(listener);
					   
				    }
			close(socket2);
			}
			mensaje[recibidos] = '\0'; /* pongo el final de cadena */
			/**Anyadido por nosotros*/
			printf("Mensaje [%d]: %s\n\r", recibidos, mensaje);
			  
			  //printf("Split \"%s\"\n", mensaje);
			  
			  metodo= strtok(mensaje, " ");	//metodo
			  //printf("%s\n", argu0);
			  
			  if(metodo!=NULL)
			    uri = strtok(NULL, " ");	//recurso
			  //printf("%s\n", argu1);
			  
			  if(uri!=NULL)
			    version = strtok(NULL, " ");	//version

	printf("%s\n", version);
	
	tiempo = time(NULL);
	tm1 = localtime(&tiempo);
	strftime(date, 80, "%H:%M:%S, %A de %B de %Y", tm1);


	//LLAMADA DE FUNCIONES A LOS DISTINTOS MÉTODOS
	
			if(strcmp(metodo,"GET")==0){
			  
				strcat(document_root, uri); 
				//anyade un bloque de memoria a otro, aqui anyado la ruta interna del servidor con la externa del cliente
		
				if(strcmp(version,"HTTP/1.1")>=0){
					asset=fopen(document_root, "r"); //buscamos en la ruta 	
					/*printf("Document_root: %s\n", document_root);*/
					if(asset==NULL){ //no lo encontramos
						
						strcpy(respuesta, "HTTP/1.1 404 not found\n\r");
						  
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
					    strcat(respuesta, "<html> <title>Error 404</title>\n<h1> Error 404: Archivo no encontrado en el servidor </h1> </html>");
						
					}
					else{	//lo encontramos			 
						
						strcat(respuesta, "HTTP/1.1 200 OK\n\r");
						strcat(respuesta, "\n\r");
						fseek(asset,0L,SEEK_END);
						size=ftell(asset);
						sprintf(tamanyo,"%d",size);
						fseek(asset,0L,SEEK_SET);
						document=malloc(size);
						if(document){
						  fread(document,1,size,asset);
						}
						fclose(asset);
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: ");
						strcat(respuesta,tamanyo);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
strcat(respuesta,"\n\r");

						
						if(document){
						  strcat(respuesta,document);
						}
						strcat(respuesta, "\n\r");
						
					}
				}
				else{
				  
				  strcat(respuesta, "HTTP/1.1 505, HTTP version not supported\n");
				  	  
					    strcat(respuesta, "Connection: close\n\r");
					    strcat(respuesta, "Content-Length: 90");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Content-Type: txt/html\n\r");
					    strcat(respuesta, "Server: Servidor SD\n\r");
					    strcat(respuesta, "Date: ");
					    strcat(respuesta, date);
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "<html> <title>Error 500</title>\n<h1> Error 505: Versión de HTTP no soportada </h1> </html>");
				  
				}
				
				
			n=strlen(respuesta);
		enviados = write(socket2, respuesta, n);
		mensaje[recibidos] = '\0';
			if (enviados == -1 || enviados < n)
			{
				fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
				close(listener);
				
			}
			close(socket2);
			}else if(strcmp(metodo,"HEAD")==0){
			  
			  strcat(document_root, uri);
				asset=fopen(document_root, "r");
				if(strcmp(version,"HTTP/1.1")>=0){
					if(asset==NULL){
						strcpy(respuesta, "HTTP/1.1 404 not found\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 143");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n");
						strcat(respuesta, "<html> <title> Error 404</title>\n<h1> Error 404: Archivo no encontrado en el servidor  </h1> \n O a lo mejor no queriamos que lo encontraras... </html>");
					}
					else{
					        printf("Entrando ok\n");
						strcpy(respuesta, "HTTP/1.1 200 OK\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 0");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type:\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n");
					}

					
				}else{
					strcat(respuesta,"HTTP/1.1 505 HTTP version not supported\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 90");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "<html> <title>  Error 505 </title>\n<h1> Error 505: Version De HTTP no soportada. </h1> </html>");				}
				if (asset!=NULL){ 
					fclose(asset);
}
			  
			  
			}else if(strcmp(metodo,"DELETE")==0){
			 
			  
			  
			  int aux;
				aux=-1;
				char name[strlen(uri)];
				strcat(document_root, uri);
				/*Operamos para el metodo DELETE*/
				if(strcmp(version,"HTTP/1.1")>=0){
					aux=remove(document_root);
					if(aux!=0){
						strcat(respuesta,"HTTP/1.1 404 not found\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 143");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "<html> <title> Error 404</title>\n<h1> Error 404: Archivo no encontrado en el servidor  </h1> \n O a lo mejor no queriamos que lo encontraras... </html>");
					}else{
						strcat(respuesta,"HTTP/1.1 200 OK\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 0");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
					}
				}else{
					strcat(respuesta,"HTTP/1.1 505 HTTP version not supported\n");
					strcat(respuesta, "Connection: close\n\r");
					strcat(respuesta, "Content-Length: 90");
					strcat(respuesta, "\n\r");
					strcat(respuesta, "Content-Type: txt/html\n\r");
					strcat(respuesta, "Server: Servidor SD\n\r");
					strcat(respuesta, "Date: ");
					strcat(respuesta, date);
					strcat(respuesta, "\n\r");
					strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
					strcat(respuesta, "<html> <title>  Error 505 </title>\n<h1> Error 505: Version De HTTP no soportada. </h1> </html>");
} 
			  
			  
			  
			}else if(strcmp(metodo,"PUT")==0){
			
			  
			  
			  
			  	strcat(document_root, uri);
				
				/*Operamos para el metodo PUT*/
				if(strcmp(version,"HTTP/1.1")>=0){
					asset=fopen(document_root, "w");
					
					if(asset==NULL){
						strcat(respuesta,"HTTP/1.1 403 Forbidden\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 77");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n");
						strcat(respuesta, "<html> <title>Error 403</title>\n<h1> Error 403: Acceso Denegado. </h1> </html>");
						
					}else{
						strcpy(respuesta, "HTTP/1.1 201 CREATED\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 0");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n");

					}
					

				}else{
					strcat(respuesta,"HTTP/1.1 505 HTTP version not supported\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 90");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "<html> <title>  Error 505 </title>\n<h1> Error 505: Version De HTTP no soportada. </h1> </html>");				}
				if (asset!=NULL){
					fclose(asset);
}
			  
			  
			  
			 
			}else{ //METODO NO SOPORTADO!!!!!!!!
			  strcpy(respuesta, "HTTP/1.1 405 method not allowed\n");
				strcat(respuesta, "Connection: close\n\r");
				strcat(respuesta, "Content-Length: 82");
				strcat(respuesta, "\n\r");
				strcat(respuesta, "Content-Type: txt/html\n\r");
				strcat(respuesta, "Server: Servidor SD\n\r");
				strcat(respuesta, "Date: ");
				strcat(respuesta, date);
				strcat(respuesta, "\n\r");
				strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
strcat(respuesta,"<html> <title>  Error 405 </title>\n<h1> Error 405: Método no permitido. </h1> </html>");
			}
			
			
			//ESCRIBIENDO LA RESPUESTA EN EL SOCKET
			
			
			n = strlen(respuesta);
			enviados = write(socket2, respuesta, n);
			if (enviados == -1 || enviados < n)
			{
				fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
				close(listener);
				return 1;
			}

						

			close(socket2);
			exit(0); /* el hijo ya no tiene que hacer nada */
		}
		else /* soy el padre */
		{
			close(socket2); /* el padre no usa esta conexión */
		}

	}

	/**** Paso 7: Cerrar el socket ****/
	close(listener);
	printf("Socket cerrado\n\r");
	return 0;
}
}




/** metodo para leer el fichero de configuracion */
/*
void leerConfig(char* filename){		
  FILE * fichero;
  char cadena[100];
  fichero = fopen(filename,"r");
  
  if(fichero==NULL){
    printf("ERROR al abrir fichero\nAnyadir envio de ERROR\n");
  }else{
    if( fgets(cadena, 100 , fichero) != NULL ){
      maxCliente=atoi(cadena);
      if( fgets(cadena, 100 , fichero) != NULL )
	strcpy(uri,cadena);
    }
  }
  if(fichero!=NULL)
    fclose(fichero);
}*/

/*int enviar(int socket2,char* respuesta, int listener){
  int n,enviados;
 	n = strlen(respuesta);
	printf("Enviar respuesta [%d bytes]: %s\n\r", n, respuesta);
	enviados = write(socket2, respuesta, n);//le envia al cliente lo que escribe en el socket
	printf("Numero de bytes enviados: %d\n", enviados);

	if (enviados == -1 || enviados < n){
		fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
		close(listener);
		return 1;
	}

	printf("Respuesta enviada\n\r");
 
}*/


int abrir_socket(){

	int sock=socket(PF_INET,SOCK_STREAM,0); //creo el socket 
      
	if(sock==-1){
		fprintf(stderr,"Error de apertura de socket\n\r");
	 }              
    	return sock;
  }


void enlazar_a_puerto(int listener, int puerto){

	  
	  struct sockaddr_in server_addr;
	  server_addr.sin_family=AF_INET;
	  server_addr.sin_port=htons(puerto);
	  server_addr.sin_addr.s_addr=htonl(INADDR_ANY); /*significa cualquier ip del servidor*/

  

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
	fprintf(stderr,"No es posible escuchar en ese puerto\n\r"); //cliente 11
	close(listener);
	return 1;
    }
	    
	  printf("Enlazado a puerto\n");
}