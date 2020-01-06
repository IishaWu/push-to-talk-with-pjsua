/* $Id: simple_pjsua.c 3553 2011-05-05 06:14:19Z nanang $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/**
 * simple_pjsua.c
 *
 * This is a very simple but fully featured SIP user agent, with the 
 * following capabilities:
 *  - SIP registration
 *  - Making and receiving call
 *  - Audio/media to sound device.
 *
 * Usage:
 *  - To make outgoing call, start simple_pjsua with the URL of remote
 *    destination to contact.
 *    E.g.:
 *	 simpleua sip:user@remote
 *
 *  - Incoming calls will automatically be answered with 200.
 *
 * This program will quit once it has completed a single call.
 */
//#include <stdio.h>
#include <pjsua-lib/pjsua.h>
//#include <pjsua-lib/pjsua_internal.h>
#define THIS_FILE	"APP"

#define SIP_DOMAIN	"[2001:e10:6840:21:20c:29ff:fe4b:62d9]"
#define SIP_USER	"tbcps"
#define SIP_PASSWD	"12345"
//int RCCS=1;
pjsua_call_id acc_id;
//int GetMessage;
/*floor*/
int f;
int g_local_port;
void set_local_rtpport(int port)
{
	g_local_port = port;
}
void on_call_sdp_created(pjsua_call_id call_id,
                                   pjmedia_sdp_session *sdp,
                                   pj_pool_t *pool,
                                   const pjmedia_sdp_session *rem_sdp)
{
	int nPort;
        pjmedia_sdp_attr *attr;
	if (sdp != NULL)
	{
		/*for (int i = 0; i < sdp->media_count; i++)
		{
			pjmedia_sdp_media *m = sdp->media[i];
			m->desc.port = 6000 + i;
			nPort = m->desc.port;
		}
		*/
                printf("___");
		pjmedia_sdp_media *m = sdp->media[sdp->media_count-1];
		m->desc.port = g_local_port;

		pjmedia_sdp_conn *c = sdp->conn;
		char* addr;
		if (c){
                        printf("______");
			addr= c->addr.ptr;}
		else
		{
			
			const pj_str_t *hostname;
			pj_sockaddr_in tmp_addr;
			char *addr;

			hostname = pj_gethostname();
			pj_sockaddr_in_init(&tmp_addr, hostname, 0);
			addr = pj_inet_ntoa(tmp_addr.sin_addr);
			//pj_ansi_strcpy(ip_addr, addr);
                        /*
                        sdp->origin.net_type = pj_str("IN");
                        sdp->origin.addr_type = pj_str("IP6");
                        sdp->origin.addr = pj_str("FF02::1");                        
                        m->conn->net_type = pj_str("IN");
                        m->conn->addr_type = pj_str("IP6");
                        m->conn->addr = pj_str("FF02::1");//"2001:e10:6840:21:f9f9:f3e1:9f0c:a3ff");
                        m->attr[0]->value = pj_str("4601 IN IP6 FF02::1");
                       */
                       //attr->value = pj_str("121 telephone-event/9000");
                       // m->attr[m->attr_count++] = attr;
                        
		}

		//addr = sdp->origin.addr.ptr;
		//printf(addr);

		//sdp->origin.addr = *pj_gethostname();
		//sdp->origin.user = pj_str("pjsip-siprtp");
		//sdp->origin.version = sdp->origin.id = tv.sec + 2208988800UL;
		//sdp->origin.net_type = pj_str("IN");
		//sdp->origin.addr_type = pj_str("IP4");
		// addr = sdp->origin.addr = *pj_gethostname();
		//sdp->name = pj_str("pjsip");
	}
        if (rem_sdp!= NULL)
        {
            pjmedia_sdp_media *rm = rem_sdp->media[rem_sdp->media_count-1];
        }
}

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			 (int)ci.remote_info.slen,
			 ci.remote_info.ptr));
    pjsua_call_hangup(call_id, 200, NULL,NULL);
    /* Automatically answer incoming calls with 200/OK */
    //pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
			 (int)ci.state_text.slen,
			 ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
	// When media is active, connect call to sound device.
	pjsua_conf_connect(ci.conf_slot, 0);
	pjsua_conf_connect(0, ci.conf_slot);
    }
}
/**
 * Incoming IM message (i.e. MESSAGE request)!
 */
void tbcp(char *addr,pj_str_t *uri){
     puts(addr);
     if (addr[0]=='G'){
        
        puts("**************************");
        if (f == 0){
           f = 1;
           pj_str_t text = pj_str("Granted");
           pjsua_im_send(acc_id, uri, NULL, &text, NULL, NULL);
         }
     
         else{
           pj_str_t text = pj_str("Taken");
           pjsua_im_send(acc_id, uri, NULL, &text, NULL, NULL);
         }
     }
     else
         f =0;
}
static void on_pager(pjsua_call_id call_id, const pj_str_t *from,
                     const pj_str_t *to, const pj_str_t *contact,
                     const pj_str_t *mime_type, const pj_str_t *text)
{
    /* Note: call index may be -1 */
    pj_str_t uri = *from;
    tbcp(text->ptr,&uri);
}
/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}

/*
 * main()
 *
 */
int main(int argc, char *argv[])
{
    //pjsua_acc_id acc_id;
    pj_status_t status;
    /* Create pjsua first! */
    status = pjsua_create();
    f = 0;
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* Init pjsua */
    {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_config_default(&cfg);
        cfg.cb.on_call_sdp_created = &on_call_sdp_created;
	cfg.cb.on_incoming_call = &on_incoming_call;
	cfg.cb.on_call_media_state = &on_call_media_state;
	cfg.cb.on_call_state = &on_call_state;
        cfg.cb.on_pager = &on_pager;
	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 4;

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }
    /* set null device*/
    //pjsua_set_null_snd_dev();
    /* Add UDP transport. */
    {
        set_local_rtpport(4700);
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = 5090;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP6, &cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

    /* Register to SIP server by creating SIP account. */
    {
	pjsua_acc_config cfg;

	pjsua_acc_config_default(&cfg);
	cfg.id = pj_str("sip:" SIP_USER "@" SIP_DOMAIN);
	cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str(SIP_DOMAIN);
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str(SIP_USER);
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str(SIP_PASSWD);
        cfg.ipv6_media_use = PJSUA_IPV6_ENABLED;
	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }


    /* Wait until user press "q" to quit. */
    for (;;) {
	char option[10];
	if (fgets(option, sizeof(option), stdin) == NULL) {
	    puts("EOF while reading stdin, will quit now..");
	    break;
	}
	if (option[0] == 'q')
	    break;
	if (option[0] == 'h')
	    pjsua_call_hangup_all();
        puts("Press 'h' to hangup all calls, 'q' to quit");
    }

    /* Destroy pjsua */
    pjsua_destroy();

    return 0;
}

