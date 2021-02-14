#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define MAXBUFFER 512
#define PORT 9999

void clearWSA();

int main() {

#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "Si è verificato un errore in WSAStartup()\n");
		return EXIT_FAILURE;
	}
#endif

	char echo[MAXBUFFER];
	int echoLength;
	printf("Inserisci la stringa da inviare al server: ");
	scanf("%s", echo);

	echoLength = strlen(echo);
	if(echoLength > MAXBUFFER) {
		fprintf(stderr, "La lunghezza della parola e' piu' lunga della dimensione massima.\n");
		clearWSA();
		return EXIT_FAILURE;
	}

	// creazione socket
	int clientSocket;
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket < 0) {
		fprintf(stderr, "Si e' verificato un errore durante la creazione della socket.\n");
		clearWSA();
		return EXIT_FAILURE;
	}

	// costruzione indirizzo server
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	// invio della stringa echo al server
	if (sendto(clientSocket, echo, echoLength, 0, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) != echoLength) {
		fprintf(stderr, "Si e' verificato un errore durante l'invio dei dati.\n");
		closesocket(clientSocket);
		clearWSA();
		return EXIT_FAILURE;
	}

	// ritorno della stringa echo
	unsigned int fromSize;
	struct sockaddr_in fromAddress;
	fromSize = sizeof(fromAddress);
	int respLength;
	char buffer[MAXBUFFER];
	respLength = recvfrom(clientSocket, buffer, MAXBUFFER, 0, (struct sockaddr*) &fromAddress, &fromSize);

	if (serverAddress.sin_addr.s_addr != fromAddress.sin_addr.s_addr) {
		fprintf(stderr, "Errore: hai ricevuto un pacchetta da una sorgente sconosciuta.\n");
		closesocket(clientSocket);
		clearWSA();
		return EXIT_FAILURE;
	}
	buffer[respLength] = '\0';
	printf("\nRicevuto: %s\n", buffer);

	closesocket(clientSocket);
	clearWSA();


	system("pause>nul");
	return EXIT_SUCCESS;
}

void clearWSA() {
#ifdef WIN32
	WSACleanup();
#endif
}
