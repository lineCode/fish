#include "HttpChannel.h"
#include "util/format.h"
#include "logger/Logger.h"
#include "network/TcpReader.h"
#include "network/FastReader.h"
#include "network/TcpWriter.h"
#include <iostream>


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
};

static std::unordered_map<uint32_t, const std::string> StatusMsg = {
#define XX(num, name, string) { num, #string },
	HTTP_STATUS_MAP(XX)
#undef XX
};

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
	callback_ = nullptr;
	userdata_ = nullptr;

	SetReader(new FastReader(1024));
	SetWriter(new TcpWriter());
}

HttpChannel::~HttpChannel() {

}

void HttpChannel::HandleRead() {
	int total = reader_->GetTotal();

	char* data = reader_->ReadData(total);

	http_parser_execute(&parser_, &ParserSetting, data, total);

	if (HTTP_PARSER_ERRNO(&parser_) != HPE_OK) {
		LOG_ERROR(fmt::format("http parse error:{}", http_errno_name(HTTP_PARSER_ERRNO(&parser_))));
		Close(true);
		return;
	}

	if (completed_) {
		if (callback_) {
			callback_(this, userdata_);
		}
	}
}

void HttpChannel::HandleError() {
	
}

void HttpChannel::SetComplete() {
	completed_ = true;
}

bool HttpChannel::GetComplete() {
	return completed_;
}

void HttpChannel::SetCallback(OnComplete callback, void* userdata) {
	callback_ = callback;
	userdata_ = userdata;
}

std::string HttpChannel::GetMethod() {
	if (!completed_) {
		return std::string("");
	}
	return std::string(http_method_str((http_method)parser_.method));
}

void HttpChannel::SetUrl(const char* data, size_t size) {
	url_.append(data, size);
}

std::string HttpChannel::GetUrl() {
	return url_;
}

void HttpChannel::SetStatus(const char* data, size_t size) {
	status_.append(data, size);
}

std::string HttpChannel::GetStatus() {
	return status_;
}

void HttpChannel::SetHeader(std::string& field, std::string& value) {
	receiveHeaders_[field] = value;
}

HttpChannel::HeadersMap& HttpChannel::GetHeader() {
	return receiveHeaders_;
}

void HttpChannel::SetContent(const char* data, size_t size) {
	content_.append(data, size);
}

std::string HttpChannel::GetContent() {
	return content_;
}

void HttpChannel::SetReplyHeader(const char* field, const char* value) {
	replyHeaders_[field] = value;
}

void HttpChannel::SetReplyHeader(std::string& field, std::string& value) {
	replyHeaders_[field] = value;
}
	
void HttpChannel::Reply(uint32_t code, std::string& content) {
	Reply(code, (char*)content.c_str(), content.length());
}

void HttpChannel::Reply(uint32_t code, char* content, size_t size) {
	std::string message = "";
	message += fmt::format("HTTP/1.1 {} {}\r\n", code, StatusMsg[code]);

	HeadersMap::iterator iter = replyHeaders_.begin();
	for ( ; iter != replyHeaders_.end(); iter++ ) {
		message += fmt::format("{}: {}\r\n", iter->first, iter->second);
	}

	if ( size != 0 ) {
		message += fmt::format("Content-Length: {}\r\n\r\n", size);
		message.append(content, size);
	}

	Write(message, NULL);
	Close(false);
}

int HttpChannel::OnParseBegin(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnParseComplete(struct http_parser* parser) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetComplete();
	return 0;
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
	return 0;
}

int HttpChannel::OnHeaderValue(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->phase_ = true;
	channel->value_.append(at, len);
	return 0;
}

int HttpChannel::OnHeaderComplete(struct http_parser* parser) {
	return 0;
}

int HttpChannel::OnContent(struct http_parser* parser,const char* at,size_t len) {
	HttpChannel* channel = (HttpChannel*)parser->data;
	channel->SetContent(at, len);
	return 0;
}

};
