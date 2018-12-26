#include "HttpChannel.h"
#include "util/format.h"

namespace Network {

static const http_parser_settings ParserSetting = {
	HttpChannel::OnParseBegin,
	HttpChannel::OnUrl,
	HttpChannel::OnStatus,
	HttpChannel::OnHeaderField,
	HttpChannel::OnHeaderValue,
	HttpChannel::OnHeaderComplete,
	HttpChannel::OnContent,
	HttpChannel::OnParseComplete,
	HttpChannel::OnChunkHeader,
	HttpChannel::OnChunkComplete,
}

HttpChannel::HttpChannel(Network::EventPoller* poller,int fd):Channel(poller,fd) {
	http_parser_init(&parser_, HTTP_REQUEST);
	url_ = "";
	content_ = "";
	status_ = "";
	field_ = "";
	value_ = "";
	phase_ = false;
	parser_.data = this;
	completed_ = false;
}

HttpChannel::~HttpChannel() {

}

void HttpChannel::HandleRead() {
	int total = reader_->GetLength();

	char* data = malloc(total);

	reader_->ReadData(data, total);

	http_parser_execute(&parser_,&ParserSetting,data,total);
	if (HTTP_PARSER_ERRNO(&parser_) != HPE_OK) {
		std::cout << http_errno_name(HTTP_PARSER_ERRNO(&parser_)) << std::endl;
		return;
	}

	if (completed_) {
		std::cout << fmt::format("url:{}", url_) << std::endl;
		std::cout << fmt::format("status:{}", status_) << std::endl;
		std::cout << fmt::format("content:{}", content_) << std::endl;

		std::map<std::string,std::string>::iterator iter;
		for (iter = headers_.begin();iter != headers_.end();iter++) {
			std::cout << fmt::format("{}:{}", iter->first, iter->second) << std::endl;
		}
	}
}

void HttpChannel::HandleError() {
	
}

void HttpChannel::SetComplete() {
	completed_ = true;
}

void HttpChannel::SetUrl(const char* data, size_t size) {
	url_.append(data, size);
}

void HttpChannel::SetStatus(const char* data, size_t size) {
	status_.append(data, size);
}

void HttpChannel::SetHeader(std::string& field, std::string& value) {
	headers_[field] = value;
}

void HttpChannel::SetContent(const char* data, size_t size) {
	content_.append(data, size);
}

int HttpChannel::OnParseBegin(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnParseComplete(struct http_parser* parser) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetComplete();
}

int HttpChannel::OnChunkHeader(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnChunkComplete(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnUrl(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetUrl(at, len);
	return 0;
}

int HttpChannel::OnStatus(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetStatus(at, len);
	return 0;
}

int HttpChannel::OnHeaderField(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	if (channel->phase_) {
		channel->phase_ = false;
		channel->SetHeader(channel->field_, channel->value_);
		channel->field_ = "";
		channel->value_ = "";
	}
	channel->field_.append(at, len);
}

int HttpChannel::OnHeaderValue(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->phase_ = true;
	channel->value_.append(at, len);
}

int HttpChannel::OnHeaderComplete(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnContent(struct http_parser* parser,const char* at,size_t len);
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetContent(at, len);
	return 0;
};
