#ifndef _HTTP_STATUS_CODE_H_
#define _HTTP_STATUS_CODE_H_

typedef struct http_status_code_t {
	uint32_t  code;
	char information[256];
} http_status_code_t;

#endif


