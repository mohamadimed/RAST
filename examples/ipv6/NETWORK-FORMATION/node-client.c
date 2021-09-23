/*
 * Copyright (c) 2015, SICS Swedish ICT.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \file
 *         A RPL+TSCH node able to act as either a simple node (6ln),
 *         DAG Root (6dr) or DAG Root with security (6dr-sec)
 *         Press use button at startup to configure.
 *
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"
#include "node-id.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ip/udp-socket.h"
#include "net/mac/tsch/tsch.h"
#include "net/rpl/rpl-private.h"
#if WITH_ORCHESTRA
#include "orchestra.h"
#endif /* WITH_ORCHESTRA */

#define DEBUG  DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define CONFIG_VIA_BUTTON PLATFORM_HAS_BUTTON
#if CONFIG_VIA_BUTTON
#include "button-sensor.h"
#endif /* CONFIG_VIA_BUTTON */


#include "dev/serial-line.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ip/uip-udp-packet.h"
#include "apps/powertrace/powertrace.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID  190


#ifndef PERIOD
#define PERIOD 	1 //60	/* 17 (both) slots for 10 nodes, trying 10 for 50 nodes*/1
#endif

#define START_INTERVAL		( 25 * CLOCK_SECOND) //180 //300s for 10nodes // 360 for 50Nodes  5.6171
#define SEND_INTERVAL		(PERIOD * CLOCK_SECOND)
//#define SEND_TIME		(random_rand() % (SEND_INTERVAL))  //(random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN		30





static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
#if CONFIG_VIA_BUTTON
AUTOSTART_PROCESSES(&node_process, &sensors_process);
#else /* CONFIG_VIA_BUTTON */
AUTOSTART_PROCESSES(&node_process);
#endif /* CONFIG_VIA_BUTTON */

/*---------------------------------------------------------------------------*/

static int seq_id;
static int reply;
static unsigned short int startIntervalReached= 0;

static void
tcpip_handler(void)
{
  char *str;

  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';
    reply++;
    printf("DATA-recv-rep '%s' (s:%d, r:%d)\n", str, seq_id, reply);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet(void *ptr)
{
  char buf[MAX_PAYLOAD_LEN];

#ifdef SERVER_REPLY
  uint8_t num_used = 0;
  uip_ds6_nbr_t *nbr;

  nbr = nbr_table_head(ds6_neighbors);
  while(nbr != NULL) {
    nbr = nbr_table_next(ds6_neighbors, nbr);
    num_used++;
  }

  if(seq_id > 0) {
    ANNOTATE("#A r=%d/%d,color=%s,n=%d %d\n", reply, seq_id,
             reply == seq_id ? "GREEN" : "RED", uip_ds6_route_num_routes(), num_used);
  }
#endif /* SERVER_REPLY */

  seq_id++;
  PRINTF("DATA-send to %d 'Hello %d'\n",
         server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], seq_id);
  sprintf(buf, "Hello %d from the client", seq_id); //payload
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT)); //printf("DATA SENT\n");

}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static void
net_init(uip_ipaddr_t *br_prefix)
{



  uip_ipaddr_t global_ipaddr;

#ifdef CONTIKI_TARGET_Z1


  uip_ip6addr(&server_ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0xc30c, 0x0000, 0x0000, 0x0001);

#elif CONTIKI_TARGET_COOJA

  uip_ip6addr(&server_ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0x0207, 0x0007, 0x0007, 0x0004);

#endif
											

  if(br_prefix) { /* We are RPL root. Will be set automatically
                     as TSCH pan coordinator via the tsch-rpl module */
    memcpy(&global_ipaddr, br_prefix, 16);
    uip_ds6_set_addr_iid(&global_ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&global_ipaddr, 0, ADDR_AUTOCONF);
    rpl_set_root(RPL_DEFAULT_INSTANCE, &global_ipaddr);
    rpl_set_prefix(rpl_get_any_dag(), br_prefix, 64);
    rpl_repair_root(RPL_DEFAULT_INSTANCE);

  }
//If we want to disable it we must put NESTSTACK AUTOSTART 1 in project-conf.h
  NETSTACK_MAC.on();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
	
  static struct etimer periodic;
  static struct ctimer backoff_timer;
  //static struct etimer et;
  static struct etimer estabilish;
  PROCESS_BEGIN();

  /* 3 possible roles:
   * - role_6ln: simple node, will join any network, secured or not
   * - role_6dr: DAG root, will advertise (unsecured) beacons
   * - role_6dr_sec: DAG root, will advertise secured beacons
   * */
  static int is_coordinator = 0;
  static enum { role_6ln, role_6dr, role_6dr_sec } node_role;
  node_role = role_6ln;
  

  int coordinator_candidate = 0;

#ifdef CONTIKI_TARGET_Z1
  /* Set node with MAC address c1:0c:00:00:00:00:01 as coordinator,
   * convenient in cooja for regression tests using z1 nodes
   * */
  extern unsigned char node_mac[8];
  unsigned char coordinator_mac[8] = { 0xc1, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

  coordinator_candidate = (memcmp(node_mac, coordinator_mac, 8) == 0);

#elif CONTIKI_TARGET_COOJA
  coordinator_candidate = (node_id == 1);
#endif

  if(coordinator_candidate) {
    if(LLSEC802154_ENABLED) {
      node_role = role_6dr_sec;
    } else {
      node_role = role_6dr;
    }
  } else {
    node_role = role_6ln;
  }

/* new connection with remote host */

	//if(node_id == 7)
{

  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL); 
  if(client_conn == NULL) {
    printf("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT)); 

  printf("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  printf(" local/remote port %u/%u\n",
	UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));}

#if WITH_COMPOWER
  powertrace_sniff(POWERTRACE_ON);
#endif
/*----------------------------------*/

  printf("Init: node starting with role %s\n",
         node_role == role_6ln ? "6ln" : (node_role == role_6dr) ? "6dr" : "6dr-sec");

  tsch_set_pan_secured(LLSEC802154_ENABLED && (node_role == role_6dr_sec));
  is_coordinator = node_role > role_6ln;

  if(is_coordinator) {
    uip_ipaddr_t prefix;
    uip_ip6addr(&prefix, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
    net_init(&prefix);
  } else {
    net_init(NULL);
  }

#if WITH_ORCHESTRA
  orchestra_init();
#endif /* WITH_ORCHESTRA */


  //etimer_set(&et, CLOCK_SECOND * 60);
  etimer_set(&estabilish, START_INTERVAL);

  while(1) {								/*****RPL-ICMP6.c to see all rpl messages*******/
	
	powertrace_start(CLOCK_SECOND * 1);//2148
	



    
    PROCESS_WAIT_EVENT();
	
    if(ev == PROCESS_EVENT_TIMER)  {
							  //SEND_TIME instead of 1
	if (data == &estabilish) {ctimer_set(&backoff_timer, 1, send_packet, NULL); // why add a send time and not send immediatly ? put it to 0 or 1 ti send directaly
				 // printf("START TIMER\n"); 
				  startIntervalReached=1;
				  etimer_set(&periodic, SEND_INTERVAL);}
							

	else if(data == &periodic && startIntervalReached==1 ){
      
     					 etimer_reset(&periodic);
								 //SEND_TIME instead of 1
      					 ctimer_set(&backoff_timer, 1, send_packet, NULL);
      					// printf("PERIODIC TIMER\n");
			 	  } 
    				    }     //End Event

    else if(ev == tcpip_event){
	tcpip_handler();
    } 
 	 /* Print out routing tables every minute */

   //  if (data == &et && ev == PROCESS_EVENT_TIMER ) {
     //  		etimer_reset(&et);
		//print_network_status();
	

    //}
   
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
