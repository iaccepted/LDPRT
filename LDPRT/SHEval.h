#ifndef _SHEVAL_H_
#define _SHEVAL_H_

#include "Global.h"

typedef void(*SHEvalFunc)(const LFLOAT, const LFLOAT, const LFLOAT, LFLOAT*);
void SHEval3(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval4(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval5(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval6(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval7(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval8(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval9(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval10(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);

#endif