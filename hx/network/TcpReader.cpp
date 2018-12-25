#include "TcpReader.h"
#include "Network.h"
#include "Channel.h"

namespace Network
{
	TcpReader::TcpReader(Channel* channel, int header, int buffsize) :Reader(channel, buffsize)
	{
		header_ = header;
		left_ = 0;
		state_ = Header;
	}

	TcpReader::~TcpReader(void)
	{
	}

	int TcpReader::Read(int fd)
	{
		int len = Reader::Read(fd);
		if (len > 0)
		{
			while (total_ > 0)
			{
				switch(state_)
				{
				case Header:
					{
						if (total_ >= header_)
						{
							uint8 len[4];
							this->ReadData((char*)len,header_);
							if (header_ == 2)
								left_ = len[1] | len[0] << 8;
							else
								left_ = len[0] | len[1] << 8 | len[2] << 16 | len[3] << 24;
							assert(left_ > header_);
							left_ -= header_;
							state_ = Body;
							break;
						}
						else
							return 0;
					}
				case Body:
					{
						if (total_ >= left_)
						{
							char* data = (char*)malloc(left_);
							this->ReadData(data,left_);
							this->channel_->Forward(data,left_);
							left_ = 0;
							state_ = Header;
							break;
						}
						else
							return 0;
					}
				default:
					assert(false);
				}
			}
		}

		return len >= 0 ? len:-1;
	}
}

