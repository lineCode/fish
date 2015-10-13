#include "TcpReader.h"
#include "Network.h"
#include "Session.h"

namespace Network
{
	TcpReader::TcpReader(Session* session,int header,int buffsize):Reader(session,buffsize)
	{
		_header = header;
		_left = 0;
		_state = Header;
	}


	TcpReader::~TcpReader(void)
	{
	}

	int TcpReader::Read(int fd)
	{
		int len = Reader::Read(fd);
		if (len > 0)
		{
			while (_total > 0)
			{
				switch(_state)
				{
				case Header:
					{
						if (_total >= _header)
						{
							_left = ReadHeader();
							_left -= _header;
							_state = Body;
							break;
						}
						else
							return 0;
					}
				case Body:
					{
						if (_total >= _left)
						{
							char* data = (char*)malloc(_left);
							this->ReadData(data,_left);
							this->_session->Forward(data,_left);
							_left = 0;
							_state = Header;
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

	int TcpReader::ReadHeader()
	{
		assert(_total >= _header);

		uint8 len[4];

		this->ReadData((char*)len,_header);

		int result = 0;
		if (_header == 2)
			result = len[1] | len[0] << 8;
		else
			result = len[0] | len[1] << 8 | len[2] << 16 | len[3] << 24;
		assert(result > _header);
		return result;
	}
}

