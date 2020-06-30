//#include <stdio.h>
#include <pjsua-lib/pjsua.h>
#include <mxml.h>
#define THIS_FILE	"APP"
#define SIP_PORT        5080
#define SIP_DOMAIN	"[2001:db8:409:1::8]"
#define SIP_USER	"glms"
#define SIP_PASSWD	"12345"
#define MCASR_ADDR      "ff08::10"
#define MCASR_PORT      4000

pjsua_call_id acc_id;
int g_local_port;
int group_num = 0;
char *mcast_addr;
mxml_node_t *grouplist;

/* Display error and exit application */
struct mcastRTP{
     int port;
     char *addr;
};
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}
void set_mcast_rtp(struct mcastRTP m)
{
	g_local_port = m.port;
        mcast_addr = m.addr;
}
void on_stream_created2(pjsua_call_id call_id,
                                  pjsua_on_stream_created_param *param)
{
    pjmedia_stream_pause(param->stream,PJMEDIA_DIR_ENCODING);
    
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
                        sdp->origin.addr = pj_str(mcast_addr);*/                        
                        m->conn->net_type = pj_str("IN");
                        m->conn->addr_type = pj_str("IP6");
                        m->conn->addr = pj_str(mcast_addr);//"2001:e10:6840:21:f9f9:f3e1:9f0c:a3ff");
                        char buf[13]="4001 IN IP6 ";
                        char * value;
                        value = malloc(strlen(mcast_addr)+strlen(buf));
                        strcpy(value,buf);
                        strcat(value,mcast_addr);
                        m->attr[0]->value = pj_str(value);
                       
                                               
		}

		
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
char rccs(){
     return 1;
}
pj_str_t rccs_3pcc(char *addr){
     pj_status_t status;
     mxml_node_t *m,*g,*data;
     pj_str_t text;
     data = mxmlLoadString(NULL, addr, MXML_OPAQUE_CALLBACK);
     // get group
     g = mxmlFindElement(data,data, "group",NULL,NULL,
                       MXML_DESCEND);
     for (m = mxmlFindElement(g, data, "member", NULL, NULL, MXML_DESCEND);
         m != NULL;
         m = mxmlFindElement(m, data, "member", NULL, NULL, MXML_DESCEND))
     {
         text = pj_str((char*)mxmlElementGetAttr(m, "name"));         
         if (rccs(&text)==1){
             /* 3pcc*/
             status = pjsua_call_make_call(acc_id,&text,0, NULL, NULL, NULL);
             //if (status != PJ_SUCCESS) error_exit("Error making call", status);}
          }
     }
     return pj_str("Success");
}
void on_pager_status(pjsua_call_id call_id,const pj_str_t *to, const pj_str_t *body,void *user_data,pjsip_status_code status, pj_str_t *reason){
     if (status >= 300){
            puts("Message delivery failed for message");
     }
}
char* getGroupID(int n){
     //char *new_addr = MCASR_ADDR;
     char *new_addr = malloc(sizeof(MCASR_ADDR));
     char c = n+'0';
     for (int i=0; i<=10;i++){
         new_addr[i] = *(MCASR_ADDR+i);}
     new_addr[10] = c;
     new_addr[11] = '\0';
     return new_addr;
}
/**
 * Incoming IM message (i.e. MESSAGE request)!
 */
static void on_pager(pjsua_call_id call_id, const pj_str_t *from,
                     const pj_str_t *to, const pj_str_t *contact,
                     const pj_str_t *mime_type, const pj_str_t *text)
{
    /* Note: call index may be -1 */
    pj_str_t failmem;
    char *work,*id;
    char buffer[500];
    pj_str_t t;
    mxml_node_t *g,*d,*xml,*group,*m,*member;
    // string to xml
    mxml_node_t *msg,*data;
    struct mcastRTP mcast;


    mcast.port = MCASR_PORT;
    mcast.addr = getGroupID(group_num);
    group_num+=1;
    set_mcast_rtp(mcast);    
    // save to my group list
    msg = mxmlLoadString(NULL, text->ptr, MXML_OPAQUE_CALLBACK);
    data = mxmlFindElement(msg,msg, "data",NULL,NULL,MXML_DESCEND);
    work = (char *)mxmlElementGetAttr(data, "work");
    if (strcmp("makeCall",work)==0){
       g = mxmlFindElement(msg,msg, "group",NULL,NULL,MXML_DESCEND);
       // create group in group list
       group = mxmlNewElement(mxmlGetFirstChild(grouplist), "group");    
       mxmlElementSetAttr(group,"id",mcast.addr);
       printf("group id:%s\nmember:",mcast.addr);
       for (m = mxmlFindElement(g, msg, "member", NULL, NULL, MXML_DESCEND);
            m != NULL;
            m = mxmlFindElement(m, msg, "member", NULL, NULL, MXML_DESCEND))
       {
            // add to group list
            member = mxmlNewElement(group, "member");
            mxmlElementSetAttr(member,"name",mxmlElementGetAttr(m, "name"));
            t = pj_str((char*)mxmlElementGetAttr(m, "name"));
            printf("%s \n",t.ptr);
       }
       // 3PCC make call
       puts("\nMake call\n");
       char buffer[500];
       failmem = rccs_3pcc(text->ptr);
       mxmlSaveString(grouplist,buffer, sizeof(buffer),MXML_NO_CALLBACK);
       //pjsua_im_send(acc_id, from, NULL, &failmem, NULL, NULL);
   }
   else{
      g = mxmlFindElement(msg,msg, "group",NULL,NULL,MXML_DESCEND);
      id = (char *)mxmlElementGetAttr(group, "name");     
      group = mxmlFindElement(grouplist,grouplist, "group",id,NULL,MXML_DESCEND); 
      // init group list xml
      xml = mxmlNewXML("1.0");    
      d = mxmlNewElement(grouplist, "data");
      mxmlElementSetAttr(d,"work","getList");
      mxmlAdd(d,MXML_ADD_AFTER,MXML_ADD_TO_PARENT,group);
      mxmlSaveString(grouplist,buffer, sizeof(buffer),MXML_NO_CALLBACK);
      t = pj_str(buffer);      
      pjsua_im_send(acc_id, from, NULL, &t, NULL, NULL);


   }
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
        cfg.cb.on_stream_created2 = &on_stream_created2;
	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 0;

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }
    /* set null device*/
    //pjsua_set_null_snd_dev();
    /* Add UDP transport. */
    {
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = SIP_PORT;
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
    // init group list xml
    grouplist = mxmlNewXML("1.0");    
    mxml_node_t *data;
    data = mxmlNewElement(grouplist, "data");
    /* Wait until user press "q" to quit. */
    puts("GLMS Server Start!\n");
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

