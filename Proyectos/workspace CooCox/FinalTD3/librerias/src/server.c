/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Ethernet remote device and sensor
 * UDP and HTTP interface
        url looks like this http://baseurl/password/command
        or http://baseurl/password/
 *
 * Chip type           : Atmega88 or Atmega168 or Atmega328 with ENC28J60
 * Note: there is a version number in the text. Search for tuxgraphics
 *********************************************/

#include "stm32f4xx.h"
//#include <stdlib.h>
#include "stdio.h"
#include <string.h>
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "server.h"
#include "TFTssd1963.h"
#include "stm32_ub_led.h"


static void TFT_show(int op);
static void TFT_save(int op);

// listen port for tcp/www (max range 1-254)
#define MYWWWPORT 80
//
// listen port for udp
#define MYUDPPORT 1200

#define BUFFER_SIZE 1400
static uint8_t buf[BUFFER_SIZE+1];

// the password string (only the first 5 char checked), (only a-z,0-9,_ characters):
static char password[]="secret"; // must not be longer than 9 char
static t_eServerState s_lastState = eServerIdle;

static t_eServerState serverTcp(uint16_t plen);
static void serverUdp(void);


//
uint8_t verify_password(char *str)
{
	// the first characters of the received string are
	// a simple password/cookie:
	if (strncmp(password,str,5)==0){
		return(1);
	}
	return(0);
}

// takes a string of the form password/commandNumber and analyse it
// return values: -1 invalid password, otherwise command number
//                -2 no command given but password valid
//                -3 valid password, no command and no trailing "/"
int8_t analyse_get_url(char *str)
{
	uint8_t loop=1;
	uint8_t i=0;
	while(loop){
		if(password[i]){
			if(*str==password[i]){
				str++;
				i++;
			}else{
				return(-1);
			}
		}else{
			// end of password
			loop=0;
		}
	}
	// is is now one char after the password
	if (*str == '/'){
		str++;
	}else{
		return(-3);
	}
	// check the first char, garbage after this is ignored (including a slash)
	if (*str < 0x3a && *str > 0x2f){
		// is a ASCII number, return it
		return(*str-0x30);
	}
	return(-2);
}

// answer HTTP/1.0 301 Moved Permanently\r\nLocation: password/\r\n\r\n
// to redirect to the url ending in a slash
uint16_t moved_perm(uint8_t *buf)
{
	uint16_t plen;
	plen=fill_tcp_data_p(buf,0, "HTTP/1.0 301 Moved Permanently\r\nLocation: ");
	plen=fill_tcp_data(buf,plen,password);
	plen=fill_tcp_data_p(buf,plen, "/\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
	plen=fill_tcp_data_p(buf,plen, "<h1>301 Moved Permanently</h1>\n");
	plen=fill_tcp_data_p(buf,plen, "add a trailing slash to the url\n");
	return(plen);
}


// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf)
{
	uint16_t plen;
	plen=fill_tcp_data_p(buf,0, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
	plen=fill_tcp_data_p(buf,plen, "<center><p>LED is: ");
	if (eServerLedOn == s_lastState){
		plen=fill_tcp_data_p(buf,plen, "<font color=\"#00FF00\"> ON</font>");
	}else{
		plen=fill_tcp_data_p(buf,plen, "OFF");
	}
	plen=fill_tcp_data_p(buf,plen, " <small><a href=\".\">[refresh status]</a></small></p>\n<p><a href=\".");
	if (eServerLedOn == s_lastState){
		plen=fill_tcp_data_p(buf,plen, "/0\">LED off</a><p>");
	}else{
		plen=fill_tcp_data_p(buf,plen, "/1\">LED on</a><p>");
	}
	plen=fill_tcp_data_p(buf,plen, "</center><hr><br>Final de Tecnicas Digitales 3, Gabriel Schvert\n");
	return(plen);
}


t_eServerState server(t_enc28j60Ctrl * apCtrl) {
	t_eServerState state = eServerIdle;
	uint16_t plen;

	// get the next new packet:
	plen = recv_ip_arp_udp_tcp(BUFFER_SIZE, buf);

	/*plen will ne unequal to zero if there is a valid
	 * packet (without crc error) */
	if(plen==0){
		return 0;
	}

	// arp is broadcast if unknown but a host may also
	// verify the mac address by sending it to
	// a unicast address.
	if(eth_type_is_arp_and_my_ip(buf,plen)){
		make_arp_answer_from_request(buf);
		return 0;
	}

	// check if ip packets are for us:
	if(eth_type_is_ip_and_my_ip(buf,plen)==0){
		return 0;
	}

	switch (buf[IP_PROTO_P]) {
	case IP_PROTO_ICMP_V:
		// a ping packet
		if (buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {
			make_echo_reply_from_request(buf,plen);
		}
		break;

	case IP_PROTO_TCP_V:
		// a tcp packet
		state = serverTcp(plen);
		break;

	case IP_PROTO_UDP_V:
		// a udp packet
		serverUdp();
		break;

	default:
		break;
	}

	return state;
}

static t_eServerState serverTcp(uint16_t plen) {
	t_eServerState state = eServerIdle;
	uint16_t       dat_p;
	int8_t         cmd;

	// compare only the lower byte
	if (buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==MYWWWPORT){
		if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
			make_tcp_synack_from_syn(buf);
			// make_tcp_synack_from_syn does already send the syn,ack
			return state;
		}
		if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
			init_len_info(buf); // init some data structures
			// we can possibly have no data, just ack:
			dat_p=get_tcp_data_pointer();
			if (dat_p==0){
				if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
					// finack, answer with ack
					make_tcp_ack_from_any(buf);
				}
				// just an ack with no data, wait for next packet
				return state;
			}
			if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
				// head, post and other methods:
				//
				// for possible status codes see:
				// http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
				plen=fill_tcp_data_p(buf,0,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>");
				goto SENDTCP;
			}
			if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
				plen=fill_tcp_data_p(buf,0, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
				plen=fill_tcp_data_p(buf,plen, "<p>Usage: http://host_or_ip/password_string</p>\n");
				goto SENDTCP;
			}
			cmd=analyse_get_url((char *)&(buf[dat_p+5]));
			// for possible status codes see:
			// http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
			if (cmd==-1){
				plen=fill_tcp_data_p(buf,0, "HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>");
				goto SENDTCP;
			}
			if (cmd==1){
				state = eServerLedOn;
				s_lastState = eServerLedOn;
			}
			if (cmd==0){
				state = eServerLedOff;
				s_lastState = eServerLedOff;
			}
			if (cmd==-3){
				// redirect to add a trailing slash
				plen=moved_perm(buf);
				goto SENDTCP;
			}
			// if (cmd==-2) or any other value
			// just display the status:
			plen=print_webpage(buf);
			//
			SENDTCP:
			make_tcp_ack_from_any(buf); // send ack for http get
			make_tcp_ack_with_data(buf,plen); // send data
		}
	}

	return state;
}

static void serverUdp(void) {
	//uint8_t payloadlen=0;
	//char str[30];
	//char msg[30];
	int i=0;	//indice para recorrer el buffer de ethernet
	static int mostrarPagina = 0;
	static int guardarPagina = 1;
	int16_t ancho = 0;
	int16_t alto = 0;

	// udp start, we listen on udp port 1200=0x4B0
	if (buf[UDP_DST_PORT_H_P]==4 && buf[UDP_DST_PORT_L_P]==0xb0) //verfico que sea mi puerto UDP
	{
		//payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;

		//Toogleo el led rojo para saber que recivi un ROGER
		if(		buf[UDP_DATA_P]==0x52 && 	//'R'
				buf[UDP_DATA_P+1]==0x4f && 	//'O'
				buf[UDP_DATA_P+2]==0x47 && 	//'G'
				buf[UDP_DATA_P+3]==0x45 && 	//'E'
				buf[UDP_DATA_P+4]==0x52)	//'R'
			GPIO_ToggleBits(GPIOD,GPIO_Pin_14); //Tooglea led rojo

		else if(buf[UDP_DATA_P]==0x46 && buf[UDP_DATA_P+1]==0x44)	//primer paquete para la imagen (FD)
		{
			UB_Led_Toggle(LED_BLUE);	//enciendo el led azul para avisar que voy a desbloquear la flash
			ancho = (int16_t)((buf[UDP_DATA_P+3]<<8) | buf[UDP_DATA_P+2]); //buffer[4] y buffer[5] son 0 xq en el server mando el ancho en formato 32 bytes
			alto = (int16_t)((buf[UDP_DATA_P+7]<<8)| buf[UDP_DATA_P+6]); //buffer[8] y buffer[9] son 0 xq en el server mando el alto en formato 32 bytes

			TFT_save(guardarPagina);
			for(i=10; i<1024; i++)
			{
				TFT_Write_Data(buf[UDP_DATA_P+i]);
			}
			/*
			strcpy(str,"imag_ancho: ");
			sprintf(msg," %d",imagen.imag_ancho);
			strcat(str,msg);
			make_udp_reply_from_request(buf,str,strlen(str),MYUDPPORT);
			strcpy(str,"imag_alto: ");
			sprintf(msg," %d",imagen.imag_alto);
			strcat(str,msg);
			make_udp_reply_from_request(buf,str,strlen(str),MYUDPPORT);
			 */
			UB_Led_On(LED_ORANGE); //Tooglea led Naranja
			return;
		}
		else if(buf[UDP_DATA_P]==0x4d && buf[UDP_DATA_P+1]==0x44)	//paquetes para rellenar la imagen (MD)
		{
			for(i=2; i<1022; i++)
			{
				TFT_Write_Data(buf[UDP_DATA_P+i]);
			}
			return;
		}
		else if(buf[UDP_DATA_P]==0x4c && buf[UDP_DATA_P+1]==0x44) //paquete que informa que se termino de recivir la imagen (LD)
		{
			UB_Led_Off(LED_ORANGE);
			TFT_show(mostrarPagina);
			guardarPagina = (guardarPagina+1)%3;
			mostrarPagina = (mostrarPagina+1)%3;
			return;
		}
	}
}


//---------------------------------------------
// Para mostrar una pantalla u otra
//---------------------------------------------
static void TFT_show(int op)
{
	switch(op)
	{
	case 0:
		// muestra la primer pantalla (0-271)
		TFT_WriteCmd(SET_SCROLL_START);
		TFT_Write_Data(0>>8);
		TFT_Write_Data(0&0xff);
		break;
	case 1:
		// muestra la segunda pantalla (272-543)
		TFT_WriteCmd(SET_SCROLL_START);
		TFT_Write_Data(272>>8);
		TFT_Write_Data(272&0xff);
		break;
	case 2:
		// muestra la segunda pantalla (544-815)
		TFT_WriteCmd(SET_SCROLL_START);
		TFT_Write_Data(544>>8);
		TFT_Write_Data(544&0xff);
		break;
	}
}

//---------------------------------------------
// Para guardar una pantalla u otra
//---------------------------------------------
static void TFT_save(int op)
{
	switch(op)
	{
	case 0:
		// muestra la primer pantalla (0-271)
		TFT_WindowSet(0,479,0,271);	//TFT_WriteCmd(SET_SCROLL_START);
		//TFT_Write_Data(0>>8);
		//TFT_Write_Data(0&0xff);
		break;
	case 1:
		// muestra la segunda pantalla (272-543)
		TFT_WindowSet(0,479,271,543);//TFT_WriteCmd(SET_SCROLL_START);
		//TFT_Write_Data(272>>8);
		//TFT_Write_Data(272&0xff);
		break;
	case 2:
		// muestra la segunda pantalla (544-815)
		TFT_WindowSet(0,479,544,815);//TFT_WriteCmd(SET_SCROLL_START);
		//TFT_Write_Data(544>>8);
		//TFT_Write_Data(544&0xff);
		break;
	}
}
