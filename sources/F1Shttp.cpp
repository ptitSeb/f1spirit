#include <stdio.h>
#include "stdlib.h"
#include "string.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "F1Shttp.h"

#include "SDL.h"
#include "SDL_thread.h"


int n_servers = 2;
char *servers[2] = {"http://www.braingames.getput.com",
                    "http://www2.braingames.getput.com"
                   };


void *myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	   NULL pointers, so we take care of it here */
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
} /* myrealloc */


size_t F1Shttp_WriteMemoryCB(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	F1Shttp *mem = (F1Shttp *)data;

	mem->web = (char *)myrealloc(mem->web, mem->size + realsize + 1);

	if (mem->web) {
		memcpy(&(mem->web[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->web[mem->size] = 0;
	}

	return realsize;
} /* F1Shttp_WriteMemoryCB */



int F1Shttp_thread(void *me)
{
	char final_url[2048];
	F1Shttp *p = (F1Shttp *)me;

	curl_global_init(CURL_GLOBAL_ALL);

	do {
		sprintf(final_url, "%s/%s", servers[p->current_server++], p->url);
		p->curl_handle = curl_easy_init();
		curl_easy_setopt(p->curl_handle, CURLOPT_URL, final_url);
		curl_easy_setopt(p->curl_handle, CURLOPT_WRITEFUNCTION, F1Shttp_WriteMemoryCB);
		curl_easy_setopt(p->curl_handle, CURLOPT_WRITEDATA, (void *)p);
		curl_easy_setopt(p->curl_handle, CURLOPT_TIMEOUT, 10);

		curl_easy_perform(p->curl_handle);
	} while (p->web == 0 && p->current_server < n_servers);

	if (p->web != 0)
		p->finished = true;

	return 0;
} /* F1Shttp_thread */



F1Shttp::F1Shttp(char *p_url)
{
	finished = false;
	web = 0;
	size = 0;
	current_server = 0;
	url = new char[strlen(p_url) + 1];
	strcpy(url, p_url);

	thread = SDL_CreateThread(F1Shttp_thread, (void *)this);

} 



F1Shttp::~F1Shttp()
{
	SDL_KillThread(thread);

	curl_easy_cleanup(curl_handle);

	if (url != 0)
		delete url;

	url = 0;

	if (web != 0)
		free(web);

	web = 0;

} 


char *F1Shttp::get_web(void)
{
	if (finished)
		return web;

	return 0;
} 

