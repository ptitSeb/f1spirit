#ifndef __F1S_HTTP
#define __F1S_HTTP

class F1Shttp
{
		friend size_t F1Shttp_WriteMemoryCB(void *ptr, size_t size, size_t nmemb, void *data);
		friend int F1Shttp_thread(void *me);

	public:
		F1Shttp(char *url);
		~F1Shttp();

		char *get_web(void);

	private:
		CURL *curl_handle;

		struct SDL_Thread *thread;

		char *url;
		int current_server;
		bool finished;
		char *web;
		size_t size;
};

#endif

