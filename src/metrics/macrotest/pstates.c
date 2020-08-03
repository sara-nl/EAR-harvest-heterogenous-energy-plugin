#include <stdio.h>
#include <unistd.h>
#include <metrics/common/msr.h>

ullong setbits(ullong reg, ullong val, ullong left_bit, ullong right_bit)
{
	ullong shift = (left_bit - right_bit);
	ullong mask  = (((0x01 << shift) - 1) << 1) + 1;
	return reg | ((val) << right_bit);
}

ullong getbits(ullong reg, int left_bit, int right_bit)
{
	uint shift = left_bit - right_bit;
	uint mask  = (((0x01 << shift) - 1) << 1) + 1;
	return ((reg >> right_bit) & mask);
}

void readbits(ullong *data, int i)
{
	ullong PstateEn = getbits(data[i], 63, 63);
	ullong CpuFid = getbits(data[i], 7, 0);
	ullong CpuDfsId = getbits(data[i], 13,  8);
	ullong CoreCOF = 0;		
	ullong CpuVid = getbits(data[i], 21, 14);
	ullong IddDiv = getbits(data[i], 31, 30);
	ullong IddValue = getbits(data[i], 29, 22);
	ullong CorePow = CpuVid * IddValue; 
	if (CpuDfsId > 0) CoreCOF = (CpuFid * 200LLU) / CpuDfsId;
	double VcoDiv = ((double) CpuDfsId) / 8.0; 

	printf("%d, En %llu, IddDiv: %llu, IddValue: %llu, CpuVid: %llu (CorePow: %llu W), CpuFid: %llu * 200 / CpuDfsId: %llu = CoreCOF: %llu, VCO/(%llu/8) = VCO/%0.3lf\n", i,
		PstateEn,
		IddDiv,
		IddValue,
		CpuVid, CorePow,
		CpuFid, CpuDfsId, CoreCOF,
		CpuDfsId, VcoDiv
	);
}

int main(int argc, char *argv[])
{
	ullong data[8];
	state_t s;
	int i;

	msr_open(0);

	s = msr_read(0, &data[0], sizeof(ullong), 0xC0010061);
	printf("P-state Current Limit (%d), PstateMaxVal: %llu, CurPstateLimit: %llu\n", s, getbits(data[0], 6, 4), getbits(data[0], 2, 0));
	s = msr_read(0, &data[0], sizeof(ullong), 0xC0010062);
	printf("P-state Control (%d), PstateCmd: %llu\n", s, getbits(data[0], 2, 0));
	s = msr_read(0, &data[0], sizeof(ullong), 0xC0010063);
	printf("P-state Status (%d), CurPstate: %llu\n", s, data[0]);
	s = msr_read(0, &data[0], sizeof(ullong), 0xC0010070);
	printf("COFVID Control%llu (%d, %s)\n", data[0], s, state_msg);
	s = msr_read(0, &data[0], sizeof(ullong), 0xC0010071);
	printf("COFVID Status %llu (%d, %s)\n", data[0], s, state_msg);

	msr_read(0, &data[0], sizeof(ullong), 0xC0010064);
	msr_read(0, &data[1], sizeof(ullong), 0xC0010065);
	msr_read(0, &data[2], sizeof(ullong), 0xC0010066);
	msr_read(0, &data[3], sizeof(ullong), 0xC0010067);
	msr_read(0, &data[4], sizeof(ullong), 0xC0010068);
	msr_read(0, &data[5], sizeof(ullong), 0xC0010069);
	msr_read(0, &data[6], sizeof(ullong), 0xC001006A);
	msr_read(0, &data[7], sizeof(ullong), 0xC001006B);

	for (i = 0; i < 8; ++i)
	{
		readbits(data, i);
	}
#if 0
	data[0] = 0LLU;
	data[0] = setbits(data[0],  1LLU, 63LLU, 63LLU);
	data[0] = setbits(data[0],  1LLU, 31LLU, 30LLU);
	data[0] = setbits(data[0], 14LLU, 29LLU, 22LLU);
	data[0] = setbits(data[0],104LLU, 21LLU, 14LLU);
	data[0] = setbits(data[0], 12LLU, 13LLU,  8LLU);
	data[0] = setbits(data[0], 84LLU,  7LLU,  0LLU);
	readbits(data, 0);

	data[0] = 0LU;
	data[1] = 0LU;

	for (i = 0; i < 128; ++i)
	{
		msr_open(i);
		s = msr_write(i, &data[0], sizeof(ullong), 0xC0010067);
		printf("%d ", s);
		s = msr_write(i, &data[1], sizeof(ullong), 0xC0010062);
	}
	printf("\n");
	for (i = 0; i < 128; ++i)
	{
		msr_read(i, &data[0], sizeof(ullong), 0xC0010067);
		readbits(data, 0);
	}
#endif
	return 0;
}
