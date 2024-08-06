## Process this file with automake to produce Makefile.in

bin_SCRIPTS = btree-dump convert-dom-db convert-ip-db expire-ip-db getdominfo rebuild-dom-db rebuild-ip-db reresolve make-report
man_MANS = btree-dump.1 convert-dom-db.1 convert-ip-db.1 expire-ip-db.1 getdominfo.1 rebuild-dom-db.1 rebuild-ip-db.1 reresolve.1 make-report.1
frdatadir = $(datadir)/fastresolve
frdata_DATA = analog.cfg
EXTRA_DIST = $(man_MANS) $(frdata_DATA) make-report.1.in

make-report.1: make-report.1.in FORCE
	@PERL@ -pe "s,DATADIR,${datadir},g" ${srcdir}/make-report.1.in > $@
CLEANFILES = ${bin_SCRIPTS} make-report.1

FORCE:
