Index: src/check.c
===================================================================
--- src/check.c	2000/09/27 01:33:23	1.1.1.4
+++ src/check.c	2000/09/27 01:45:04	1.4
@@ -109,7 +109,7 @@
   
   DLIST_CHECK(ads->udpw, qu, , {
     assert(qu->state==query_tosend);
-    assert(qu->retries <= UDPMAXRETRIES);
+    assert(qu->retries <= ads->udpmaxretries);
     assert(qu->udpsent);
     assert(!qu->children.head && !qu->children.tail);
     checkc_query(ads,qu);
Index: src/internal.h
===================================================================
--- src/internal.h	2000/09/27 01:33:23	1.1.1.6
+++ src/internal.h	2000/09/27 01:45:04	1.8
@@ -288,6 +288,7 @@
   int nextid, udpsocket, tcpsocket;
   vbuf tcpsend, tcprecv;
   int nservers, nsortlist, nsearchlist, searchndots, tcpserver, tcprecv_skip;
+  int udpmaxretries, udpretryms, tcpms;
   enum adns__tcpstate {
     server_disconnected, server_connecting,
     server_ok, server_broken
Index: src/setup.c
===================================================================
--- src/setup.c	2000/09/27 01:33:23	1.1.1.5
+++ src/setup.c	2000/09/27 01:45:04	1.9
@@ -227,6 +227,7 @@
       ads->iflags |= adns_if_debug;
       continue;
     }
+
     if (l>=6 && !memcmp(word,"ndots:",6)) {
       v= strtoul(word+6,&ep,10);
       if (l==6 || ep != word+l || v > INT_MAX) {
@@ -236,6 +237,37 @@
       ads->searchndots= v;
       continue;
     }
+
+    if (l>=14 && !memcmp(word,"udpmaxretries:",14)) {
+      v= strtoul(word+14,&ep,10);
+      if (l==14 || ep != word+l || v > INT_MAX) {
+	configparseerr(ads,fn,lno,"option `%.*s' malformed or has bad value",l,word);
+	continue;
+      }
+      ads->udpmaxretries= v;
+      continue;
+    }
+
+    if (l>=11 && !memcmp(word,"udpretryms:",11)) {
+      v= strtoul(word+11,&ep,10);
+      if (l==11 || ep != word+l || v > INT_MAX) {
+	configparseerr(ads,fn,lno,"option `%.*s' malformed or has bad value",l,word);
+	continue;
+      }
+      ads->udpretryms= v;
+      continue;
+    }
+
+    if (l>=6 && !memcmp(word,"tcpms:",6)) {
+      v= strtoul(word+6,&ep,10);
+      if (l==6 || ep != word+l || v > INT_MAX) {
+	configparseerr(ads,fn,lno,"option `%.*s' malformed or has bad value",l,word);
+	continue;
+      }
+      ads->tcpms= v;
+      continue;
+    }
+
     if (l>=12 && !memcmp(word,"adns_checkc:",12)) {
       if (!strcmp(word+12,"none")) {
 	ads->iflags &= ~adns_if_checkc_freq;
@@ -251,6 +283,7 @@
       }
       continue;
     }
+
     adns__diag(ads,-1,0,"%s:%d: unknown option `%.*s'", fn,lno, l,word);
   }
 }
@@ -478,6 +511,9 @@
   adns__vbuf_init(&ads->tcprecv);
   ads->tcprecv_skip= 0;
   ads->nservers= ads->nsortlist= ads->nsearchlist= ads->tcpserver= 0;
+  ads->udpmaxretries = UDPMAXRETRIES;
+  ads->udpretryms = UDPRETRYMS;
+  ads->tcpms = TCPWAITMS;
   ads->searchndots= 1;
   ads->tcpstate= server_disconnected;
   timerclear(&ads->tcptimeout);
Index: src/transmit.c
===================================================================
--- src/transmit.c	2000/09/27 01:33:23	1.1.1.6
+++ src/transmit.c	2000/09/27 01:45:04	1.9
@@ -215,7 +215,7 @@
 static void query_usetcp(adns_query qu, struct timeval now) {
   qu->state= query_tcpw;
   qu->timeout= now;
-  timevaladd(&qu->timeout,TCPWAITMS);
+  timevaladd(&qu->timeout,qu->ads->tcpms);
   LIST_LINK_TAIL(qu->ads->tcpw,qu);
   adns__querysend_tcp(qu,now);
   adns__tcp_tryconnect(qu->ads,now);
@@ -232,7 +232,8 @@
     return;
   }
 
-  if (qu->retries >= UDPMAXRETRIES) {
+  ads= qu->ads;
+  if (qu->retries >= ads->udpmaxretries) {
     adns__query_fail(qu,adns_s_timeout);
     return;
   }
@@ -240,7 +241,6 @@
   serv= qu->udpnextserver;
   memset(&servaddr,0,sizeof(servaddr));
 
-  ads= qu->ads;
   servaddr.sin_family= AF_INET;
   servaddr.sin_addr= ads->servers[serv].addr;
   servaddr.sin_port= htons(DNS_PORT);
@@ -251,7 +251,7 @@
   if (r<0 && errno != EAGAIN) adns__warn(ads,serv,0,"sendto failed: %s",strerror(errno));
   
   qu->timeout= now;
-  timevaladd(&qu->timeout,UDPRETRYMS);
+  timevaladd(&qu->timeout,ads->udpretryms);
   qu->udpsent |= (1<<serv);
   qu->udpnextserver= (serv+1)%ads->nservers;
   qu->retries++;
Index: dynamic/Makefile.in
===================================================================
--- dynamic/Makefile.in	2000/09/27 01:33:22	1.1.1.5
+++ dynamic/Makefile.in	2000/09/27 03:36:37	1.2
@@ -33,6 +33,7 @@
 install:
 		$(INSTALL_PROGRAM) $(SHLIBFILE) $(lib_dir)/$(SHLIBFILE)
 		ln -sf $(SHLIBFILE) $(lib_dir)/$(SHLIBSONAME)
+		ln -sf $(SHLIBFILE) $(lib_dir)/$(SHLIBFORLINK)
 
 uninstall:
 		rm -f $(lib_dir)/$(SHLIBFILE) $(lib_dir)/$(SHLIBSONAME)
