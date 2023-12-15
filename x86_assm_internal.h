// operands types
#define REG8	0x100
#define	REG16	0x200
#define	REG32	0x300

#define REG8_AL		0x101
#define REG8_CL		0x102
#define REG8_DL		0x103
#define REG8_BL		0x104
#define REG8_AH		0x105
#define REG8_CH		0x106
#define REG8_DH		0x107
#define REG8_BH		0x108

#define REG16_AX	0x201
#define REG16_CX	0x202
#define REG16_DX	0x203
#define REG16_BX	0x204
#define REG16_SP	0x205
#define REG16_BP	0x206
#define REG16_SI	0x207
#define REG16_DI	0x208

#define REG32_EAX	0x301
#define REG32_ECX	0x302
#define REG32_EDX	0x303
#define REG32_EBX	0x304
#define REG32_ESP	0x305
#define REG32_EBP	0x306
#define REG32_ESI	0x307
#define REG32_EDI	0x308

#define MEM8	0x400
#define MEM16	0x500
#define MEM32	0x600

#define IMM8	0x700
#define IMM16	0x800
#define IMM32	0x900

// 8bit registers
#define AL	0
#define CL	1
#define DL	2
#define BL	3
#define AH	4
#define CH	5
#define DH	6
#define BH	7
// 16bit registers
#define AX	0
#define CX	1
#define DX	2
#define BX	3
#define SP	4
#define BP	5
#define SI	6
#define DI	7
// 32bit registers
#define EAX	0
#define ECX	1
#define EDX	2
#define EBX	3
#define ESP	4
#define EBP	5
#define ESI	6
#define EDI	7
// MOD
#define MOD_INDIRECT	0
#define	MOD_ONE_BYTE	1
#define MOD_FOUR_BYES	2
#define MOD_REG			3

// information flags
#define FLAG_MODRM					1		// has a modrm byte?
#define FLAG_0F_PREFIX				2		// has a 0F perfix?
#define FLAG_2ND_OPCODE				4		// has a 2nd opcode?
#define FLAG_REVERSE_OPERANDS		8		// reverse the order of the operands in the modrm byte?
#define FLAG_MODRM_OPCODE			16		// has an opcode extention in the modrm byte?
#define FLAG_1ST_OPERAND_ONLY		32		// encode 1st operand as is and drop the 2nd? (ex: mov shr ah, cl)
#define FLAG_1ST_IMM_OPERAND_ONLY	64		// encode imm in the second operand only? (ex: mov al, 01)
#define FLAG_2ND_IMM_OPERAND_ONLY	128		// encode imm in the second operand only? (ex: mov al, 01)
#define FLAG_SINGLE_BYTE			256		// single byte opcode? no need for parameters?

// mem types
#define MEM_TYPE_OFF			1
#define MEM_TYPE_REG			2
#define MEM_TYPE_OFF_REG		3
#define MEM_TYPE_OFF_SI			4
#define MEM_TYPE_SIB			5
#define MEM_TYPE_OFF_SIB		6

struct Instruction {
	// opcode
	unsigned char prefix;
	unsigned char opcode1;
	unsigned char opcode2;
	unsigned char opcodeExt;
	// operands
	unsigned int opCount;
	unsigned int op1;
	unsigned int op2;
	unsigned int op3;
	unsigned int op4;
	// other info
	char mnemonic[16];
 	unsigned short flags;
};

struct Operand {
	unsigned int type;
	union {
		struct {
			unsigned int type;
			unsigned int scale; // [0, 3]
			unsigned int index;
			unsigned int base;
			int offsetSize; // 8 or 32
			union {
				char offset8;
				int  offset32;
			};
		} mem;
		union {
			// 8
			char val8;
			unsigned char uVal8;
			// 16
			int val16;
			unsigned int uVal16;
			// 32
			int val32;
			unsigned int uVal32;
			// 64
			long long val64;
			unsigned long long uVal64;
		};
	};
};

struct JumpInstruction {
	unsigned char opcode;
	char mnemonic[8];
};

#include "x86_32bit_instructions.h"

// util

const char *operandToStr(Operand *pOperand) {
	if (pOperand->type == REG8) {
		if (pOperand->val32 == AL) return "<REG8_AL>";
		if (pOperand->val32 == BL) return "<REG8_BL>";
		if (pOperand->val32 == CL) return "<REG8_CL>";
		if (pOperand->val32 == DL) return "<REG8_DL>";
		if (pOperand->val32 == AH) return "<REG8_AH>";
		if (pOperand->val32 == BH) return "<REG8_BH>";
		if (pOperand->val32 == CH) return "<REG8_CH>";
		if (pOperand->val32 == DH) return "<REG8_DH>";
		return "<REG8_?>"; // should never happen
	}
	if (pOperand->type == REG16) {
		if (pOperand->val32 == AX) return "<REG16_AX>";
		if (pOperand->val32 == BX) return "<REG16_BX>";
		if (pOperand->val32 == CX) return "<REG16_CX>";
		if (pOperand->val32 == DX) return "<REG16_DX>";
		if (pOperand->val32 == SP) return "<REG16_SP>";
		if (pOperand->val32 == BP) return "<REG16_BP>";
		if (pOperand->val32 == SI) return "<REG16_SI>";
		if (pOperand->val32 == DI) return "<REG16_DI>";
		return "<REG16_?>"; // should never happen
	}
	if (pOperand->type == REG32) {
		if (pOperand->val32 == EAX) return "<REG32_EAX>";
		if (pOperand->val32 == ECX) return "<REG32_ECX>";
		if (pOperand->val32 == EDX) return "<REG32_EDX>";
		if (pOperand->val32 == EBX) return "<REG32_EBX>";
		if (pOperand->val32 == ESP) return "<REG32_ESP>";
		if (pOperand->val32 == EBP) return "<REG32_EBP>";
		if (pOperand->val32 == ESI) return "<REG32_ESI>";
		if (pOperand->val32 == EDI) return "<REG32_EDI>";
		return "<REG32_?>"; // should never happen
	}
	if (pOperand->type == IMM8) return "<IMM8>";
	if (pOperand->type == IMM16) return "<IMM16>";
	if (pOperand->type == IMM32) return "<IMM32>";
	if (pOperand->type == MEM8) return "<MEM8>";
	if (pOperand->type == MEM16) return "<MEM16>";
	if (pOperand->type == MEM32) return "<MEM32>";
	return "<?>";
}

inline int log2(int x) {
	int l = 0;
	while (x > 1) {
		l++;
		x >>= 1;
	}
	return l;
}

// str utils

int str_comp(char *a, char *b) {
	for (int i = 0; ; i++) {
		if (a[i] < b[i]) return -1;
		if (b[i] < a[i]) return 1;
		if (!a[i]) break;
	}
	return 0;
}

inline bool str_equal(const char *a, const char *b) {
	int i = 0;
	while (true) {
		if (a[i] != b[i]) return false;
		if (!a[i]) break;
		i++;
	}
	return true;
}

static inline unsigned int str_length(const char *pStr) {
	unsigned int i = 0;
	while (pStr[i]) i++;
	return i;
}

inline bool startsWith(const char *pTxt, const char *pPrefix) {
	for (int i = 0; ; i++) {
		if (!pPrefix[i]) return true;
		char c = pTxt[i];
		if ('A' <= c && c <= 'Z') c |= 0x20;
		if (c != pPrefix[i]) return false;
		if (!pTxt[i]) return true;
	}
	return true;
}

inline void removeSpaces(char *pSrc, char *pDst) {
	int i = 0;
	int j = 0;
	bool spaceFound = false;
	char lastChar = 0;
	while (true) {
		if (!pSrc[i]) {
			pDst[j] = 0;
			break;
		}
		if (pSrc[i] > ' ') {
			if (spaceFound) {
				if (j > 0 && pSrc[i] != ']' && lastChar != '[' && (pSrc[i] < '*' || '/' < pSrc[i]) && (lastChar < '*' || '/' < lastChar)) {
					pDst[j++] = ' ';
				}
				spaceFound = false;
			}
			lastChar = pSrc[i];
			pDst[j++] = pSrc[i];
		} else {
			spaceFound = true;
		}
		i++;
	}
}

inline int nextTokenLen(char *inst) {
	int size = 0;
	while (inst[size] && inst[size] > ' ') {
		size++;
	}
	return size;
}

inline char *trimLeft(char *pTxt) {
	char *pRes = pTxt;
	while (pRes[0] && (pRes[0] <= ' ' || pRes[0] >= 127)) {
		pRes++;
	}
	return pRes;
}

inline bool parseHex(char *pTxt, unsigned int len, int *pType, void *pOut) {
	if (len != 2 && len != 4 && len != 8) return false;
	int val = 0;
	for (unsigned int i = 0; i < len; i++) {
		char c = pTxt[i];
		if ('0' <= c && c <= '9') {
			c -= '0';
		} else if ('A' <= c && c <= 'F') {
			c -= 'A' - 10;
		} else if ('a' <= c && c <= 'f') {
			c -= 'a' - 10;
		} else {
			return false;
		}
		val *= 16;
		val += c;
	}

	if (len == 2) {
		*((char *) pOut) = (char) val;
		*pType = IMM8;
	} else if (len == 4) {
		*((short *) pOut) = (short) val;
		*pType = IMM16;
	} else {
		*((int *) pOut) = val;
		*pType = IMM32;
	}

	return true;
}

inline bool parseReg(char *pTxt, unsigned int len, int *pType, int *pReg) {
	if (len < 2) return 0;
#define ifc2(i, l, u) if (pTxt[i] == l || pTxt[i] == u)
	// 8 & 16 bit registers
	if (len == 2) {
		ifc2(1, 'l', 'L') {
			ifc2(0, 'a', 'A') { *pReg = AL; *pType = REG8; return true; }
			ifc2(0, 'b', 'B') { *pReg = BL; *pType = REG8; return true; }
			ifc2(0, 'c', 'C') { *pReg = CL; *pType = REG8; return true; }
			ifc2(0, 'd', 'D') { *pReg = DL; *pType = REG8; return true; }
		}
		ifc2(1, 'h', 'H') {
			ifc2(0, 'a', 'A') { *pReg = AH; *pType = REG8; return true; }
			ifc2(0, 'b', 'B') { *pReg = BH; *pType = REG8; return true; }
			ifc2(0, 'c', 'C') { *pReg = CH; *pType = REG8; return true; }
			ifc2(0, 'd', 'D') { *pReg = DH; *pType = REG8; return true; }
		}
		ifc2(1, 'x', 'X') {
			ifc2(0, 'a', 'A') { *pReg = AX; *pType = REG16; return true; }
			ifc2(0, 'b', 'B') { *pReg = BX; *pType = REG16; return true; }
			ifc2(0, 'c', 'C') { *pReg = CX; *pType = REG16; return true; }
			ifc2(0, 'd', 'D') { *pReg = DX; *pType = REG16; return true; }
		}
		ifc2(1, 'p', 'P') {
			ifc2(0, 's', 'S') { *pReg = SP; *pType = REG16; return true; }
			ifc2(0, 'b', 'B') { *pReg = BP; *pType = REG16; return true; }
		}
		ifc2(1, 'i', 'I') {
			ifc2(0, 's', 'S') { *pReg = SI; *pType = REG16; return true; }
			ifc2(0, 'd', 'D') { *pReg = DI; *pType = REG16; return true; }
		}
		return false;
	}
	if (len == 3) {
		ifc2(0, 'e', 'E') {
			ifc2(2, 'x', 'X') {
				ifc2(1, 'a', 'A') { *pReg = EAX; *pType = REG32; return true; }
				ifc2(1, 'b', 'B') { *pReg = EBX; *pType = REG32; return true; }
				ifc2(1, 'c', 'C') { *pReg = ECX; *pType = REG32; return true; }
				ifc2(1, 'd', 'D') { *pReg = EDX; *pType = REG32; return true; }
			}
			ifc2(2, 'p', 'P') {
				ifc2(1, 's', 'S') { *pReg = ESP; *pType = REG32; return true; }
				ifc2(1, 'b', 'B') { *pReg = EBP; *pType = REG32; return true; }
			}
			ifc2(2, 'i', 'I') {
				ifc2(1, 's', 'S') { *pReg = ESI; *pType = REG32; return true; }
				ifc2(1, 'd', 'D') { *pReg = EDI; *pType = REG32; return true; }
			}
		}
		return false;
	}
	return false;
}

// assm util

bool parseOperand(char *pBuf, Operand *pOut) {
	unsigned int bufLen = str_length(pBuf);
	pOut->type = 0;
	// try reg
	int regType;
	int regVal;
	if (parseReg(pBuf, bufLen, &regType, &regVal)) {
		pOut->type = regType;
		pOut->val32 = regVal;
		return true;
	}
	// try imm
	int immType;
	int immVal;
	if (parseHex(pBuf, bufLen, &immType, (void *) &immVal)) {
		pOut->type = immType;
		if (immType == IMM8) {
			pOut->val8 = *((char *) &immVal);
		} else if (immType == IMM16) {
			pOut->val16 = *((short *) &immVal);
		} else {
			pOut->val32 = immVal;
		}
		return true;
	}
	// try mem
	if (pBuf[bufLen - 1] == ']') {
		int type = 0;
		char *pMemStr = pBuf;
		int memSLen = bufLen - 1;
		pBuf[memSLen] = 0;
		if (startsWith(pMemStr, "[")) {
			type = MEM32;
			pMemStr++;
			memSLen--;
		} else if (startsWith(pMemStr, "word ptr [")) {
			type = MEM16;
			pMemStr += 10;
			memSLen -= 10;
		} else if (startsWith(pMemStr, "byte ptr [")) {
			type = MEM8;
			pMemStr += 10;
			memSLen -= 10;
		} else if (startsWith(pMemStr, "dword ptr [")) {
			type = MEM32;
			pMemStr += 11;
			memSLen -= 11;
		}
		if (type && memSLen > 0) {
			char base[MAX_INST_LEN];
			char index[MAX_INST_LEN];
			char offset[MAX_INST_LEN];
			char scale;
			int offsetType;
			int offsetVal;
			int baseType;
			int baseVal;
			int indexType;
			int indexVal;
			#define IF_OFFSET_IS_HEX		if (parseHex((char *) &offset, offsetLen, &offsetType, &offsetVal))
			#define IF_BASE_IS_REG		if (parseReg((char *) &base, baseLen, &baseType, &baseVal))
			#define IF_INDEX_IS_REG		if (parseReg((char *) &index, indexLen, &indexType, &indexVal))

			// try [offset]
			if (parseHex(pMemStr, memSLen, &offsetType, &offsetVal)) {
				pOut->type = type;
				pOut->mem.type = MEM_TYPE_OFF;
				pOut->mem.offsetSize = 32;
				if (offsetType == IMM8) {
					pOut->mem.offset32 = *((char *) &offsetVal);
				} else if (offsetType == IMM32) {
					pOut->mem.offset32 = offsetVal;
				} else if (offsetType == IMM16) {
					pOut->mem.offset32 = *((short *) &offsetVal);
				} else {
					return false;
				}
				return true;
			}
			// try [reg]
			if (parseReg(pMemStr, memSLen, &baseType, &baseVal)) {
				pOut->type = type;
				pOut->mem.type = MEM_TYPE_REG;
				pOut->mem.base = baseVal;
				return true;
			}
			// try [offset+base+index*scale]
			if(sscanf(pMemStr, "%[^+]+%[^*]*%c+%[^+]", base, index, &scale, offset) == 4) {
				int baseLen = str_length((char *) &base);
				int indexLen = str_length((char *) &index);
				int offsetLen = str_length((char *) &offset);
				if (baseLen + indexLen + offsetLen + 4 == memSLen) {
					IF_OFFSET_IS_HEX {
						IF_BASE_IS_REG {
							if (baseType == REG32) {
								IF_INDEX_IS_REG {
									if (indexType == REG32) {
										scale -= '0';
										if (scale == 1 || scale == 2 || scale == 4 || scale == 8) {
											pOut->type = type;
											pOut->mem.type = MEM_TYPE_OFF_SIB;
											pOut->mem.scale = log2(scale);
											pOut->mem.index = indexVal;
											pOut->mem.base = baseVal;
											if (offsetType == IMM8) {
												pOut->mem.offsetSize = 8;
												pOut->mem.offset8 = *((char *) &offsetVal);
											} else if (offsetType == IMM32) {
												pOut->mem.offsetSize = 32;
												pOut->mem.offset32 = offsetVal;
											} else if (offsetType == IMM16) {
												pOut->mem.offsetSize = 32;
												pOut->mem.offset32 = *((short *) &offsetVal);
											} else {
												return false;
											}
											return true;
										}
									}
								}
							}
						}
					}
				}
				return false;
			}
			// try [base+index*scale]
			if(sscanf(pMemStr, "%[^+]+%[^*]*%c", base, index, &scale) == 3) {
				int baseLen = str_length((char *) &base);
				int indexLen = str_length((char *) &index);
				if (baseLen + indexLen + 3 == memSLen) {
					IF_BASE_IS_REG {
						if (baseType == REG32) {
							IF_INDEX_IS_REG {
								if (indexType == REG32) {
									scale -= '0';
									if (scale == 1 || scale == 2 || scale == 4 || scale == 8) {
										pOut->type = type;
										pOut->mem.type = MEM_TYPE_SIB;
										pOut->mem.scale = log2(scale);
										pOut->mem.index = indexVal;
										pOut->mem.base = baseVal;
										return true;
									}
								}
							}
						}
					}
				}
				// return false; // matches the next case
			}
			// try [offset+index*scale]
			if(sscanf(pMemStr, "%[^+]+%[^*]*%c", offset, index, &scale) == 3) {
				int indexLen = str_length((char *) &index);
				int offsetLen = str_length((char *) &offset);
				if (indexLen + offsetLen + 3 == memSLen) {
					IF_OFFSET_IS_HEX {
						IF_INDEX_IS_REG {
							if (indexType == REG32) {
								scale -= '0';
								if (scale == 1 || scale == 2 || scale == 4 || scale == 8) {
									pOut->type = type;
									pOut->mem.type = MEM_TYPE_OFF_SI;
									pOut->mem.scale = log2(scale);
									pOut->mem.index = indexVal;
									pOut->mem.base = baseVal;
									if (offsetType == IMM8) {
										pOut->mem.offsetSize = 8;
										pOut->mem.offset8 = *((char *) &offsetVal);
									} else if (offsetType == IMM32) {
										pOut->mem.offsetSize = 32;
										pOut->mem.offset32 = offsetVal;
									} else if (offsetType == IMM16) {
										pOut->mem.offsetSize = 32;
										pOut->mem.offset32 = *((short *) &offsetVal);
									} else {
										return false;
									}
									return true;
								}
							}
						}
					}
				}
				return false;
			}
			// try [base+offset]
			if(sscanf(pMemStr, "%[^+]+%[^\n]", base, offset) == 2) {
				int baseLen = str_length((char *) &base);
				int offsetLen = str_length((char *) &offset);
				if (baseLen + offsetLen + 1 == memSLen) {
					IF_OFFSET_IS_HEX {
						IF_BASE_IS_REG {
							if (baseType == REG32) {
								pOut->type = type;
								pOut->mem.type = MEM_TYPE_OFF_REG;
								pOut->mem.base = baseVal;
								if (offsetType == IMM8) {
									pOut->mem.offsetSize = 8;
									pOut->mem.offset8 = *((char *) &offsetVal);
								} else if (offsetType == IMM32) {
									pOut->mem.offsetSize = 32;
									pOut->mem.offset32 = offsetVal;
								} else if (offsetType == IMM16) {
									pOut->mem.offsetSize = 32;
									pOut->mem.offset32 = *((short *) &offsetVal);
								} else {
									return false;
								}
								return true;
							}
						}
					}
				}
				return false;
			}
		}
	}
	return false;
}

bool parse2Operands(const char *pTxt, Operand *pDst, Operand *pSrc) {
	char op1Str[MAX_INST_LEN];
	char op2Str[MAX_INST_LEN];

	int s = sscanf(pTxt, "%[^,], %[^,]", op1Str, op2Str);

	if (s == 2) {
		int l = str_length(pTxt);
		int l1 = str_length((char *) &op1Str);
		int l2 = str_length((char *) &op2Str);
		for(int i = l1 + l2; i < l; i++) {
			if (pTxt[i] == ',') {
				return false;
			}
		}
		if (parseOperand((char *) &op1Str, pDst)) {
			if (parseOperand((char *) &op2Str, pSrc)) {
				return true;
			}
		}
	}
	return false;
}

bool parse3Operands(const char *pTxt, Operand *pOp1, Operand *pOp2, Operand *pOp3) {
	char op1Str[MAX_INST_LEN];
	char op2Str[MAX_INST_LEN];
	char op3Str[MAX_INST_LEN];

	int s = sscanf(pTxt, "%[^,], %[^,], %[^,]", op1Str, op2Str, op3Str);

	if (s == 3) {
		int l = str_length(pTxt);
		int l1 = str_length((char *) &op1Str);
		int l2 = str_length((char *) &op2Str);
		int l3 = str_length((char *) &op3Str);
		// no commas must exist after these params
		for(int i = l1 + l2 + l3; i < l; i++) {
			if (pTxt[i] == ',') {
				return false;
			}
		}
		if (parseOperand((char *) &op1Str, pOp1)) {
			if (parseOperand((char *) &op2Str, pOp2)) {
				if (parseOperand((char *) &op3Str, pOp3)) {
					return true;
				}
			}
		}
	}
	return false;
}

inline bool isOperandOfType(Operand *pOperand, unsigned int type) {
	if (type != pOperand->type) {
		if (pOperand->type == REG8 || pOperand->type == REG16 || pOperand->type == REG32) {
			if (((pOperand->type | pOperand->val32) + 1) == type) {
				return true;
			}
		}
		return false;
	}
	return true;
}

int assm32FindMatch(const Instruction instArr[], unsigned int count, unsigned int opCount, Operand *pOp1 = 0, Operand *pOp2 = 0, Operand *pOp3 = 0, Operand *pOp4 = 0) {
	for (unsigned int i = 0; i < count; i++) {
		Instruction inst = instArr[i];
		if (inst.opCount != opCount) {
			continue;
		}
		if (!opCount) return i;
		
		if (!isOperandOfType(pOp1, inst.op1)) continue;
		if (opCount > 1 && !isOperandOfType(pOp2, inst.op2)) continue;
		if (opCount > 2 && !isOperandOfType(pOp3, inst.op3)) continue;
		if (opCount > 3 && !isOperandOfType(pOp4, inst.op4)) continue;

		return i;
	}
	return -1;
}


inline int encodeStaticOperand(Operand *pOperand, unsigned char *pBuf) {
	if (pOperand->type == IMM8) {
		*pBuf = pOperand->val8;
		return 1;
	}
	if (pOperand->type == IMM16) {
		*((short *)pBuf) = pOperand->val16;
		return 2;
	}
	if (pOperand->type == IMM32) {
		*((int *)pBuf) = pOperand->val32;
		return 4;
	}
	return -1;
}

int encodeOpcodeExtModrmSib(unsigned char *pBuf, int opcodeExt, Operand *pOperand1, Operand *pImmOp = 0) {
	// modrm
	unsigned int mod;
	unsigned int rm;
	// sib
	unsigned int scale;
	unsigned int index;
	unsigned int base;
	// other
	bool sibNeeded = false;
	int offsetSize = 0; // 0, 8, 32
	int offsetVal;
	switch (pOperand1->type) {
		case REG8:
		case REG16:
		case REG32:
		{
			mod = MOD_REG;
			rm = pOperand1->val32;
			break;
		}
		case MEM8:
		case MEM16:
		case MEM32:
		{
			switch (pOperand1->mem.type) {
				case MEM_TYPE_OFF: {
					mod = 0;
					rm = EBP; // forbid the usage of ebp reg addressing
					offsetSize = 32;
					if (pOperand1->mem.offsetSize == 32) {
						offsetVal = pOperand1->mem.offset32;
					} else if (pOperand1->mem.offsetSize == 8) {
						offsetVal = pOperand1->mem.offset8;
					} else {
						return -1;
					}
					break;
				}
				case MEM_TYPE_REG: {
					rm = pOperand1->mem.base;
					if (rm == EBP) {
						mod = MOD_ONE_BYTE;
						offsetSize = 8;
						offsetVal = 0;
					} else {
						mod = MOD_INDIRECT;
						if (rm == ESP) {
							sibNeeded = true;
							scale = 0;
							index = ESP;
							base = ESP;
						}
					}
					break;
				}
				case MEM_TYPE_OFF_REG:
				{
					rm = pOperand1->mem.base;
					if (rm == ESP) {
						sibNeeded = true;
						scale = 0;
						index = ESP;
						base = ESP;
						if (pOperand1->mem.offsetSize == 32) {
							mod = MOD_FOUR_BYES;
							offsetSize = 32;
							offsetVal = pOperand1->mem.offset32;
						} else if (pOperand1->mem.offsetSize == 8) {
							mod = MOD_ONE_BYTE;
							offsetSize = 8;
							offsetVal = pOperand1->mem.offset8;
						} else { return -1; }
					} else 	if (pOperand1->mem.offsetSize == 32) {
						mod = MOD_FOUR_BYES;
						offsetSize = 32;
						offsetVal = pOperand1->mem.offset32;
					} else if (pOperand1->mem.offsetSize == 8) {
						mod = MOD_ONE_BYTE;
						offsetSize = 8;
						offsetVal = pOperand1->mem.offset8;
					} else { return -1; }
					break;
				}
				case MEM_TYPE_OFF_SI:
				{
					if (pOperand1->mem.base == EBP) return -1;
				}
				case MEM_TYPE_OFF_SIB:
				{

					if (pOperand1->mem.offsetSize == 32) {
						offsetSize = 32;
						offsetVal = pOperand1->mem.offset32;
					} else if (pOperand1->mem.offsetSize == 8) {
						offsetSize = 8;
						offsetVal = pOperand1->mem.offset8;
					} else { return -1; }
				}
				case MEM_TYPE_SIB:
				{
					if (pOperand1->mem.base == ESP) return -1;
					sibNeeded = true;
					rm = ESP;
					if (pOperand1->mem.type == MEM_TYPE_OFF_SI) {
						mod = 0;
						base = EBP;
					} else if (offsetSize == 32) {
						mod = 2;
						base = pOperand1->mem.base;
					} else if (offsetSize == 8) {
						mod = 1;
						base = pOperand1->mem.base;
					} else {
						mod = 0;
						base = pOperand1->mem.base;
					}
					scale = pOperand1->mem.scale;
					index = pOperand1->mem.index;
					if (index == ESP) return -1;
				}
			}
			break;
		}
		default:
		{
			return -1;
		}
	}

	unsigned char modrm = (unsigned char) ((mod << 6) | (opcodeExt << 3) | rm);
	pBuf[0] = modrm;
	int size = 1;
	if (sibNeeded) {
		size++;
		unsigned char sib = (unsigned char) ((scale << 6) | (index << 3) | base);
		pBuf[1] = sib;
	}
	if (offsetSize == 32) {
		*((int *) (pBuf+size)) = offsetVal;
		size += 4;
	} else if (offsetSize == 8) {
		*((char *) (pBuf+size)) = (char) offsetVal;
		size++;
	}

	if (pImmOp) {
		int immOpSz = encodeStaticOperand(pImmOp, pBuf + size);
		if (immOpSz < 0) return -1;
		size += immOpSz;
	}
	return size;
}

inline int encode2OpModrmSib(unsigned char *pBuf, Operand *pDst, Operand *pSrc) {
	if (pDst->type != REG8 && pDst->type != REG16 && pDst->type != REG32) return -1;
	return encodeOpcodeExtModrmSib(pBuf, pDst->val32, pSrc);
}

inline int encodeOpcode(Instruction *pInst, unsigned char *pBuf) {
	int size = 0;
	if (pInst->prefix) {
		pBuf[size++] = pInst->prefix;
	}
	if (pInst->flags & FLAG_0F_PREFIX) {
		pBuf[size++] = 0x0F;
	}
	pBuf[size++] = pInst->opcode1;
	if (pInst->flags & FLAG_2ND_OPCODE) {
		pBuf[size++] = pInst->opcode2;
	}
	return size;
}

/* assebles a basic 32 instruction with 2 operands */
int assm2OpInst(Instruction inst, Operand *pDst, Operand *pSrc, unsigned char *pBuf, unsigned int address) {
	int sz0 = encodeOpcode(&inst, pBuf);

	if (inst.flags & FLAG_SINGLE_BYTE) {
		if (inst.flags & FLAG_MODRM) return -1; // mutally exclusive
		if (inst.flags & FLAG_REVERSE_OPERANDS) return -1; // mutally exclusive
		if (inst.flags & FLAG_MODRM_OPCODE) return -1; // mutally exclusive
		if (inst.flags & FLAG_1ST_OPERAND_ONLY) return -1; // mutally exclusive
		if (inst.flags & FLAG_1ST_IMM_OPERAND_ONLY) return -1; // mutally exclusive
		if (inst.flags & FLAG_2ND_IMM_OPERAND_ONLY) return -1; // mutally exclusive
		return sz0;
	}
	
	if (inst.flags & FLAG_1ST_IMM_OPERAND_ONLY) {
		if (!pDst) return -1;
		if (inst.flags & FLAG_1ST_OPERAND_ONLY) return -1; // mutally exclusive with FLAG_1ST_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_2ND_IMM_OPERAND_ONLY) return -1; // mutally exclusive with FLAG_1ST_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_MODRM) return -1; // mutually exclusive with FLAG_1ST_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_MODRM_OPCODE) return -1; // mutually exclusive with FLAG_1ST_IMM_OPERAND_ONLY
		int sz1 = encodeStaticOperand(pDst, pBuf + sz0);
		if (sz1 < 0) return -1;
		return sz0 + sz1;
	}
	if (inst.flags & FLAG_2ND_IMM_OPERAND_ONLY) {
		if (!pSrc) return -1;
		if (inst.flags & FLAG_1ST_IMM_OPERAND_ONLY) return -1; // mutally exclusive with FLAG_2ND_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_1ST_OPERAND_ONLY) return -1; // mutally exclusive with FLAG_2ND_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_MODRM) return -1; // mutually exclusive with FLAG_2ND_IMM_OPERAND_ONLY
		if (inst.flags & FLAG_MODRM_OPCODE) return -1; // mutually exclusive with FLAG_2ND_IMM_OPERAND_ONLY
		int sz1 = encodeStaticOperand(pSrc, pBuf + sz0);
		if (sz1 < 0) return -1;
		return sz0 + sz1;
	}
	if (inst.flags & FLAG_MODRM) {
		if (inst.flags & FLAG_2ND_IMM_OPERAND_ONLY) return -1; // mutually exclusive with FLAG_MODRM
		int sz1 = -1;
		if (inst.flags & FLAG_MODRM_OPCODE) {
			if (inst.flags & FLAG_REVERSE_OPERANDS) return -1; // mutually exclusive with FLAG_MODRM_OPCODE
			if (inst.flags & FLAG_1ST_OPERAND_ONLY) {
				sz1 = encodeOpcodeExtModrmSib(pBuf + sz0, inst.opcodeExt, pDst);
			} else {
				sz1 = encodeOpcodeExtModrmSib(pBuf + sz0, inst.opcodeExt, pDst, pSrc);
			}
		} else if ((inst.flags & FLAG_REVERSE_OPERANDS) && pSrc)  {
			sz1 = encode2OpModrmSib(pBuf + sz0, pSrc, pDst);
		} else if (pSrc) {
			sz1 = encode2OpModrmSib(pBuf + sz0, pDst, pSrc);
		}
		if (sz1 < 0) return -1;
			return sz0 + sz1;
	}
	cout << "[ASM32-ERROR] Can Not Encode Instruction" << endl;
	return -1;
}

int assm0OpInst(const Instruction instArr[], unsigned int count, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	int i = assm32FindMatch(instArr, count, 0);
	if (pParamStr[0] || i >= 0) {
		return assm2OpInst(instArr[i], 0, 0, pBuf, address);
	} else {
		cout << "[ASM32-ERROR] No Instruction With This Signature: " << instArr[0].mnemonic << endl;
		return -1;
	}
}

int assm1OpInst(const Instruction instArr[], unsigned int count, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	Operand o;
	if (parseOperand(pParamStr, &o)) {
		int i = assm32FindMatch(instArr, count, 1, &o);
		if (i >= 0) {
			return assm2OpInst(instArr[i], &o, 0, pBuf, address);
		} else {
			cout << "[ASM32-ERROR] No Instruction With This Signature: " << instArr[0].mnemonic << " " << operandToStr(&o) << endl;
			return -1;
		}
	}
	cout << "[ASM32-ERROR] Error Reading Operand: " << pParamStr << endl;
	return -1;
}

int assm2OpInst(const Instruction instArr[], unsigned int count, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	Operand dst, src;
	if (parse2Operands(pParamStr, &dst, &src)) {
		int i = assm32FindMatch(instArr, count, 2, &dst, &src);
		if (i >= 0) {
			return assm2OpInst(instArr[i], &dst, &src, pBuf, address);
		} else {
			cout << "[ASM32-ERROR] No Instruction With This Signature: " << instArr[0].mnemonic << " " << operandToStr(&dst) << " " << operandToStr(&src) << endl;
			return -1;
		}
	}
	cout << "[ASM32-ERROR] Error Reading Operands: " << pParamStr << endl;
	return -1;
}

int assm3OpInst(const Instruction instArr[], unsigned int count, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	int comma = 0;
	for(; pParamStr[comma] && pParamStr[comma] != ','; comma++);
	for(; pParamStr[comma] && pParamStr[comma] != ','; comma++);
	if (pParamStr[comma] != ',') {
		cout << "[ASM32-ERROR] Expected 3 Params: " << pParamStr << endl;
		return -1;
	}
	Operand o1, o2, o3;
	if (parse3Operands(pParamStr, &o1, &o2, &o3)) {
		if (o3.type == IMM8 || o3.type == IMM16 || o3.type == IMM32) {
			int i = assm32FindMatch(instArr, count, 3, &o1, &o2, &o3);
			if (i >= 0) {
				int sz0 = assm2OpInst(instArr[i], &o1, &o2, pBuf, address);
				if (sz0 > 0) {
					int sz1 = encodeStaticOperand(&o3, pBuf + sz0);
					if (sz1 > 0) {
						return sz1 + sz0;
					}
				}
				return -1;
			} else {
				cout << "[ASM32-ERROR] No Instruction With This Signature: " << instArr[0].mnemonic << " " << operandToStr(&o1) << " " << operandToStr(&o2) << " " << operandToStr(&o3) << endl;
				return -1;
			}
		}
	}
	cout << "[ASM32-ERROR] Error Reading Operands: " << pParamStr << endl;
	return -1;
}

int assm32Inst(const Instruction instArr[], unsigned int count, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	int paramCount = 0;
	if (pParamStr[0]) {
		paramCount++;
	}
	
	for(int i = 0; pParamStr[i]; i++)
	{
		if (pParamStr[i] == ',') {
			paramCount++;
		}
	}
	if (paramCount == 1) { return assm1OpInst(instArr, count, pParamStr, pBuf, address); }
	if (paramCount == 2) { return assm2OpInst(instArr, count, pParamStr, pBuf, address); }
	if (paramCount == 3) { return assm3OpInst(instArr, count, pParamStr, pBuf, address); }
	return -1;
}

// #################
// #   ODD CASES   #
// #################

inline int assm32Call(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	bool absolute = pParamStr[0] == '+';
	Operand op;
	if (parseOperand(pParamStr + (absolute? 1 : 0), &op)) {
		if (op.type == IMM32) {
			if (!absolute) {
				op.val32 -= address + 5;
			}
			pBuf[0] = 0xE8;
			*((int *) (pBuf + 1)) = op.val32;
			return 5;
		}
	}
	return -1;
}

inline int assm32Jmp(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// todo: implement this shit
	// 66	FF	/4	reg16
	// 66	FF	/4	mem16

	bool absolute = pParamStr[0] == '+';

	Operand op;
	if (parseOperand(pParamStr + (absolute? 1 : 0), &op)) {
		if (!absolute) {
			int val;
			if (op.type == IMM8) {
				val = op.val8;
			} else if (op.type == IMM16) {
				val = op.val16;
			} else if (op.type == IMM32) {
				val = op.val32;
			} else return -1;

			val -= address;

			if (-126 <= val && val <= 127) {
				op.type = IMM8;
				op.val8 = val - 2;
			} else {
				op.type = IMM32;
				op.val32 = val - 5;
			}
		}

		// --	EB	--	rel8
		if (op.type == IMM8) {
			pBuf[0] = 0xEB;
			pBuf[1] = op.val8;
			return 2;
		}
		// 66	E9	--	rel16 (modified to be 32 instead)
		// --	E9	--	rel32
		if (op.type == IMM16 || op.type == IMM32) {
			pBuf[0] = 0xE9;
			*((int *) (pBuf + 1)) = op.val32;
			return 5;
		}
		if (absolute) return -1; // + works only with IMM values
		// --	FF	/4	reg32
		// --	FF	/4	mem32
		if (op.type == MEM32 || op.type == REG32) {
			pBuf[0] = 0xFF;
			int sz = encodeOpcodeExtModrmSib(pBuf + 1, 4, &op);
			if (sz > 0) {
				return 1 + sz;
			}
		}
	}
	return -1;
}

inline int assmJcond(char *pMnemonic, char *pParamStr, unsigned char *pBuf, unsigned int address) {
	bool absolute = pParamStr[0] == '+';
	Operand op;
	if (parseOperand(pParamStr + (absolute? 1 : 0), &op)) {
		if (!absolute) {
			int val;
			if (op.type == IMM8) {
				val = op.val8;
			} else if (op.type == IMM16) {
				val = op.val16;
			} else if (op.type == IMM32) {
				val = op.val32;
			} else return -1;

			val -= address;

			if (-126 <= val && val <= 127) {
				op.type = IMM8;
				op.val8 = val - 2;
			} else {
				op.type = IMM32;
				op.val32 = val - 6;
			}
		}
		
		const int cnt = sizeof(cjmp_instructions) / sizeof(JumpInstruction);
		for (int n = 0; n < cnt; n++) {
			if (str_equal(pMnemonic, (char *) &(cjmp_instructions[n].mnemonic))) {
				if (op.type == IMM8) {
					pBuf[0] = (unsigned char) (0x70 | cjmp_instructions[n].opcode);
					pBuf[1] = op.val8;
					return 2;
				}
				if (op.type == IMM32) {
					pBuf[0] = 0x0F;
					pBuf[1] = (unsigned char) (0x80 | cjmp_instructions[n].opcode);
					*((int *) (pBuf + 2)) = op.val32;
					return 6;
				}
				break;
			}
		}
	}
	return -1;
}

inline int assm32Jecxz(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	bool absolute = pParamStr[0] == '+';
	Operand op;
	if (parseOperand(pParamStr + (absolute? 1 : 0), &op)) {
		if (!absolute) {
			int val;
			if (op.type == IMM8) {
				val = op.val8;
			} else if (op.type == IMM16) {
				val = op.val16;
			} else if (op.type == IMM32) {
				val = op.val32;
			} else return -1;

			val -= address;

			if (-126 <= val && val <= 127) {
				op.type = IMM8;
				op.val8 = val - 2;
			} else {
				return -1;
			}
		}

		if (op.type == IMM8) {
			pBuf[0] = 0xE3;
			pBuf[0] = op.val8;
			return 2;
		}
	}
	return -1;
}

inline int assm32Ret(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// C3		--
	// C2		IMM16
	if (!pParamStr[0]) {
		*pBuf = 0xC3;
		return 1;
	} else {
		Operand op;
		if (parseOperand(pParamStr, &op)) {
			if (op.type == IMM8) {
				*pBuf = 0xC2;
				*((unsigned short *) (pBuf + 1)) = op.uVal8;
				return 3;
			}
			if (op.type == IMM16) {
				*pBuf = 0xC2;
				*((unsigned short *) (pBuf + 1)) = op.uVal16;
				return 3;
			}
		}
	}
	return -1;
}

inline int assm32Nop(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	if (!pParamStr[0]) {
		*pBuf = 0x90;
		return 1;
	}
	cout << "[ASM32-ERROR] Nop has no operands" << endl;
	return -1;
}

inline int assm32Db(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	Operand sz;
	if (parseOperand(pParamStr, &sz)) {
		if (sz.type == IMM32) {
			unsigned int allocSize = (unsigned int) sz.val32;
			for (unsigned int i = 0; i < allocSize; i++) {
				pBuf[i] = 0;
			}
			return allocSize;
		}
	}
	return -1;
}

// #################
// #   BASIC       #
// #################

inline int assm32Mov(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(mov_instructions, sizeof(mov_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Cmp(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(cmp_instructions, sizeof(cmp_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Movsx(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(movsx_instructions, sizeof(movsx_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Movzx(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(movzx_instructions, sizeof(movzx_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Shr(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm32Inst(shr_instructions, sizeof(shr_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Sar(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm32Inst(sar_instructions, sizeof(sar_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Shl(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm32Inst(shl_instructions, sizeof(shl_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Add(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(add_instructions, sizeof(add_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Xor(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(xor_instructions, sizeof(xor_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Mul(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(mul_instructions, sizeof(mul_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Imul(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm32Inst(imul_instructions, sizeof(imul_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Lea(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(lea_instructions, sizeof(lea_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Push(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(push_instructions, sizeof(push_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Pop(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(pop_instructions, sizeof(pop_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Sub(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm2OpInst(sub_instructions, sizeof(sub_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Inc(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(inc_instructions, sizeof(inc_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Dec(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(dec_instructions, sizeof(dec_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Fld(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm1OpInst(fld_instructions, sizeof(fld_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Fmulp(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// todo: add other opcodes
	return assm0OpInst(fmulp_instructions, sizeof(fmulp_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Fist(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// todo: add other opcodes
	return assm1OpInst(fist_instructions, sizeof(fist_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Fild(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// todo: add other opcodes
	return assm1OpInst(fild_instructions, sizeof(fild_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Fistp(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	// todo: add other opcodes
	return assm1OpInst(fistp_instructions, sizeof(fistp_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Pushad(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm0OpInst(pushad_instructions, sizeof(pushad_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

inline int assm32Popad(char *pParamStr, unsigned char *pBuf, unsigned int address) {
	return assm0OpInst(popad_instructions, sizeof(popad_instructions) / sizeof(Instruction), pParamStr, pBuf, address);
}

//

int assm32_(const char *inst, unsigned char *pBuf, unsigned int address) {
	if (str_length(inst) > MAX_INST_LEN) {
		cout << "[ASM32-ERROR] Line Too Long: " << inst << endl;
		return -1; // instruction way too long!
	}
	char *pInst = trimLeft((char *) inst);
	int mnSize = nextTokenLen(pInst);
	int mcLen;
	if (mnSize < 2) return -1; // too short to be an instruction
	char opbuf[MAX_INST_LEN];
	char *pOpBuf = (char *) &opbuf;
	removeSpaces(pInst + mnSize, pOpBuf);
	#define ifc(i, x) if (pInst[i] == x)
	#define call(m) mcLen = m(pOpBuf, pBuf, address); \
	                if (mcLen <= 0) { cout << "[ASM32-ERROR] Could Not Assemble: "  << pInst << endl; } \
					return mcLen;
	#define UNKNOWN_INSTRUCTION cout << "[ASM32-ERROR] Unknown Instruction: " << pInst << endl; return -1;
	ifc(0, 'a') {
		ifc(1, 'd') {
			ifc(2, 'd') {
				// add
				if (mnSize == 3) { call(assm32Add) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'c') {
		ifc(1, 'a') {
			ifc(2, 'l') {
				ifc(3, 'l') {
					if (mnSize == 4) { call(assm32Call) }
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'm') {
			ifc(2, 'p') {
				// cmp
				if (mnSize == 3) { call(assm32Cmp) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'd') {
		ifc(1, 'b') {
			// db
			if (mnSize == 2) { call(assm32Db) }
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'e') {
			ifc(2, 'c') {
				// dec
				if (mnSize == 3) { call(assm32Dec) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'f') {
		ifc(1, 'l') {
			ifc(2, 'd') {
				if (mnSize == 3) { call(assm32Fld) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'i') {
			ifc(2, 'l') {
				ifc(3, 'd') {
					if (mnSize == 4) { call (assm32Fild) }
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			ifc(2, 's') {
				ifc(3, 't') {
					if (mnSize == 4) { call(assm32Fist) }
					ifc(4, 'p') {
						if (mnSize == 5) { call(assm32Fistp) }
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'm') {
			ifc(2, 'u') {
				ifc(3, 'l') {
					ifc(4, 'p') {
						if (mnSize == 5) { call(assm32Fmulp) }
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'i') {
		ifc(1, 'm') {
			ifc(2, 'u') {
				ifc(3, 'l') {
					// imul
					if (mnSize == 4) { call(assm32Imul) }
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'n') {
			ifc(2, 'c') {
				if (mnSize == 3) { call(assm32Inc) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'j') {
		ifc(1, 'm') {
			ifc(2, 'p') {
				// jmp
				if (mnSize == 3) { call(assm32Jmp) }
				// UNKNOWN_INSTRUCTION // may block cond jump check
			}
			// UNKNOWN_INSTRUCTION // may block cond jump check
		}
		ifc(1, 'e') {
			ifc(2, 'c') {
				ifc(3, 'x') {
					ifc(4, 'z') {
						// jecxz
						if (mnSize == 5) { call(assm32Jecxz) };
						// UNKNOWN_INSTRUCTION // may block cond jump check
					}
					// UNKNOWN_INSTRUCTION // may block cond jump check
				}
				// UNKNOWN_INSTRUCTION // may block cond jump check
			}
			// UNKNOWN_INSTRUCTION // may block cond jump check
		}
		// cond jump
		if (mnSize < 8) {
			char mnemonic[8];
			int u = 0;
			for (; u < mnSize; u++) {
				mnemonic[u] = pInst[u];
			}
			mnemonic[u] = 0;
			return assmJcond((char *) &mnemonic, pOpBuf, pBuf, address);
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'l') {
		ifc(1, 'e') {
			ifc(2, 'a') {
				if (mnSize == 3) { call((assm32Lea)) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'm') {
		ifc(1, 'o') {
			ifc(2, 'v') {
				// mov
				if (mnSize == 3) { call(assm32Mov) }
				ifc(3, 's') {
					ifc(4, 'x') {
						// movsx
						if (mnSize == 5) { call(assm32Movsx) }
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				ifc(3, 'z') {
					ifc(4, 'x') {
						// movzx
						if (mnSize == 5) { call(assm32Movzx) }
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'u') {
			ifc(2, 'l') {
				// mul
				if (mnSize == 3) { call(assm32Mul) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'n') {
		ifc(1, 'o') {
			ifc(2, 'p') {
				// nop
				if (mnSize == 3) { call(assm32Nop) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'p') {
		ifc(1, 'o') {
			ifc(2, 'p') {
				if (mnSize == 3) { call(assm32Pop) }
				ifc(3, 'a') {
					ifc(4, 'd') {
						if (mnSize == 5) { call(assm32Popad) }
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'u') {
			ifc(2, 's'){
				ifc(3, 'h') {
					if (mnSize == 4) { call(assm32Push) }
					ifc(4, 'a') {
						ifc(5, 'd') {
							if (mnSize == 6) { call(assm32Pushad) }
							UNKNOWN_INSTRUCTION
						}
						UNKNOWN_INSTRUCTION
					}
					UNKNOWN_INSTRUCTION
				}
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'r') {
		ifc(1, 'e') {
			ifc(2, 't') {
				// ret
				if (mnSize == 3) { call(assm32Ret) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 's') {
		ifc(1, 'a') {
			ifc(2, 'l') {
				// sal
				if (mnSize == 3) { call(assm32Shl) }
				UNKNOWN_INSTRUCTION
			}
			ifc(2, 'r') {
				// sar
				if (mnSize == 3) { call(assm32Sar) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'h') {
			ifc(2, 'l') {
				// shl
				if (mnSize == 3) { call(assm32Shl) }
				UNKNOWN_INSTRUCTION
			}
			ifc(2, 'r') {
				// shr
				if (mnSize == 3) { call(assm32Shr) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		ifc(1, 'u') {
			ifc(2, 'b') {
				if (mnSize == 3) { call(assm32Sub) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	ifc(0, 'x') {
		ifc(1, 'o') {
			ifc(2, 'r') {
				if (mnSize == 3) { call(assm32Xor) }
				UNKNOWN_INSTRUCTION
			}
			UNKNOWN_INSTRUCTION
		}
		UNKNOWN_INSTRUCTION
	}
	UNKNOWN_INSTRUCTION
}
