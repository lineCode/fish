
#include "ZeroPack.h"


static int packSegment(uint8* src,uint8* buffer,int sz,int n)
{
	uint8 header = 0;
	int notzero = 0;
	uint8* obuffer = buffer;

	++buffer;
	--sz;

	if (sz < 0)
		obuffer = NULL;

	for (int i = 0;i < 8;i++)
	{
		if (src[i] != 0)
		{
			notzero++;
			header |= 1<<i;
			if (sz > 0)
			{
				*buffer = src[i];
				++buffer;
				--sz;
			}
		}
	}

	if ((notzero == 6 || notzero == 7) && n > 0)
		notzero = 8;

	if (notzero == 8)
	{
		if (n > 0)
			return 8;
		else
			return 10;
	}
	if (obuffer)
		*obuffer = header;

	return notzero+1;
}

static void makeFF(uint8* src,uint8* des,int n)
{
	int align8_n = (n+7)&(~7);

	des[0] = 0xff;
	des[1] = align8_n/8 - 1;
	memcpy(des+2, src, n);
	for(int i=0; i< align8_n-n; i++)
		des[n+2+i] = 0;
}

int Zeropack(void* srcv,int srcsz,void* bufferv,int bufsz)
{
	uint8 tmp[8];
	uint8* ffSrcStart = NULL;
	uint8* ffDesStart = NULL;
	int ffCount = 0;
	int size = 0;

	uint8* src = (uint8*)srcv;
	uint8* buffer = (uint8*)bufferv;

	for (int i = 0;i < srcsz;i+=8)
	{
		int padding = i+8 - srcsz;
		if (padding > 0)
		{
			memcpy(tmp, src, 8-padding);
			for (int j=0;j<padding;j++)
				tmp[7-j] = 0;
			src = tmp;
		}

		int n = packSegment(src,buffer,bufsz,ffCount);
		bufsz -= n;

		if (n == 10) 
		{
			ffSrcStart = src;
			ffDesStart = buffer;
			ffCount = 1;
		}
		else if (n == 8 && ffCount > 0)
		{
			++ffCount;
			if (ffCount == 256)
			{
				if (bufsz >= 0)
					makeFF(ffSrcStart,ffDesStart,256*8);
				ffCount = 0;
			}
		}
		else
		{
			if (ffCount > 0)
			{
				if (bufsz >= 0)
					makeFF(ffSrcStart,ffDesStart,ffCount*8);
				ffCount = 0;
			}
		}
		src += 8;
		buffer += n;
		size += n;
	}
	if(bufsz >= 0)
	{
		if(ffCount == 1)
			makeFF(ffSrcStart, ffDesStart, 8);
		else if (ffCount > 1)
			makeFF(ffSrcStart, ffDesStart, srcsz - (intptr_t)(ffSrcStart - (uint8*)srcv));
	}
	return size;
}

int Zerounpack(void * srcv, int srcsz, void* bufferv,int* buffsize)
{
	int bufsz = *buffsize;
	const uint8 * src = (const uint8*)srcv;
	uint8 * buffer = (uint8*)bufferv;
	int size = 0;
	while (srcsz > 0) 
	{
		uint8 header = src[0];
		--srcsz;
		++src;
		if (header == 0xff)
		{
			if (srcsz < 0)
				return -1;

			int n = (src[0] + 1) * 8;
			if (srcsz < n + 1)
				return -1;
			srcsz -= n + 1;
			++src;
			if (bufsz >= n)
				memcpy(buffer, src, n);

			bufsz -= n;
			buffer += n;
			src += n;
			size += n;
		} 
		else
		{
			for (int i=0;i<8;i++) 
			{
				int nz = (header >> i) & 1;
				if (nz)
				{
					if (srcsz < 0)
						return -1;
					if (bufsz > 0)
					{
						*buffer = *src;
						--bufsz;
						++buffer;
					}
					++src;
					--srcsz;
				} 
				else 
				{
					if (bufsz > 0) 
					{
						*buffer = 0;
						--bufsz;
						++buffer;
					}
				}
				++size;
			}
		}
	}
	return size;
}
