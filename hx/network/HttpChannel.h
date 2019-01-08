#ifndef HTTP_CHANNEL_H
#define HTTP_CHANNEL_H
#include "Channel.h"
#include "http_parser.h"
#include <functional>
#include <unordered_map>

namespace Network {

class HttpChannel : public Channel {
public:
	typedef std::function<void(Network::HttpChannel*, void*)> OnComplete;
	typedef std::unordered_map<std::string, std::string> HeadersMap;

public:
	HttpChannel(Network::EventPoller* poller,int fd);

	virtual ~HttpChannel();

	virtual void HandleRead();

	virtual void HandleError();

	virtual void SetCallback(OnComplete callback_, void* userdata);

	virtual void SetComplete();

	virtual bool GetComplete();

	virtual std::string GetMethod();
	
	virtual void SetUrl(const char* data, size_t size);

	virtual std::string GetUrl();

	virtual void SetStatus(const char* data, size_t size);

	virtual std::string GetStatus();
	
	virtual void SetHeader(std::string& field, std::string& value);

	virtual HeadersMap& GetHeader();

	virtual void SetContent(const char* data, size_t size);

	virtual std::string GetContent();

	virtual void SetReplyHeader(const char* field, const char* value);
	virtual void SetReplyHeader(std::string& field, std::string& value);

	virtual void Reply(uint32_t code, std::string& content);
	virtual void Reply(uint32_t code, char* content, size_t size);

	static int OnParseBegin(struct http_parser* parser);
	static int OnParseComplete(struct http_parser* parser);

	static int OnChunkHeader(struct http_parser* parser);
	static int OnChunkComplete(struct http_parser* parser);

	static int OnUrl(struct http_parser* parser,const char* at,size_t length);
	static int OnStatus(struct http_parser* parser,const char* at,size_t length);

	static int OnHeaderField(struct http_parser* parser,const char* at,size_t length);
	static int OnHeaderValue(struct http_parser* parser,const char* at,size_t length);
	static int OnHeaderComplete(struct http_parser* parser);

	static int OnContent(struct http_parser* parser,const char* at,size_t length);

public:
	std::string field_;
	std::string value_;
	int phase_;

private:
	struct http_parser parser_;
	HeadersMap receiveHeaders_;
	std::string url_;
	std::string content_;
	std::string status_;
	bool completed_;
	OnComplete callback_;
	void* userdata_;

	HeadersMap replyHeaders_;
};
};

#endif