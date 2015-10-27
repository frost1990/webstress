#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "url.h"
#include "screen.h"
#include "regex_util.h"

static unsigned char char2hex(unsigned char x) 
{ 
	return (unsigned char)(x > 9 ? x + 55: x + 48); 
} 

static bool is_numeric(unsigned char c) 
{ 
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))  {
		return true; 	
	} else {
		return false; 
	}
} 

void urlencode(unsigned char *src, int  src_len, unsigned char *dest, int  dest_len) 
{ 
	unsigned char ch; 
	int len = 0; 

	while (len < (dest_len - 4) && *src) { 
		ch = (unsigned char)*src; 
		if (*src == ' ') { 
			*dest++ = '+'; 
		} else if (is_numeric(ch) || strchr("-_.!~*'()", ch)) { 
			*dest++ = *src; 
		} else { 
			*dest++ = '%'; 
			*dest++ = char2hex( (unsigned char)(ch >> 4) ); 
			*dest++ = char2hex( (unsigned char)(ch % 16) ); 
		}  
		++src; 
		++len; 
	} 

	*dest = 0; 
	return ; 
} 

unsigned char* urldecode(unsigned char* encode, unsigned char* decode) 
{ 
	char *enc = (char *)encode; 
	char p[2]; 
	int j = 0; 

	for (int i = 0; i < strlen(enc); i++ ) { 
		memset(p, '\0', 2); 
		if (enc[i] != '%') { 
			decode[j++] = enc[i]; 
			continue; 
		} 
		p[0] = enc[++i]; 
		p[1] = enc[++i]; 

		p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0); 
		p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0); 
		decode[j++] = (unsigned char)(p[0] * 16 + p[1]); 

	} 
	decode[j] = '\0'; 
	return decode; 
}

int parse_url(const char *url, struct http_parser_url *u, http_request *request)
{
	if (url == NULL) {
		return -1;
	}
	char real_url[1024] = {0};
	if ((is_match_pattern(url, REGEX_URL_SCHEME)) != 0)  {
		snprintf(real_url, 1024, "http://%s", url);
	} else {
		snprintf(real_url, 1024, "%s", url);
	}
	SCREEN(SCREEN_YELLOW, stdout, "Request url:");
	SCREEN(SCREEN_DARK_GREEN, stdout, " %s\n", real_url);

	int url_len = strlen(real_url);
	int ret = http_parser_parse_url(real_url, url_len, 0, u);
	if (ret != 0) {
		printf("Invalid request url!\n");
		return -1;
	}

	char strport[64] = {0};
	for (unsigned i = 0; i < UF_MAX; i++) {
		if ((u->field_set & (1 << i)) == 0) {
			continue;
		}
		switch (i) {
			case 0:
				snprintf(request->scheme, 16, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				break;
			case 1:
				snprintf(request->host, 256, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				break;
			case 2:
				snprintf(strport, 64, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				request->port = atoi(strport);
				break;
			case 3:
				snprintf(request->path, 256, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				break;
			case 4:
				snprintf(request->querystring, 1024, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				break;
			case 5:
				snprintf(request->fragment, 256, "%.*s", u->field_data[i].len, real_url + u->field_data[i].off);
				break;
			default:
				break;
		}
	}
	return 0;
}
