/*                                                                         */
/*  mod_auth_pg.c       Copyright 1998-2000 Min Sik Kim <minskim@bawi.org> */
/*                                                                         */

#include <libpq-fe.h>

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"

#define AUTH_PG_VERSION "1.2"

typedef struct {
    int cookie;     /* on/off */

    char *host;     /* Host name     */
    char *port;     /* Port number   */
    char *user;     /* Username to connect to database with */
    char *password; /* Password to connect to database with */
    char *options;  /* Options */
    char *database; /* Database name */

    char *usr_table; /* password table name */
    char *grp_table; /* group table name */

    char *pwd_field; /* password field name */
    char *uname_field; /* user name field name */
    char *gname_field; /* group name field name */

    char *query;     /* query to use instead of the default one */

    int virtual; /* Use user@domain style userids */
    char *domain_field; /* domain name field name */

    int anonymous;
    char *anonymous_username;

    int encrypted;
    int authoritative;

    char *cookie_name;
    char *separator;
} auth_pg_config;

void *create_auth_pg_dir_config(pool *p, char *d)
{
    auth_pg_config *sec
	= (auth_pg_config *)ap_pcalloc(p, sizeof(auth_pg_config));
    if (!sec) {
	fprintf(stderr, "mod_auth_pg: Memory allocation error\n");
	return NULL;
    }

    sec->cookie = 0;
    sec->host = NULL;
    sec->port = NULL;
    sec->user = NULL;
    sec->password = NULL;
    sec->options = NULL;
    sec->database = NULL;
    sec->usr_table = NULL;
    sec->grp_table = NULL;
    sec->pwd_field = "password";
    sec->uname_field = "userid";
    sec->gname_field = NULL;
    sec->query = NULL;
    sec->virtual = 0;
    sec->domain_field = NULL;
    sec->anonymous = 0;
    sec->anonymous_username = "guest";
    sec->encrypted = 1;
    sec->authoritative = 1;

    sec->cookie_name = "AUTHPG";
    sec->separator = ":";

    return sec;
}

command_rec auth_pg_cmds[] = {
    { "AuthPGCookie", ap_set_flag_slot,
      (void*)XtOffsetOf(auth_pg_config, cookie),
      OR_AUTHCFG, FLAG, NULL },
    { "AuthPGHost", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, host),
      OR_AUTHCFG, TAKE1, "The host name of the postgreSQL server." },
    { "AuthPGPort", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, port),
      OR_AUTHCFG, TAKE1, "Port number." },
    { "AuthPGUser", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, user),
      OR_AUTHCFG, TAKE1, "The username to connect to the database with." },
    { "AuthPGPassword", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, password),
      OR_AUTHCFG, TAKE1, "The password to connect to the database with." },
    { "AuthPGOptions", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, options),
      OR_AUTHCFG, TAKE1, "Options." },
    { "AuthPGDatabase", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, database),
      OR_AUTHCFG, TAKE1, "Name of database which contains the password tables." },
    { "AuthPGUserTable", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, usr_table),
      OR_AUTHCFG, TAKE1, "Name of the table which contains the password/user-name combination." },
    { "AuthPGGroupTable", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, grp_table),
      OR_AUTHCFG, TAKE1, "Name of the table which contains the groups" },
    { "AuthPGPasswordField", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, pwd_field),
      OR_AUTHCFG, TAKE1, NULL },
    { "AuthPGUserNameField", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, uname_field),
      OR_AUTHCFG, TAKE1, NULL },
    { "AuthPGGroupNameField", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, gname_field),
      OR_AUTHCFG, TAKE1, NULL },
    { "AuthPGQuery", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, query),
      OR_AUTHCFG, TAKE1, NULL },
    { "AuthPGVirtual", ap_set_flag_slot,
      (void*)XtOffsetOf(auth_pg_config, virtual),
      OR_AUTHCFG, FLAG, NULL },
    { "AuthPGDomainField", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, domain_field),
      OR_AUTHCFG, TAKE1, NULL },
    { "AuthPGAnonymous", ap_set_flag_slot,
      (void*)XtOffsetOf(auth_pg_config, anonymous),
      OR_AUTHCFG, FLAG, NULL },
    { "AuthPGAnonymousUserName", ap_set_string_slot,
      (void *) XtOffsetOf (auth_pg_config, anonymous_username),
      OR_AUTHCFG, TAKE1, NULL },

    { "AuthPGEncrypted", ap_set_flag_slot,
      (void*)XtOffsetOf(auth_pg_config, encrypted),
      OR_AUTHCFG, FLAG, NULL },
    { "AuthPGAuthoritative", ap_set_flag_slot,
      (void*)XtOffsetOf(auth_pg_config, authoritative),
      OR_AUTHCFG, FLAG, NULL },

    { "AuthPGCookieName", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, cookie_name),
      OR_AUTHCFG, FLAG, NULL },
    { "AuthPGCookieSeparator", ap_set_string_slot,
      (void*)XtOffsetOf(auth_pg_config, separator),
      OR_AUTHCFG, FLAG, NULL },
    { NULL }
};

module auth_pg_module;

char query[8192], dbconnect[1024];

void auth_pg_init(server_rec *s, pool *p)
{
    ap_add_version_component("AuthPG/" AUTH_PG_VERSION);
}

char *get_pg_pw(request_rec *r, char *user, auth_pg_config *sec, char errstr[])
{
    char *userid, *passwd, *domain = NULL;
    PGconn *conn;
    PGresult *res;

    userid = ap_pstrdup(r->pool, user);

    if (sec->query == NULL) {
      if (sec->virtual) {
        if (domain = strchr(userid,'@')) {
          *domain++ = '\0';
          ap_snprintf(query, sizeof(query),
            "select %s from %s where %s='%s' and %s='%s'",
            sec->pwd_field, sec->usr_table, sec->uname_field,
            userid, sec->domain_field, domain);
        } else {
          ap_snprintf(query, sizeof(query),
            "select %s from %s where %s='%s' and (%s='' or %s is null)",
            sec->pwd_field, sec->usr_table, sec->uname_field,
            userid, sec->domain_field, sec->domain_field);
        }
      } else {
        ap_snprintf(query, sizeof(query), "select %s from %s where %s='%s'",
          sec->pwd_field, sec->usr_table, sec->uname_field, userid);
      }
    } else {
      ap_snprintf(query, sizeof(query), sec->query, userid);
    }

    ap_snprintf(dbconnect,sizeof(dbconnect),"dbname=%s%s%s%s%s%s%s%s%s%s%s",
      sec->database,
      sec->host ? " host=" : "",
      sec->host ? sec->host : "",
      sec->port ? " port=" : "",
      sec->port ? sec->port : "",
      sec->user ? " user=" : "",
      sec->user ? sec->user : "",
      sec->password ? " password=" : "",
      sec->password ? sec->password : "",
      sec->options ? " options=" : "",
      sec->options ? sec->options : ""
    );

    conn = PQconnectdb(dbconnect);
    if (PQstatus(conn) == CONNECTION_BAD) {
      ap_snprintf(errstr, MAX_STRING_LEN, "Could not connect to postgreSQL backend (%s)", PQerrorMessage(conn));
      PQfinish(conn);
      return NULL;
    }

    res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
	PQclear(res);
	PQfinish(conn);
	ap_snprintf(errstr, MAX_STRING_LEN, "Could not execute the query: %s",
		    query);
	return NULL;
    }
    if (PQntuples(res) > 1) {
	PQclear(res);
	PQfinish(conn);
	ap_snprintf(errstr, MAX_STRING_LEN, "User %s has several passwords.",
		    user);
	return NULL;
    }
    if (PQntuples(res) == 0) {
	PQclear(res);
	PQfinish(conn);
	return NULL;
    }
    passwd = ap_pstrdup(r->pool, PQgetvalue(res, 0, 0));
    PQclear(res);
    PQfinish(conn);
    return passwd;
}

table *groups_for_pg_user(pool *p, const char *user, auth_pg_config *sec,
			  char errstr[])
{
    char *userid, *domain = NULL;
    table *grps;
    PGconn *conn;
    PGresult *res;
    int i;

    userid = ap_pstrdup(p, user);

    if (sec->virtual) {
      if (domain = strchr(userid,'@')) {
        *domain++ = '\0';
        ap_snprintf(query, sizeof(query),
          "select %s from %s where %s='%s' and %s='%s'",
          sec->gname_field, sec->grp_table, sec->uname_field,
          userid, sec->domain_field, domain);
      } else {
        ap_snprintf(query, sizeof(query),
          "select %s from %s where %s='%s' and (%s='' or %s is null)",
          sec->gname_field, sec->grp_table, sec->uname_field,
          userid, sec->domain_field, sec->domain_field);
      }
    } else {
      ap_snprintf(query, sizeof(query), "select %s from %s where %s='%s'",
        sec->gname_field, sec->grp_table, sec->uname_field, userid);
    }

    ap_snprintf(dbconnect, sizeof(dbconnect), "dbname=%s%s%s%s%s%s%s%s%s%s%s",
      sec->database,
      sec->host ? " host=" : "",
      sec->host ? sec->host : "",
      sec->port ? " port=" : "",
      sec->port ? sec->port : "",
      sec->user ? " user=" : "",
      sec->user ? sec->user : "",
      sec->password ? " password=" : "",
      sec->password ? sec->password : "",
      sec->options ? " options=" : "",
      sec->options ? sec->options : ""
    );

    conn = PQconnectdb(dbconnect);
    if (PQstatus(conn) == CONNECTION_BAD) {
	ap_snprintf(errstr, MAX_STRING_LEN, "Could not connect to postgreSQL backend (%s)", PQerrorMessage(conn));
	PQfinish(conn);
	return NULL;
    }

    res = PQexec(conn, query);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
      PQclear(res); /*- When res is NULL, I don't have to clear. */
      PQfinish(conn);
      ap_snprintf(errstr, MAX_STRING_LEN, "Could not execute the query.");
      return NULL;
    }

    grps = ap_make_table(p, PQntuples(res));
    for (i = 0; i < PQntuples(res); i++) {
	ap_table_set(grps, PQgetvalue(res, i, 0), "in");
    }

    PQclear(res);
    PQfinish(conn);
    return grps;
}

/*
 * Determine user ID, and check if it really is that user,
 * for HTTP basic authentication.
 */
int pg_authenticate_basic_user(request_rec *r)
{
    char *crypt(const char *key, const char *salt);
    auth_pg_config *sec =
	(auth_pg_config *)ap_get_module_config(r->per_dir_config,
					       &auth_pg_module);
    conn_rec *c = r->connection;
    char *sent_pw, *str;
    const char *cstr;
    char errstr[MAX_STRING_LEN];
    int res;

    if (!(sec->database) || !(sec->query) && !(sec->usr_table))
	if (sec->authoritative) {
	    ap_note_basic_auth_failure(r);
	    return AUTH_REQUIRED;
	} else return DECLINED;

    if (sec->cookie) {
	char *cookie;
	if (!(cstr = ap_table_get(r->headers_in, "Cookie")))
	    if (sec->authoritative) {
		ap_log_reason("No cookie available.", r->uri, r);
		ap_note_basic_auth_failure(r);
		return AUTH_REQUIRED;
	    } else return DECLINED;
	res = strlen(cstr);  /* length of cookie */
	if (!(cookie = (char*) ap_palloc(r->pool, res + 2))) {
	    ap_log_reason("Could not allocate memory for a cookie", r->uri, r);
	    return SERVER_ERROR;
	}
	memcpy(cookie, cstr, res);
	cookie[res++] = ';';
	cookie[res] = '\0';
	for (cookie = strtok(cookie, " ;\n\r\t\f"); cookie;
	     cookie = strtok(NULL, " ;\n\r\t\f")) {
	    if (!(str = strchr (cookie, '='))) /* Invalid cookie */
		continue;
	    *(str++) = '\0';
	    if (strcmp(cookie, sec->cookie_name) == 0) {
		sent_pw = strchr(str, sec->separator[0]);
		if (sent_pw) *(sent_pw++) = '\0';
		else sent_pw = "";
		c->user = str;
		break;
	    }
	}
    } else
	if ((res = ap_get_basic_auth_pw(r, &sent_pw)))
	    return res;

    if ((sec->anonymous) &&
	strcmp(c->user, sec->anonymous_username) == 0) return OK;

    errstr[0] = '\0';
    if (!(str = get_pg_pw(r, c->user, sec, errstr))) {
	if (errstr[0] == '\0') {
	    if (!(sec->authoritative))
		return DECLINED;
	    ap_snprintf(errstr, sizeof(errstr), "user %s not found", c->user);
	}
	ap_log_reason(errstr, r->uri, r); /*- How about r->filename? */
	ap_note_basic_auth_failure(r);
	return AUTH_REQUIRED;
    }

    if (strcmp(str, sec->encrypted ? crypt(sent_pw, str) : sent_pw)) {
	ap_snprintf(errstr, sizeof(errstr),
		 "user %s: password mismatch", c->user);
	ap_log_reason(errstr, r->uri, r);
	ap_note_basic_auth_failure(r);
	return AUTH_REQUIRED;
    }
    return OK;
}

/*
 * Checking ID
 */
int pg_check_user_auth(request_rec *r)
{
    auth_pg_config *sec =
	(auth_pg_config *)ap_get_module_config(r->per_dir_config,
					    &auth_pg_module);
    char *user = r->connection->user;
    int m = r->method_number;
    register int x;
    const char *t, *w;
    table *grpstatus = NULL;
    const array_header *reqs_arr = ap_requires(r);
    require_line *reqs;

    char errstr[MAX_STRING_LEN];

    /* If there is no "require" directive, then any user will do. */
    if (!reqs_arr) return OK;

    if (sec->anonymous && strcmp(user, sec->anonymous_username) == 0) return OK;

    reqs = (require_line *)reqs_arr->elts;

    for (x = 0; x < reqs_arr->nelts; x++) {

	if (!(reqs[x].method_mask & (1 << m))) continue;

	t = reqs[x].requirement;
	w = ap_getword(r->pool, &t, ' ');
	if (!strcmp(w, "valid-user"))
	    return OK;
	if (!strcmp(w, "user")) {
	    while (t[0]) {
		w = ap_getword_conf(r->pool, &t);
		if (!strcmp(user, w))
		    return OK;
	    }
	} else if (!strcmp(w, "group")) {
	    if (sec->grp_table && sec->gname_field)
		grpstatus = groups_for_pg_user(r->pool, user, sec, errstr);

	    if (!grpstatus) {
	        if (sec->authoritative) {
		    ap_snprintf(errstr, sizeof(errstr), "user %s denied, no access rules specified.", user);
		    ap_log_reason(errstr, r->uri, r);
		    ap_note_basic_auth_failure(r);
		    return AUTH_REQUIRED;
		}
		return DECLINED;
	    }

	    while (t[0]) {
		w = ap_getword_conf(r->pool, &t);
		if (ap_table_get(grpstatus, w))
		    return OK;
	    }
	}
    }

    if (!(sec->authoritative))
	return DECLINED;

    ap_note_basic_auth_failure(r); /*-log?*/
    return AUTH_REQUIRED;
}

module auth_pg_module = {
    STANDARD_MODULE_STUFF,
    auth_pg_init,               /* initializer */
    create_auth_pg_dir_config,  /* dir config creater */
    NULL,                       /* dir merger --- default is to override */
    NULL,                       /* server config */
    NULL,                       /* merge server config */
    auth_pg_cmds,               /* command table */
    NULL,                       /* handlers */
    NULL,                       /* filename translation */
    pg_authenticate_basic_user, /* check_user_id */
    pg_check_user_auth,         /* check auth */
    NULL,                       /* check access */
    NULL,                       /* type_checker */
    NULL,                       /* fixups */
    NULL,                       /* logger */
    NULL                        /* header parser */
};
