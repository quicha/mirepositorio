#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main (int argc, char *argv[])
{
	char *servidor_ip;
	char *servidor_puerto;
	char mensaje[1024],*metodo, *uri, *version, respuesta[1024];
	struct sockaddr_in direccion;
	int s;
	int n, enviados, recibidos;

	/* Comprobar los argumentos */
	if (argc !=  3)
	{
		fprintf(stderr, "Error. Debe indicar la direccion del servidor (IP y Puerto) y el mensaje a enviar\r\n");
		fprintf(stderr, "Sintaxis: %s  <puerto> <metodo>\n\r", argv[0]);
		fprintf(stderr, "Ejemplo : %s  8880 \"GET /index.html HTTP/1.1\"\n\r", argv[0]);
		return 1;
	}

	/* Tomar los argumentos */		
	servidor_ip = "127.0.0.1";
	servidor_puerto = argv[1];
	metodo = "GET";
	uri = argv[2];
	version = "HTTP/1.1";

	//printf("\n\rEnviar mensaje \"%s\" a %s:%s...\n\r\n\r", mensaje, servidor_ip, servidor_puerto);

	/**** Paso 1: Abrir el socket ****/

	s = socket(AF_INET, SOCK_STREAM, 0); /* creo el socket */
	if (s == -1)
	{
		fprintf(stderr, "Error. No se puede abrir el socket\n\r");
		return 1;
	}
	printf("Socket abierto\n\r");

	/**** Paso 2: Conectar al servidor ****/		

	/* Cargar la dirección */
	direccion.sin_family = AF_INET; /* socket familia INET */
	direccion.sin_addr.s_addr = inet_addr(servidor_ip);
	direccion.sin_port = htons(atoi(servidor_puerto));
	
	if (connect(s, (struct sockaddr *)&direccion, 	sizeof (direccion)) == -1)
	{
		fprintf(stderr, "Error. No se puede conectar al servidor\n\r");
		close(s);
		return 1;
	}
	printf("Conexion establecida\n\r");

	/**** Paso 3: Enviar mensaje ****/
	mensaje[0]='\0';
	strcat(mensaje, metodo);
	strcat(mensaje, " ");
	strcat(mensaje, uri);
	strcat(mensaje, " ");
	strcat(mensaje, version);

	n = strlen(mensaje);
	enviados = write(s, mensaje, n);
	mensaje[0]='\0';
	if (enviados == -1 || enviados < n)
	{
		fprintf(stderr, "Error enviando el mensaje\n\r");
		close(s);
		return 1;
	}

	printf("Mensaje enviado\n\r");

	/**** Paso 4: Recibir respuesta ****/

	n = sizeof(respuesta) - 1;
	recibidos = read(s, respuesta, n);
	if (recibidos == 1)
	{
		fprintf(stderr, "Error recibiendo respuesta\n\r");
		close(s);
		return 1;
	}
	respuesta[recibidos] = '\0';
	printf("Respuesta [%d bytes]: %s\n\r", recibidos, respuesta);

	/**** Paso 5: Cerrar el socket ****/
	close(s);
	printf("Socket cerrado. Comunicacion finalizada\n\r");

	return 0;
}
