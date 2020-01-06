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

#include <pjsua-lib/pjsua.h>
//#include <pjsua-lib/pjsua_internal.h>
#define THIS_FILE	"APP"

#define SIP_DOMAIN	"[2001:e10:6840:21:20c:29ff:fe4b:62d9]"
#define SIP_USER	"nicole"
#define SIP_PASSWD	"12345"
//#define RTP_ADDR        "FF02::1"
pjsua_call_id acc_id;
int Granted;
int g_local_port;

void set_local_rtpport(int port)
{
	g_local_port = port;
}
/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
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
		pjmedia_sdp_media *m = sdp->media[sdp->media_count-1];
		m->desc.port = g_local_port;

		pjmedia_sdp_conn *c = sdp->conn;
		char* addr;
		if (c){
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
                        m->attr[0]->value = pj_str("4001 IN IP6 FF02::1");
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
            //printf("%s%i","______________________________",rm->desc.port);
        }
	/*
	if (rem_sdp!= NULL)
	{
		for (int i = 0; i < rem_sdp->media_count; i++)
		{
			pjmedia_sdp_media *m = rem_sdp->media[i];
			m->desc.port = 6000 + i;
			nPort = m->desc.port;
		}
	}
	*/
}
   /**
     * This callback is called when media transport state is changed. See
     * also #pjsua_med_tp_state_cb.
     */
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
    //pjsua_media_channel_deinit(call_id);
    /*
    pjsua_acc_config cfg;
    pj_bool_t changed = PJ_FALSE;
    pj_pool_t *tmp_pool = pjsua_pool_create("tmp-pjsua", 1000, 1000);
    pjsua_acc_get_config(acc_id, tmp_pool, &cfg);
    cfg.rtp_cfg.public_addr =pj_str("ff02::1:ffd3:158");
    cfg.ipv6_media_mcast_use = PJSUA_IPV6_MCAST_ENABLED;
    pjsua_transport_create(cfg);*/
    puts("Press 'a' to answer the calls,'h' to hangup all calls");
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
    pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
    status = pjsua_conf_connect(ci.conf_slot, 0);
    printf("%s","********************Connect***************\n");
    if (Granted==0){
       status = pjsua_conf_disconnect(0,ci.conf_slot);
       status = pjsua_conf_connect(ci.conf_slot, 0);
       if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);
       printf("%s","************Floor Release***********\n");
    }
    if (Granted==1){
    // if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
       //status = pjsua_conf_disconnect(ci.conf_slot, 0);
       status = pjsua_conf_disconnect(ci.conf_slot, 0);
       status = pjsua_conf_connect(0,ci.conf_slot);
       printf("%s","*************Floor Granted**************\n");}
}
void on_pager_status2(pjsua_call_id call_id,
			     const pj_str_t *to,
			     const pj_str_t *body,
			     void *user_data,
			     pjsip_status_code status,
			     const pj_str_t *reason,
			     pjsip_tx_data *tdata,
			     pjsip_rx_data *rdata,
			     pjsua_acc_id acc_id){
    // PJ_LOG(4,(THIS_FILE,"###################### \'%s\'",rdata->msg_info.msg));
}
static void on_pager(pjsua_call_id call_id, const pj_str_t *from,
                     const pj_str_t *to, const pj_str_t *contact,
                     const pj_str_t *mime_type, const pj_str_t *text)
{
    /* Note: call index may be -1 */
     /*
     char sub[strlen(text->ptr)];
     strncpy(sub, addr, strlen(text->ptr)-1);    
     printf("%*s",sub);
     puts(sub);
     */
     puts(text->ptr);
     if (text->ptr[0]=='G'){
        Granted=1;
        on_call_media_state(acc_id);
    }
    else{
        Granted=0;
        //on_call_media_state(acc_id);
    }
}
void make_call(){
     char input[50],pkg[200]="";
     char *split;
     char sub[strlen(input)];
     puts("Enter uri you want to invite, e.g. nicole alice");
     fgets(input, sizeof(input), stdin);
     strncpy(sub, input, strlen(input)-1);
     sub[strlen(input)-1] = 0;
     split=strtok(sub," ");
     do{
         char buf[50]="sip:";
         sprintf(buf+strlen(buf),split);
         sprintf(buf+strlen(buf),"@[2001:e10:6840:21:20c:29ff:fe4b:62d9]:5060 ");
         sprintf(pkg+strlen(pkg),buf);
         split=strtok(NULL," ");    
     }while (split);
     
     pj_str_t text = pj_str(pkg);
     pj_str_t uri = pj_str("sip:glms@[2001:e10:6840:21:20c:29ff:fe4b:62d9]:5060");
     pjsua_im_send(acc_id, &uri, NULL, &text, NULL, NULL);

}

void sendTBCP(char *tbcp){
     
     pj_str_t text = pj_str(tbcp);
     pj_str_t uri = pj_str("sip:tbcps@[2001:e10:6840:21:20c:29ff:fe4b:62d9]:5060");
     pjsua_im_send(acc_id, &uri, NULL, &text, NULL, NULL);
     if (tbcp[0]=='R'){
        Granted=0;
        on_call_media_state(acc_id);              
     }
}
/*
 * main()
 *
 * argv[1] may contain URL to call.
 */
int main(int argc, char *argv[])
{
    pjsua_acc_id acc_id;
    pj_status_t status;
    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* If argument is specified, it's got to be a valid SIP URL */
    if (argc > 1) {
	status = pjsua_verify_url(argv[1]);
	if (status != PJ_SUCCESS) error_exit("Invalid URL in argv", status);
    }
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
        //cfg.cb.on_pager_status2 = on_pager_status2;
	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 4;

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add UDP transport. */
    {
        set_local_rtpport(4000);
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = 5080;
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
        cfg.ipv6_media_mcast_use = PJSUA_IPV6_MCAST_ENABLED;
        //cfg.rtp_cfg.public_addr =pj_str("ff02::1:ffd3:158");
        //cfg.rtp_cfg.bound_addr =pj_str(RTP_ADDR);
	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }

    /* Wait until user press "q" to quit. */
    for (;;) {
	char option[10];

	puts("Press 'h' to hangup all calls, 'q' to quit");
	if (fgets(option, sizeof(option), stdin) == NULL) {
	    puts("EOF while reading stdin, will quit now..");
	    break;
	}
        if (option[0] == 's')
           sendTBCP("G");
        if (option[0] == 'e')
           sendTBCP("R");
        if (option[0] == 'm')
            make_call();
              
	if (option[0] == 'q')
	    break;
        if (option[0] == 'a'){
           pjsua_call_answer(acc_id, 200, NULL, NULL);
           puts("Press 's' to get the floor,'e' to release the floor");
        }
	if (option[0] == 'h')
	    pjsua_call_hangup_all();
    }

    /* Destroy pjsua */
    pjsua_destroy();

    return 0;
}
