RAST implementation - ReadMe
============================

<h1> Overview </h1>

RAST is a practical and effective Radio duty-cycled, Active-Scan based network formation process for TSCH networks. It leans on active-scan procedures combined with radio duty cycling mechanisms to shorten joining delays and reduce energy consumption.

This implementation is presented in depth and evaluated in our paper: (upcoming).


<h1> Features </h1>

This implementation includes:
<dl>
 <dt>-Standard IEEE 802.15.4-2015 frame version 2</dt>
<dt>-Standard TSCH joining procedure with Enhanced Beacons Request</dt>
<dt>-Standard TSCH link selection and slot operation </dt>
<dt>-Standard TSCH synchronization, including with ACK/NACK time correction Information Element</dt>
<dt>-Standard TSCH queues and CSMA-CA mechanism</dt>
<dt>-A scheduling API to add/remove slotframes and links </dt>
</dl>
  
<h1> Code structure </h1>

RAST with all its mechanisms is implemented in:

The IEEE 802.15.4e-2012 frame format is implemented in:

<ul>
 <li>os/net/mac/framer/frame802154.[ch]: handling of frame version 2 (including EBR)</li>
  
<li>os/net/mac/framer/frame802154-ie.[ch]: handling of Information Elements (To add the time and channel information)</li>

<li>os/net/mac/tsch/tsch.[ch]: TSCH management (active scan-based association (EBRs), keep-alive (KA), periodical advertisement (EBs), EBR input process, sending EB upon receiving EBRs, and dynamic removal of extra links...), processes handling pending outgoing and incoming packets, and interface with upper layers as a MAC driver. To correctly enable RAST, Contikimac RDC is required.</li>
  
<li>tsch-slot-operation.[ch]: TSCH low-level slot operation, fully interrupt-driven. Node wake up at every active slot (according to the slotframes and links installed), transmit or receive frames and ACKs.</li>

  
<li>tsch-packet.[ch]: TSCH Enhanced ACK (EACK), enhanced Beacon Request (EBR), and enhanced Beacon (EB) creation and parsing.</li>
  

<li>tsch-schedule.[ch]: TSCH slotframe and link handling, and API for slotframe and link installation/removal (including Dynamic Schedule).</li>
  
<li>tsch-const.h: the constants required by TSCH.</li>
  
<li>tsch-types.h: the data types defined by TSCH.</li>
  
<li>tsch-conf.h: general configuration file for TSCH.</li>
  
<li>contikimac.[ch]: EBR packet managment (handling strobes ).</li> 
</ul>


<h1> Using RAST (Run an example)</h1>

A simple Active Network Formation example is included under RAST/examples/ipv6/NETWORK-FORMATION/Z1_49_node. To use TSCH, first make sure your platform supports it. Currently, Z1 and cooja are the supported platforms.

To configure further, see the macros in .h files under os/net/mac/tsch/ and redefine your own in your project-conf.h.

<h1> Configuring the association process </h1>
<ul>
When attempting to associate to a network, nodes scan actively channels at random until they receive an enhanced beacon (EB). The beaconing interval and the channel hopping sequence play a key role in how fast and efficient the association process is. To fine-tune, experiment with the following defines:
  
<li>NETSTACK_CONF_RDC: configure the contikimac_driver to allow the sending several stobes of EBRs </li>

 <li>STROBE_TIME: Define the lenghth of storbe time</li>
   
 
  <li>EBR_INTERVAL: Define the interval between EBR frames</li> 
 
  <li>EB_INTERVAL: Define the interval between EB packets</li>   
  
  
<li>TSCH_CONF_DEFAULT_HOPPING_SEQUENCE: The default hopping sequence (optionally, a coordinator could choose advertise a different sequence). Use a sequence with fewer different channels for faster association (but less frequency diversity).</li>
  
<li>TSCH_CONF_JOIN_HOPPING_SEQUENCE: Optionally, set a different hopping sequence for scanning. Only use this if you also implement your own mechanism to restrict EBs to a subset of frequencies (e.g. using a slotframe of length 4 with one slot for EBs would result in hopping over only 4 channels).</li>
  
  <li>TSCH_SCHEDULE_CONF_DEFAULT_LENGTH : defines the 6TiSCH minimal schedule length. Larger values result in less frequent active slots: reduces capacity and saves energy</li>   
  
</ul>
