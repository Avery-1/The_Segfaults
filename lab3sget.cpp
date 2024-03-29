//
//lab3sget.c
//Written by: Gordon Griesel
//Modified by: Loay Samha
//Date:       Winter 2019
//Purpose:    Example of HTTP request using SSL.
//            Used to contact a web server and receive a page as text.
//--------------------------------------------------------------------
//Some code in this program comes from samples at the following sources:
//   http://fm4dd.com/openssl/sslconnect.htm
//   https://ubuntuforums.org/showthread.php?t=2217101
//
//Build:
//  gcc lab3sget.c -Wall -olab3sget -lssl -lcrypto
//  
//Usage:
//  ./lab3sget <host> <page>
//
//The program will contact a web server with host and page, and receive
//back the page requested or other information.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
//
//optional includes.
//These includes were removed during development.
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <signal.h>
//#include <errno.h>
//#include <sys/time.h>
//#include <stdlib.h>
//#include <memory.h>
//#include <ifaddrs.h>
//#include <net/if.h>
//#include <netinet/in.h>
//#include <stdarg.h>
//
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
//
#define PORT 443
#define USERAGENT "CMPS-3350"
//added int type
const int MAX_READ_ERRORS = 100;

int showHighScores(int argc, char *argv[]) {
	BIO *ssl_setup_bio(void);
	void show_cert_data(SSL *ssl, BIO *outbio, const char *hostname);
	void set_to_non_blocking(const int sock);
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    BIO *outbio = NULL;
    //added a const on the SSL_METHOD *method;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    char req[1000];
    int req_len;
	char hostname[256] = "www.cs.csub.edu";
	char pagename[256] = "/~lsamha/3350/lab07/scorepage.php";
    int port = PORT;
    int bytes, nreads, nerrs;
    char buf[256];
	int ret;
	//Get any command-line arguments.
	if (argc > 1)
		strcpy(hostname, argv[1]);
	if (argc > 2)
		strcpy(pagename, argv[2]);
	//Setup the SSL BIO
	outbio = ssl_setup_bio();
	//Initialize the SSL library
    if(SSL_library_init() < 0)
       	BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");
    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	//next 2 lines of code are not currently needed.
	//SSL_MODE_AUTO_RETRY flag of the SSL_CTX_set_mode call.
	//SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
	//Setup the socket used for connection.
    host = gethostbyname(hostname);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        BIO_printf(outbio, "%s: Cannot connect to host %s [%s] on port %d.\n",
	       	argv[0], hostname, inet_ntoa(addr.sin_addr), port);
    }
	//Connect using the SSL certificate.
    ssl = SSL_new(ctx); 
    SSL_set_fd(ssl, sd);
    SSL_connect(ssl);
	//Show the certificate data
	//--------------------show_cert_data(ssl, outbio, hostname);
	//A non-blocking socket will make the ssl_read() not block.
	set_to_non_blocking(sd);
	//Send the http request to the host server.
	sprintf(req, "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n",
	       	pagename, hostname, USERAGENT);    
    req_len = strlen(req);
    ret = SSL_write(ssl, req, req_len);
	if (ret <= 0)
		fprintf(stderr, "ERROR: SSL_write\n"); 
     	//return down fflush(stderrr)
	fflush(stderr);
	//Get data returned from the server.
	//First, do priming read.
	//We can take this approach because our socket is non-blocking.
	//Start with an error condition.
	bytes = -1;
	memset(buf, '\0', sizeof(buf));
	while(bytes <= 0){
	    bytes = SSL_read(ssl, buf, sizeof(buf));
		//A slight pause can cause fewer reads to be needed.
		usleep(10000);
	}
	//A successful priming read was accomplished.
	//Now read all the data.
	nreads = 1;
	//Allow for some read errors to happen, while getting the complete data.
	nerrs = 0;
	while(bytes >= 0 && nerrs < MAX_READ_ERRORS){
        write(STDOUT_FILENO, buf, bytes);
        memset(buf, '\0', sizeof(buf));
		++nreads;
        bytes = SSL_read(ssl, buf, sizeof(buf));
		if (bytes == 0) ++nerrs; else nerrs = 0;
		//A slight pause can cause fewer reads to be needed.
		usleep(20000);
    }
	//printf("\nn calls to ssl_read(): %i\n", nreads); fflush(stdout);
	//Cleanup.
    SSL_free(ssl);
    close(sd);
    SSL_CTX_free(ctx);
	return 0;
}

BIO *ssl_setup_bio(void){
	//Setup the ssl BIO, basic I/O abstraction.
	//https://www.openssl.org/docs/man1.1.0/man3/bio.html
    BIO *bio = NULL;
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    bio = BIO_new(BIO_s_file());
    bio = BIO_new_fp(stdout, BIO_NOCLOSE);
	return bio;
}

void show_cert_data(SSL *ssl, BIO *outbio, const char *hostname) {
	//Display ssl certificate data here.
	//Get the remote certificate into the X509 structure
	printf("--------------------------------------------------------------\n");
	printf("Certificate data...\n");
	X509 *cert;
	X509_NAME *certname;
	printf("calling SSL_get_peer_certificate(ssl)\n");
	cert = SSL_get_peer_certificate(ssl);
	if (cert == NULL)
		printf("Error: Could not get a certificate from: %s.\n",
		       	hostname);
	else
		printf("Retrieved the server's certificate from: %s.\n",
		       	hostname);
	//extract various certificate information
	certname = X509_NAME_new();
	certname = X509_get_subject_name(cert);
	//display the cert subject here
	if (BIO_printf(outbio, "Displaying the certificate subject data:\n")
	       	< 0)
		fprintf(stderr, "ERROR: BIO_printf\n");
	X509_NAME_print_ex(outbio, certname, 0, 0);
	if (BIO_printf(outbio, "\n\n") < 0)
		fprintf(stderr, "ERROR: BIO_printf\n");
	printf("--------------------------------------------------------------\n");
}

void set_to_non_blocking(const int sock){
	//Set a socket to be non-blocking.
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0)
	{
		perror("ERROR: fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock, F_SETFL, opts) < 0)
	{
		perror("ERROR: fcntl(O_NONBLOCK)");
		exit(EXIT_FAILURE);
	}
}


