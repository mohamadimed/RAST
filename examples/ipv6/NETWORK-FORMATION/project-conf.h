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
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__

/* Set to run orchestra */
#ifndef WITH_ORCHESTRA				// orcestra et sans (minimal) //  
#define WITH_ORCHESTRA 0
#endif /* WITH_ORCHESTRA */

/* Set to enable TSCH security */
#ifndef WITH_SECURITY				// orcestra securité, non  et sans (sec, non) (minimal)
#define WITH_SECURITY 0				//TSCH-min-3 d=0.36, E= 10.01, Prr=1   EB size : 39, DATA  : 23
#endif /* WITH_SECURITY */

/*******************************************************/
/********* Enable RPL non-storing mode *****************/
/*******************************************************/

/*******************************************************/


/*******************************************************/


#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 0 /* No need for routes */
/*******************************************************/

#undef RPL_CONF_MOP

//If i want to use RPL WITH NON STORING WITHOUT UPWARD ROUTES ONLY

/*******************************************************/
//#define RPL_CONF_MOP RPL_MOP_NO_DOWNWARD_ROUTES
#define  RPL_CONF_WITH_STORING 0

/*******************************************************/

//If i want to use RPL WITH NON STORING WITH DOWNWARD & UPWARD ROUTES, to use DownWard Routes i have to place back the 3 NOT MODIFIED FILES (rpl-icmp6.c ... ) In \RPL to activate DAO

/*******************************************************/
#define RPL_CONF_MOP RPL_MOP_NON_STORING /* Mode of operation*/
/*******************************************************/

//#undef ORCHESTRA_CONF_RULES
//#define ORCHESTRA_CONF_RULES {&eb_per_time_source, &unicast_per_neighbor_rpl_ns, &default_common} /* Orchestra in non-storing */

/*******************************************************/
/********************* Enable TSCH *********************/
/*******************************************************/

/* Netstack layers */
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC   tschmac_driver //tschmac_driver //nullmac_driver,csma_driver   

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC  contikimac_driver//nordc_driver//, contikimac_driver, nullrdc_driver

/***************************************************************************************************************************************************************/
/*whene we use contikimac_driver we should put it to 1 to change code for tsch-slot-operation*/
#undef TSCH_WITH_RDC
#define TSCH_WITH_RDC  1

/*******************RV and RH schedule must be used with WITH_RDC=0 and conf RDC nord_driver*************************/
#undef TSCH_WITH_RV
#define TSCH_WITH_RV  0

#undef TSCH_WITH_RH
#define TSCH_WITH_RH  1


/*Define the lenghth of storbe time -- 492 = 15 ms so 2500 => 75ms ( 75 = 15 * 5 slots <==> SF size)*/
#define STROBE_TIME            2500 //2500 (SF05) --- 15500 (Orchetra SF31)

/*Define the interval between EBR frames -- if it equals to 2 , so we will the interval is 2 times CCR (2 times SF size) because we admet that CCR = SF size*/
#define EBR_INTERVAL 2 //2 //4 // 5

/*Define the interval between EB frames -- the interval is define in tick that will be converted into seconds when called by the function.  125 tiks = 1sec | 500 tiks x 8 = 4000 ms = 4 seconds*/
#define EB_INTERVAL 2000//250//625  //625 = 5 s // 250 = 2 s //2000 = 16 s (if no rdc)
				
/*Define the number of chennels used for communication : 2 // 4 // 8 // 12 //16 */

#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_8

#undef CONTIKIMAC_CONF_COMPOWER
#define CONTIKIMAC_CONF_COMPOWER 0

/***************************************************************************************************************************************************************/

#undef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8

/*This parameter that defines the channel check rate is NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE, and it should be set to a power of 2 (2, 4, 8, 16, 32, etc.); its unit is Hz (how many times the nodes will check medium in second).*/


#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER   framer_802154  //contikimac_framer 

/* IEEE802.15.4 frame version */
#undef FRAME802154_CONF_VERSION
#define FRAME802154_CONF_VERSION FRAME802154_IEEE802154E_2012

/* TSCH and RPL callbacks */
#define RPL_CALLBACK_PARENT_SWITCH tsch_rpl_callback_parent_switch
#define RPL_CALLBACK_NEW_DIO_INTERVAL tsch_rpl_callback_new_dio_interval
#define TSCH_CALLBACK_JOINING_NETWORK tsch_rpl_callback_joining_network
#define TSCH_CALLBACK_LEAVING_NETWORK tsch_rpl_callback_leaving_network

/* Needed for CC2538 platforms only */
/* For TSCH we have to use the more accurate crystal oscillator
 * by default the RC oscillator is activated */
#undef SYS_CTRL_CONF_OSC32K_USE_XTAL
#define SYS_CTRL_CONF_OSC32K_USE_XTAL 1

/* Needed for cc2420 platforms only */
/* Disable DCO calibration (uses timerB) */
#undef DCOSYNCH_CONF_ENABLED
#define DCOSYNCH_CONF_ENABLED 0
/* Enable SFD timestamps (uses timerB) */
#undef CC2420_CONF_SFD_TIMESTAMPS
#define CC2420_CONF_SFD_TIMESTAMPS 1




/*******************************************************/
/******************* Configure TSCH ********************/
/*******************************************************/

/* TSCH logging. 0: disabled. 1: basic log. 2: with delayed
 * log messages from interrupt */
#undef TSCH_LOG_CONF_LEVEL
#define TSCH_LOG_CONF_LEVEL 1 //2

/* IEEE802.15.4 PANID */
#undef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID 0xabcd

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#undef TSCH_CONF_AUTOSTART
#define TSCH_CONF_AUTOSTART 0

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#undef TSCH_SCHEDULE_CONF_DEFAULT_LENGTH
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 5//101//5//100//5//05//101//5//8//7//10//17//10//17//3    //5 for 10 nodes  //10 slots gives good results when compring rdc with no rdc (6 nodes tested )

//TSCH_CONF_DEFAULT_TIMESLOT_LENGTH 1500 defined to Z1 because 10ms is too bit short-- see project-conf of z1 platforme
#

#if WITH_SECURITY
/* Enable security */

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED 1
/* TSCH uses explicit keys to identify k1 and k2 */
#undef LLSEC802154_CONF_USES_EXPLICIT_KEYS
#define LLSEC802154_CONF_USES_EXPLICIT_KEYS 1
/* TSCH uses the ASN rather than frame counter to construct the Nonce */
#undef LLSEC802154_CONF_USES_FRAME_COUNTER
#define LLSEC802154_CONF_USES_FRAME_COUNTER 0

#endif /* WITH_SECURITY */



#if WITH_ORCHESTRA

/* See apps/orchestra/README.md for more Orchestra configuration options */
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL 0 /* No 6TiSCH minimal schedule */
#define TSCH_CONF_WITH_LINK_SELECTOR 1 /* Orchestra requires per-packet link selection */
/* Orchestra callbacks */
#define TSCH_CALLBACK_NEW_TIME_SOURCE orchestra_callback_new_time_source
#define TSCH_CALLBACK_PACKET_READY orchestra_callback_packet_ready
#define NETSTACK_CONF_ROUTING_NEIGHBOR_ADDED_CALLBACK orchestra_callback_child_added
#define NETSTACK_CONF_ROUTING_NEIGHBOR_REMOVED_CALLBACK orchestra_callback_child_removed


#endif /* WITH_ORCHESTRA */

/*******************************************************/
/************* Other system configuration **************/
/*******************************************************/

#if CONTIKI_TARGET_Z1
/* Save some space to fit the limited RAM of the z1 */

#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1

#undef UIP_CONF_TCP
#define UIP_CONF_TCP 0

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM  4//5 // (3 FOR ORCHESTRA) (5 with NBR_TABLE_CONF_MAX_NEIGHBORS 6) is the number of queuebufs stored in RAM.
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM  30 //51//60//51//60//51//60//80 // 60 | 51   [80: All TSCH networks] [60: Orchestra 95 Network] [51: Orchestra and TSCh  50 and 10 Network]
		
				// For 95 Node with Orchestra: 60 Link is suffisant, 80 Link For TSCH-Min / For 50 Node for [TSCH min et Orch]: 51 Link is suffisant Also FOR 10 nodes [TSCH Min et Orch]

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 8//1
#undef UIP_CONF_ND6_SEND_NS
#define UIP_CONF_ND6_SEND_NS 0
#undef SICSLOWPAN_CONF_FRAG
#define SICSLOWPAN_CONF_FRAG 0



#undef TSCH_LOG_CONF_LEVEL
#define TSCH_LOG_CONF_LEVEL 1


#if WITH_SECURITY
/* Note: on sky or z1 in cooja, crypto operations are done in S/W and
 * cannot be accommodated in normal slots. Use 65ms slots instead, and
 * a very short 6TiSCH minimal schedule length */
#undef TSCH_CONF_DEFAULT_TIMESLOT_LENGTH
#define TSCH_CONF_DEFAULT_TIMESLOT_LENGTH 65000
#undef TSCH_SCHEDULE_CONF_DEFAULT_LENGTH
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 2
/* Reduce log level to make space for security on z1 */
#undef TSCH_LOG_CONF_LEVEL
#define TSCH_LOG_CONF_LEVEL 0
#endif /* WITH_SECURITY */

#endif /* CONTIKI_TARGET_Z1 */

#if CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_ZOUL || \
  CONTIKI_TARGET_OPENMOTE_CC2538
#define TSCH_CONF_HW_FRAME_FILTERING    0
#endif /* CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_ZOUL \
       || CONTIKI_TARGET_OPENMOTE_CC2538 */

#if CONTIKI_TARGET_COOJA
#define COOJA_CONF_SIMULATE_TURNAROUND 0
#endif /* CONTIKI_TARGET_COOJA */

#endif /* __PROJECT_CONF_H__ */

/********CONFIGURE IEs********/
/*
#undef TSCH_PACKET_CONF_EB_WITH_TIMESLOT_TIMING
#define TSCH_PACKET_CONF_EB_WITH_TIMESLOT_TIMING 1
#undef TSCH_PACKET_CONF_EB_WITH_HOPPING_SEQUENCE						//when added eb size is greater by 48 byte and becomes 80 instead of 37
#define TSCH_PACKET_CONF_EB_WITH_HOPPING_SEQUENCE 1
#undef TSCH_PACKET_EB_WITH_SLOTFRAME_AND_LINK
#define TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK 1

*/ 



#undef CCA_ENABLED 
#define CCA_ENABLED 0
