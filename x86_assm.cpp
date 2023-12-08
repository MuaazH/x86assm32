#include "x86_assm.h"
#include <ArrayDeque.h>
#include <stdio.h>

#include <iostream>

using namespace std;
using namespace x86;

#define MAX_INST_LEN	128
#define MAX_X86_LEN		16

#include "x86_assm_internal.h"

inline void copyMem(char *dst, char *src, unsigned int sz) {
	for (unsigned int i = 0; i < sz; i++) {
		dst[i] = src[i];
	}
}

inline int labelLen(char *pInst) {
	int len = nextTokenLen(pInst);
	if (pInst[len - 1] == ':') {
		return len;
	}
	return -1;
}

int getLabel(char *pInst, char *pOut) {
	int start = -1;
	for (unsigned int i = 0; ; i++) {
		if (!pInst[i]) break;
		if (pInst[i] == '%') {
			if (start < 0) {
				start = i + 1;
			} else {
				if (pOut) {
					unsigned int labelLen = i - start;
					copyMem(pOut, pInst + start, labelLen);
					pOut[labelLen] = 0;
				}
				return start - 1;
			}
		}
	}
	return -1;
}

void writeHexInt_(char *pOut, unsigned int val) {
	const char *hex = "0123456789ABCDEF";
	int j = 0;
	for (int u = 24; u >= 0; u -= 8) {
		unsigned int b = (val >> u) & 0x00FF;
		pOut[j++] = hex[(b >> 4) & 0x0F];
		pOut[j++] = hex[b & 0x0F];
	}
}

void replaceLabel(char *txt, char *label, int offset, unsigned int val, char *pOut) {
	int j = 0;
	int i = 0;
	// copy the text before the label
	for (; i < offset; i++) {
		pOut[i] = txt[i];
	}
	j = i;
	i++;
	// write the number in hex
	writeHexInt_(pOut + j, val);
	j += 8;

	// copy the text after the label
	while (txt[i] != '%') i++;
	i++;
	for (; txt[i]; i++) {
		pOut[j++] = txt[i];
	}
	pOut[j] = 0;
}

struct InstructionLine {
	char *line;
	char *inst;
	char *label;
	unsigned char machineCode[MAX_X86_LEN];
	int mcLen;
	unsigned int address;

	InstructionLine() {
		line = 0;
		inst = 0;
		label = 0;
		mcLen = 0;
		address = 0;
	}

	~InstructionLine() {
		if (line) {
			delete [] line;
		}
	}
};

bool splitLines(char *pPrg, ArrayDeque<InstructionLine *> *pLines, RedBlackBST<char *, InstructionLine *> *pLabels, RedBlackBST<char *, unsigned int> *pGlobalLabels) {
	int i = 0;
	int start = 0;
	int len = 0;
	int spaces = 0;
	bool comment = false;
	for (; ; i++) {
		if (!pPrg[i] || pPrg[i] == '\r' || pPrg[i] == '\n' || pPrg[i] == ';') {
			if (len && !comment) {
				InstructionLine *pInst = new InstructionLine();
				pLines->Add(pInst);

				char *pLine = new char[len + 1];
				pInst->line = pLine;

				copyMem(pLine, pPrg + start, len);
				pLine[len] = 0;

				int l = labelLen(pLine);
				if (l > 0) {
					l--;
					pLine[l] = 0;
					pInst->inst = pLine + l + 1;
					while (pInst->inst[0] && pInst->inst[0] <= ' ') pInst->inst++;
					pInst->label = pLine;
					if (pLabels->Contains(pInst->label) || (pGlobalLabels && pGlobalLabels->Contains(pInst->label)) ) {
						cerr << "Assmebler: Error, Label Redifined '" << pInst->label << "'" << endl;
						return false;
					}
					pLabels->Put(pInst->label, pInst);
				} else {
					pInst->inst = pLine;
				}
			}
			if (!pPrg[i]) {
				break;
			}
			if (pPrg[i] == ';') {
				comment = true;
			} else {
				comment = false;
			}
			len = 0;
		}
		if (comment) continue;
		if (!len) {
			if (pPrg[i] > ' ') {
				len = 1;
				start = i;
				spaces = 0;
			}
		} else {
			if (pPrg[i] <= ' ') {
				spaces++;
			} else {
				len++;
				len += spaces;
				spaces = 0;
			}
		}
	}
	return true;
}

namespace x86 {

	void printHex(unsigned char *pAssm, int len) {
		const char *hex = "0123456789ABCDEF";
		for (int i = 0; i < len; i++) {
			unsigned int b = pAssm[i];
			if (b < 0x10) cout << '0' << hex[b] << ' ';
			else cout << hex[(b & 0xF0) >> 4] << hex[b & 0x0F] << ' ';
		}
		cout << endl;
	}

	void writeHexInt(char *txt, unsigned int i) {
		writeHexInt_(txt, i);
	}

	int assm32Program(char *pPrg, unsigned char *pBuf, int maxSize, unsigned int address, RedBlackBST<char *, unsigned int> *pGlobalLabels) {
		ArrayDeque<InstructionLine *> lines;
		RedBlackBST<char *, InstructionLine *> labels(str_comp); // does not need a cleaner, uses pointers from other objects only

		int ret = 0;

		if (!splitLines(pPrg, &lines, &labels, pGlobalLabels)) {
			ret = -1;
		}

		unsigned int linesCount = lines.Size();
		if (!linesCount) {
			return 0; // no lines, what the shit?
		}

		if (!ret) {
			unsigned int lastAddress = address;
			lines.At(0)->address = lastAddress;

			// pass 1: assm what you can, and get all labels
			for (unsigned int i = 0; i < linesCount; i++) {
				InstructionLine *pLine = lines.At(i);
				pLine->address = lastAddress;
				// if it contains a label, skip it.
				if (getLabel(pLine->inst, 0) < 0) {
					pLine->mcLen = assm32_(pLine->inst, pLine->machineCode, pLine->address);
					if (pLine->mcLen > 0) {
						lastAddress  += pLine->mcLen;
					}
				}
			}

			char labelBuf[MAX_INST_LEN];
			char instBuf[MAX_INST_LEN * 2];

			bool changesDetected = true;
			while (changesDetected) {
				changesDetected = false;
				// pass 2: check if anything failed and why & fix that shit
				lastAddress = lines.At(0)->address;
				for (unsigned int i = 0; i < linesCount; i++) {
					InstructionLine *pLine = lines.At(i);
					if (pLine->address != lastAddress) {
						changesDetected = true;
						pLine->address = lastAddress;
					}
					int labelOff = getLabel(pLine->inst, labelBuf);
					if (labelOff > -1) {
						unsigned int labelAddress = pGlobalLabels->Get(labelBuf);
						if (!labelAddress) {
							InstructionLine *pTarget = labels.Get(labelBuf);
							if (pTarget) {
								labelAddress = pTarget->address;
							}
						}
						if (labelAddress > 0) {
							replaceLabel(pLine->inst, labelBuf, labelOff, labelAddress, instBuf);
							pLine->mcLen = assm32_(instBuf, pLine->machineCode, pLine->address);
							if (pLine->mcLen <= 0) {
								// bad code
								cerr << "Assmebler: Error '" << pLine->line << "'" << endl;
								ret = -1;
								break;
							}
						} else {
							// used label not found
							cerr << "Assmebler: Label Not Found '" << pLine->line << "'" << endl;
							ret = -2;
							break;
						}
					} else {
						if (pLine->mcLen <= 0) {
							// bad code
							cerr << "Assmebler: Error '" << pLine->line << "'" << endl;
							ret = -1;
							break;
						}
						pLine->mcLen = assm32_(pLine->inst, pLine->machineCode, pLine->address);
					}
					// printHex(pLine->machineCode, pLine->mcLen);
					lastAddress  += pLine->mcLen;
				}
			}

			if (!ret) {
				// pass 3: write shit to the shit.
				for (unsigned int i = 0; i < linesCount; i++) {
					InstructionLine *pLine = lines.At(i);
					if (ret + pLine->mcLen > maxSize) {
						cerr << "Assmebler: Out Of Memory" << endl;
						return -3; // out of memory
					}
					copyMem((char *) (pBuf + ret), (char *) pLine->machineCode, pLine->mcLen);
					ret += pLine->mcLen;
				}
			}
		}
		// pass x: free memory
		for (unsigned int i = 0; i < linesCount; i++) {
			InstructionLine *pLine = lines.At(i);
			if (pLine->label && pGlobalLabels) {
				int gLabelLength = str_length(pLine->label);
				char *pGLabelBuf = new char[gLabelLength + 1];
				copyMem(pGLabelBuf, pLine->label, gLabelLength);
				pGLabelBuf[gLabelLength] = 0;
				pGlobalLabels->Put(pGLabelBuf, pLine->address);
			}
			delete pLine;
		}
		return ret;
	}

	/* returns instruction size, or -1 for error */
	int assm32(const char *inst, unsigned char *pBuf, unsigned int address) {
		return assm32_(inst, pBuf, address);
	}

}

#ifdef CONF_DEBUG_X86ASSM

int main(int argc, char *argv[]) {

	/*char *test[1024] = {
						"				movsx eax,word ptr [ecx+60]		\n"
						"				cmp eax, 01						\n"
						"				je %isPlyr%						\n"
						"				movsx eax, word ptr [ecx+5C]	\n"
						"				cmp eax, 01						\n"
						"				jg %enemyAlive%					\n"
						"				ret								\n"
						"enemyAlive:	mov eax, 00000001				\n"
						"				ret								\n"
						"isPlyr:		movsx eax, word ptr [ecx+5C]	\n"
						"				ret								\n";
	};*/

	char test[100] = {"sub eax, 00000FEC"};

	const unsigned int assmSize = 1024 * 2; // 2k
	unsigned char assm[assmSize];
	for (unsigned int i = 0; i < assmSize; i++) assm[i] = 0;

	int sz = assm32Program(test, assm, assmSize, 0x00400000, 0);
	if (sz > 0) {
		cout << sz << endl;
	} else {
		cout << "\n\nERROR" << endl;
	}

	if (sz > 0)
		printHex(assm, sz);

	cout << "====== END ====== " << endl;

	return 0;
}

#endif
