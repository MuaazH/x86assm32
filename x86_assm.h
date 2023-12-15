#ifndef X86_ASSM
#define X86_ASSM
#include <RedBlackBST.h>

namespace x86 {

	void printHex(unsigned char *pAssm, int len);

	void writeHexInt(char *txt, unsigned int i);

	int assm32(char *inst, unsigned char *pBuf, unsigned int address);

	int assm32Program(char *pPrg, unsigned char *pBuf, int maxSize, unsigned int address, RedBlackBST<char *, unsigned int> *pGlobalLabels);

	const char assm32version[] = "1.19.0";
}

#endif
