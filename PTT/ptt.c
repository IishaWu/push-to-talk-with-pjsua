


#include <pjsua-lib/pjsua.h>
#include <mxml.h>
#define THIS_FILE	"APP"

#define SIP_DOMAIN	"[2001:db8:409:1::8]"
#define SIP_USER	"alice"
#define SIP_PASSWD	"12345"

pjsua_call_id acc_id;

// floor and group id
struct floor_id fi;
int g_local_port;
char *mcast_addr;

// floor and group id
struct floor_id
{
      int floor;
      char* id;
};

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
                        
		}

	}
        if (rem_sdp!= NULL)
        {
            pjmedia_sdp_media *rm = rem_sdp->media[rem_sdp->media_count-1];
        }
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
    printf("Incoming call from %s!!\n",ci.remote_info.ptr);
    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			 (int)ci.remote_info.slen,
			 ci.remote_info.ptr));
    //pjsua_media_channel_deinit(call_id);
    
    pjsua_acc_config cfg;
    pj_bool_t changed = PJ_FALSE;
    pj_pool_t *tmp_pool = pjsua_pool_create("tmp-pjsua", 1000, 1000);
    pjsua_acc_get_config(acc_id, tmp_pool, &cfg);
    //puts(cfg.rtp_cfg.public_addr.ptr);
    mcast_addr = cfg.rtp_cfg.public_addr.ptr;
    
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
    if (fi.floor==1){
       // can talk 
      puts("You can talk Now!");
       status = pjsua_conf_disconnect(ci.conf_slot, 0);
       status = pjsua_conf_connect(0,ci.conf_slot);
       if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);
    }
    else{
       // can't talk
       puts("You don't have floor!");
       status = pjsua_conf_disconnect(0,ci.conf_slot);
       status = pjsua_conf_connect(ci.conf_slot, 0);
       if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);
    }
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
     mxml_node_t *g,*data,*fn,*msg;
     char *floor,*id,*work;
     //puts(text->ptr);
     msg = mxmlLoadString(NULL, text->ptr, MXML_OPAQUE_CALLBACK);
     data = mxmlFindElement(msg,msg, "data",NULL,NULL,MXML_DESCEND);
     work = (char *)mxmlElementGetAttr(data, "work");
     if (strcmp("TBCP",work)==0){
        // get group
        g = mxmlFindElement(data,data, "group",NULL,NULL,MXML_DESCEND);
        id = (char *)mxmlElementGetAttr(g, "id");
        fn = mxmlFindElement(data,data, "floor",NULL,NULL,MXML_DESCEND);
        floor = (char *)mxmlElementGetAttr(fn, "value");
        if (strcmp("Granted",floor)==0){
           fi.floor=1;
           on_call_media_state(acc_id);
        }
        else{
           fi.floor=0;
           on_call_media_state(acc_id);
        }
        //on_call_media_state(acc_id);
      }
}
void make_call(){
     char input[50],buffer[500]="";
     char *split;
     char sub[strlen(input)];
     // create xml

     mxml_node_t *xml;    // <?xml ... ?> 
     mxml_node_t *data;   // <data>/
     mxml_node_t *group;  // <group>
     mxml_node_t *member; //<member>

     xml = mxmlNewXML("1.0");
     // default group name
     data = mxmlNewElement(xml, "data");
     mxmlElementSetAttr(data,"work","makeCall");     
     group = mxmlNewElement(data, "group");
     // add myself
     char buf[50]="sip:";
     sprintf(buf+strlen(buf),SIP_USER);
     sprintf(buf+strlen(buf),"@[2001:db8:409:1::8]:5060");
     member = mxmlNewElement(group, "member");
     mxmlElementSetAttr(member,"name",buf);
     // get member
     fgets(input, sizeof(input), stdin);
     if (strlen(input)>1){
       strncpy(sub, input, strlen(input)-1);
       sub[strlen(input)-1] = 0;
       split=strtok(sub," ");
       do{
         char buf[50]="sip:";
         sprintf(buf+strlen(buf),split);
         sprintf(buf+strlen(buf),"@[2001:db8:409:1::8]:5060");
         member = mxmlNewElement(group, "member");
         mxmlElementSetAttr(member,"name",buf);
         split=strtok(NULL," ");    
       }while (split);
     }
     // xml to string
     mxmlSaveString(xml,buffer, sizeof(buffer),MXML_NO_CALLBACK);
     pj_str_t text = pj_str(buffer);
     //pj_str_t uri = pj_str("sip:glms@[2001:db8:409:1::8]:5080");
     pj_str_t uri = pj_str("sip:glms@[2001:db8:409:1::8]:5060");
     pjsua_im_send(acc_id, &uri, NULL, &text, NULL, NULL);

}

void sendTBCP(char *tbcp){
     
     // create xml

     mxml_node_t *xml;    // <?xml ... ?> 
     mxml_node_t *data;   // <data>/
     mxml_node_t *group;  // <group>
     mxml_node_t *floor; //<member>
     char buffer[500];

     xml = mxmlNewXML("1.0");
     // default group name
     data = mxmlNewElement(xml, "data");
     mxmlElementSetAttr(data,"work","TBCP");
     group = mxmlNewElement(data, "group");
     mxmlElementSetAttr(group,"id",mcast_addr);
     floor = mxmlNewElement(data, "floor");
     mxmlElementSetAttr(floor,"value",tbcp);     
     mxmlSaveString(xml,buffer, sizeof(buffer),MXML_NO_CALLBACK);
     pj_str_t uri = pj_str("sip:tbcps@[2001:db8:409:1::8]:5060");
     pj_str_t text = pj_str(buffer);
     pjsua_im_send(acc_id, &uri, NULL, &text, NULL, NULL);
     //puts(tbcp[0]);
     //puts("++++++++++++++++++++++++++++");
     if (strcmp("Release",tbcp)==0){
        fi.floor=0;
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
	log_cfg.console_level = 0;
	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add SIP UDP transport. */
    {
        set_local_rtpport(4000);
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = 5080;
        cfg.bound_addr =pj_str("::");
        //cfg.public_addr = pj_str("ff02::1:105");
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
        cfg.ptt_mcast_use = PJSUA_PTT_MCAST_ENABLED;
	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        puts("Registration Finished!");
	if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }
    // floor and group id and floor =  Taken
    fi.floor = 0;

    /* Wait until user press "q" to quit. */
    puts("Press 'h' to hangup all calls, 'm' to make call, 'q' to quit");
    for (;;) {
	char option[10];

	if (fgets(option, sizeof(option), stdin) == NULL) {
	    puts("EOF while reading stdin, will quit now..");
	    break;
	}
        if (option[0] == 's'){
           puts("Request the floor");
           sendTBCP("Request");}
        if (option[0] == 'e'){
           puts("Release the floor");
           sendTBCP("Release");}
        if (option[0] == 'm'){
            puts("Enter name you want to invite, e.g. nicole alice denny");
            make_call();}
	if (option[0] == 'q'){
            puts("Bye Bye!");
	    break;}
        if (option[0] == 'a'){
           pjsua_call_answer(acc_id, 200, NULL, NULL);
           puts("Press 's' to get the floor,'e' to release the floor");
        }
	if (option[0] == 'h'){
            puts("hangout the call");
	    pjsua_call_hangup_all();}
    }

    /* Destroy pjsua */
    pjsua_destroy();

    return 0;
}
