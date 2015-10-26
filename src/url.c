#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "url.h"
#include "regex_util.h"

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
	printf("Request url %s\n", real_url);

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
