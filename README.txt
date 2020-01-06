ReadMe - Push to talk over Multicast IPV6
---Kamailio
---IPV6 Multicast & Push to talk setting
---PTT UA
---GLMS
---TBCP server



![](https://paper-attachments.dropbox.com/s_F095D803CACCE1CE8CDD4829D54E37D04EAB1620AEFAA56F81CA29A45676D9AD_1578316692047_ptt_flowchart3-version4.png)



Kamailio
    $ sudo apt install kamailio kamailio-mysql-modules
    $ sudo apt install kamailio-websocket-modules
    $ which kamailio
    $ kamailio -V
    $ sudo -i
    $ cd /etc/kamailio/
    $ cp kamailio.cfg kamailio.cfg.bkg
    $ vim kamailio.cfg
    alias='nicolewu.tk'
    /* uncomment and configure the following line if you want Kamailio to
     * bind on a specific interface/port/proto (default bind on all available) */
    # listen=udp:10.0.0.10:5060
    listen= 163.22.21.85
    listen = 2001:e10:6840:21:1e6f:65ff:fed3:dd9
    $ kamctl ul show

如果要關掉service

    service kamailio stop

可以用以下指令檢查一下聽在哪個port

    netstat -nltp

如果遇到錯誤core/udp_server.c:397]: udp_init(): bind ,可以重開機試試看


IPV6 Multicast & Push to talk setting
1. 因為pjsip2.9不支援ipv6 multicast,可參考此步驟,自行更改,或直接下載更改過後的pjproject2.9
2. 如果發現沒辦法收到ipv6 multicast 的封包,可以用netstat -gn 查看是否有加入群組
PTT UA
1. Library compile
    $ apt-get install  build-essential libasound2-dev
    $ wget https://www.pjsip.org/release/2.9/pjproject-2.9.tar.bz2
    $ tar -xf pjproject-2.9.tar.bz2 && cd pjproject-2.9/
    $ ./configure --enable-shared
    $ make dep
    $ make
    $ sudo make install
    $ export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib


2. 如果遇到make error可以用以下指令後在重新make
    make distclean
    make clean
    make realclean


3. 如果出現找不到default audio device
    請新開一個terminal
    $ apt-get install libasound2-dev
    $ cd pjproject-2.9/
    $ ./configure --enable-shared
    $ make dep
    $ make
    $ sudo make install


4. compile PTT UA
    $ gcc ptt.c -o ptt `pkg-config --cflags --libs libpjproject`
    $ ./ptt


5. How to make call
    依畫面指示按下m 可以輸入想要邀請的使用者 e.g. nicole alice
    依畫面指示按下a 可以接受通話
    依畫面指示按下s 可以取得發化權並發話
    依畫面指示按下e 可以釋放發化權
    依畫面指示按下h 可以結束通話
    依畫面指示按下q 可以結束通話
GLMS
1. Library compile同PTT UA第1-3步驟
2. compile GLMS 3PCC
    $ gcc glms.c -o glms `pkg-config --cflags --libs libpjproject`
    $ ./glms
3. How to make call
    依畫面指示按下h 可以結束所有通話


TBCP Server
1. Library compile同PTT UA第1-3步驟
2. compile GLMS 3PCC TBCP
    $ gcc tbcps.c -o tbcps `pkg-config --cflags --libs libpjproject`
    $ ./tbcps
1. How to make call
    依畫面指示按下h 可以結束所有通話


