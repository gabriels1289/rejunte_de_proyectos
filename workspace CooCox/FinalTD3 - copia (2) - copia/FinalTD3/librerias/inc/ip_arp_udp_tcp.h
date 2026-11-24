/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 *
 * IP/ARP/UDP/TCP functions
 *
 *********************************************/
//@{
#ifndef IP_ARP_UDP_TCP_H
#define IP_ARP_UDP_TCP_H
#include "enc28j60.h"

// you must call this function once before you use any of the other functions:
void init_ip_arp_udp_tcp(t_enc28j60Ctrl * apCtrl, uint8_t *mymac,uint8_t *myip,uint8_t wwwp);
uint16_t recv_ip_arp_udp_tcp(uint16_t len, uint8_t *buf);

uint8_t eth_type_is_arp_and_my_ip(uint8_t *buf,uint16_t len);
uint8_t eth_type_is_ip_and_my_ip(uint8_t *buf,uint16_t len);
void make_arp_answer_from_request(uint8_t *buf);
void make_echo_reply_from_request(uint8_t *buf,uint16_t len);
void make_udp_reply_from_request(uint8_t *buf,char *data,uint8_t datalen,uint16_t port);

void make_tcp_synack_from_syn(uint8_t *buf);
void init_len_info(uint8_t *buf);
uint16_t get_tcp_data_pointer(void);
uint16_t fill_tcp_data_p(uint8_t *buf,uint16_t pos, const char *progmem_s);
uint16_t fill_tcp_data(uint8_t *buf,uint16_t pos, const char *s);
void make_tcp_ack_from_any(uint8_t *buf);
void make_tcp_ack_with_data(uint8_t *buf,uint16_t dlen);
void fill_buf_p(uint8_t *buf,uint16_t len, const char *progmem_s);
void send_udp_prepare(uint8_t *buf,uint16_t sport, uint8_t *dip, uint16_t dport);
void send_udp_transmit(uint8_t *buf,uint8_t datalen);
void send_udp(uint8_t *buf,char *data,uint8_t datalen,uint16_t sport, uint8_t *dip, uint16_t dport);
void client_arp_whohas(uint8_t *buf,uint8_t *ip_we_search);
uint8_t client_waiting_gw(void);
uint8_t client_store_gw_mac(uint8_t *buf);
void client_gw_arp_refresh(void);
void client_set_gwip(uint8_t *gwipaddr);

#endif /* IP_ARP_UDP_TCP_H */
//@}
