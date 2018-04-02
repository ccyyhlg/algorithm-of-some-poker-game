#include "StdAfx.h"
#include "GameLogic.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>
//////////////////////////////////////////////////////////////////////////

//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[52] =
{
	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x01,	//方块 2 - K A
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x11,	//梅花 2 - K A
	0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x21,	//红桃 2 - K A
	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x31,	//黑桃 2 - K A	
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_iCurIdx = 0;
}

//析构函数
CGameLogic::~CGameLogic()
{
}


int Random2Value(int val)
{
	int n = abs(rand() + (int)time(NULL));
	if (val > 0)
		return abs(n%val);
	return n;
}
//混乱扑克
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	//混乱准备
	BYTE bCardData[sizeof(m_bCardListData)];
	memcpy(bCardData, m_bCardListData, sizeof(m_bCardListData));

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = Random2Value(bBufferCount - bRandCount);
		bCardBuffer[bRandCount++] = bCardData[bPosition];
		bCardData[bPosition] = bCardData[bBufferCount - bRandCount];
	} while (bRandCount < bBufferCount);

	return;
}

int CGameLogic::CompareCardType(int type1, int type2){
	int sign = 1, maxtype = type1;
	int re = 0;

	if (type1 < type2){ sign = -1; maxtype = type2; }

	if		(maxtype == THREEFLUSH)			{ re = 3; }
	else if (maxtype == THREESTRIGHT)		{ re =4; }
	else if (maxtype == SIXPAIRS)			{ re = 4; }
	else if (maxtype == FIVEPAIRSANDTHREE)	{ re = 5; }
	else if (maxtype == FOUR_THREE)			{ re = 6; }
	else if (maxtype == ALLSAMECOLOR)		{ re = 10; }
	else if (maxtype == ALLLOW)				{ re = 10; }
	else if (maxtype == ALLHIGH)			{ re = 10; }
	else if (maxtype == THREE_BOMB)			{ re = 20; }
	else if (maxtype == THREE_STRIGHTFLUSH)	{ re = 20; }
	else if (maxtype == ROYALCARD)			{ re = 24; }
	else if (maxtype == LOWDRAGON)			{ re = 36; }
	else if (maxtype == BLACKDRAGON)		{ re = 108; }

	return re*sign;
}

void DeleteSortedCards(BYTE So[], BYTE Ori[], int sorted){
	int oc = 13 - sorted;
	for (int i = 0; i < sorted; i++){
		for (int j = 0; j < 13; j++){
			if (So[i] == Ori[j]){
				BYTE Cu[13] {};
				int po = j + 1;
				int c = 13 - po;
				memcpy(Cu, Ori + po, c);
				memcpy(Ori + po - 1, Cu, c);
				break;
			}
		}
	}
	memset(Ori + oc, 0, sorted);
}

//時間もう切れだ
//2018.3.14  Stephen William Hawking R.I.P
void CGameLogic::SortCards(BYTE cards[], int STYPE){
	int v[13] {};
	int c[15] {};
	int ExStr = 0;
	for (int i = 0; i < 13; i++){
		v[i] = (cards[i] & 0x0f);
		c[v[i]]++;
		if ((ExStr & (1 << (v[i] - 1))) == 0){ ExStr += 1 << (v[i] - 1); }
		if (v[i] == 1){ 
			v[i] = 14; 
			c[v[i]]++; 
			if ((ExStr & (1 << (v[i] - 1))) == 0){ ExStr += 1 << (v[i] - 1); }
		}
	}

	if (STYPE == THREEFLUSH || STYPE == THREE_STRIGHTFLUSH || STYPE == BLACKDRAGON){
		//basic sort
		for (int i = 0; i < 13; i++){
			int index = i;
			for (int j = i; j < 13; j++){
				if (cards[j] > cards[index]){
					index = j;
				}
			}
			BYTE ch = cards[i];
			cards[i] = cards[index];
			cards[index] = ch;
		}
	}
	else{
		//sort with val
		for (int i = 0; i < 13; i++){
			int index = i;
			for (int j = i + 1; j < 13; j++){
				if (v[j] > v[index]){
					index = j;
				}
			}
			BYTE ch = cards[i];
			cards[i] = cards[index];
			cards[index] = ch;

			int in = v[i];
			v[i] = v[index];
			v[index] = in;
		}

		if (STYPE == THREESTRIGHT){
			int TexPos[] = { 15873, 7936, 3968, 1984, 992, 496, 248, 124, 62, 8223 };
			int ThPos[] = { 14337, 7168, 3584, 1792, 896, 448, 224, 112, 56, 28, 14, 8199 };

			int sorted = 0;
			BYTE cp[13] {};
			memcpy(cp, cards, 13);
			BYTE scards[13] {};

			while (sorted < 13){
				for (int i = 0; i < 12; i++){
					if (((TexPos[i] & ExStr) == TexPos[i]) && i < 10){
						int val = 14 - i;
						int oc = 13 - sorted;
						int times = 5;
						for (int j = 0; j < oc; j++){
							if (val == (cp[j] & 0x0f) || val == 14){
								times--;
								scards[sorted++] = cp[j];
								c[val]--;
								if (val == 14){ c[1]--; }
								else if (val == 1){ c[14]--; }
								if (c[val] == 0){ 
									ExStr -= 1 << (val - 1);
									if (val == 1){ ExStr -= 1 << 13; }
									else if (val == 14){ ExStr -= 1; }
								}
								val--;
								if (val == 1){ j = -1; }
							}
							if (times == 0){ break; }
						}
						DeleteSortedCards(scards, cp, sorted);
						i--;
						continue;
					}

					if ((ThPos[i] & ExStr) == ThPos[i]){
						int val = 14 - i;
						int oc = 13 - sorted;
						int times = 3;
						for (int j = 0; j < oc; j++){
							if (val == (cp[j] & 0x0f) || val == 14){
								times--;
								scards[sorted++] = cp[j];
								c[val]--;
								if (val == 14){ c[1]--; }
								else if (val == 1){ c[14]--; }
								if (c[val] == 0){
									ExStr -= 1 << (val - 1);
									if (val == 1){ ExStr -= 1 << 13; }
									else if (val == 14){ ExStr -= 1; }
								}
								val--;
								if (val == 1){ j = -1; }
							}
							if (times == 0){ break; }
						}
						DeleteSortedCards(scards, cp, sorted);
						i--;
						continue;
					}
				}
			}
			memcpy(cards, scards, 13);
		}
	}
}

void CGameLogic::CompareCards(unsigned int pv1[], unsigned int pv2[], int re[]){
	int in = 1;
	int maxtype;

	for (int i = 0; i < 3; i++){
		if (pv1[i]>pv2[i]){
			in = 1;
			maxtype = (int)(pv1[i] >> 24);
		}
		else{
			in = -1;
			maxtype = (int)(pv2[i] >> 24);
		}

		if (maxtype == CT_THREE && i==0){ re[i] = 3 * in; }
		else if (maxtype == CT_FULLHOUSE && i == 1){ re[i] = 2 * in; }
		else if (maxtype == CT_FOUR && i == 1){ re[i] = 7 * in; }
		else if (maxtype == CT_FOUR && i == 2){ re[i] = 5 * in; }
		else if (maxtype == CT_FLUSHSTRIGHT && i == 1){ re[i] = 9 * in; }
		else if (maxtype == CT_FLUSHSTRIGHT && i == 2){ re[i] = 5 * in; }
		else { re[i] = 1 * in; }
	}
}

int GetCount(int val){
	val = val >> 1;
	int count = 0;
	while (val > 0){
		count += val % 2;
		val = val >> 1;
	}
	return count;
}

//回溯3层
bool JudgeThreeFlushStright(int Str[], int TexPos[], int ThPos[], int lay){
	bool re = false;

	if (lay < 3){
		for (int i = 0; i < 10; i++){
			for (int j = 0; j < 4; j++){
				if ((Str[j] & TexPos[i]) == TexPos[i]){
					Str[j] -= TexPos[i];
					re = JudgeThreeFlushStright(Str, TexPos, ThPos, lay + 1);
					Str[j] += TexPos[i];
				}
			}
		}

		return re;
	}
	else{
		for (int i = 0; i < 12; i++){
			for (int j = 0; j < 4; j++){
				if ((Str[j] & ThPos[i]) == ThPos[i]){
					return true;
				}
			}
		}
	}

	return re;
}

int CalcNewStr(int Str, int Tex, int h[]){
	Str -= Tex;
	int index = 1;
	while (Tex){
		if (Tex % 2){ h[index]--; }
		index++;
		Tex = Tex >> 1;
	}

	for (int i = 0; i < 15; i++){
		if (h[i]>0 && (Str&(1 << (i - 1))) == 0){
			Str += (1 << (i - 1));
		}
	}

	return Str;
}

bool JudgeThreeStright(int ExStr, int h[], int TexPos[], int ThPos[],int lay){
	bool re = false;

	if (lay < 3){
		for (int i = 0; i < 10; i++){
			if ((ExStr&TexPos[i]) == TexPos[i]){
				ExStr = CalcNewStr(ExStr, TexPos[i], h);
				return re = JudgeThreeStright(ExStr, h, TexPos, ThPos, lay + 1);
			}
		}
	}
	else{
		for (int i = 0; i < 12; i++){
			if ((ExStr&ThPos[i]) == ThPos[i]){
				return true;
			}
		}
	}

	return re;
}

#define WEIGHT(a,b) (a<<24|b)
unsigned int GetWeight(int Str[], int Exstr, int h[], int TexPos[], int ThPos[], int BlackCount){

	//青龙
	int FiveFlushCount = 0, ThreeFlushCount = 0;
	for (int i = 0; i < 4; i++){
		if (Str[i] == 0x3fff){
			return BLACKDRAGON;
		}

		int in = GetCount(Str[i]);
		if (in == 3){ ThreeFlushCount++; }
		else if (in == 5){ FiveFlushCount++; }
		else if (in == 8){ FiveFlushCount++; ThreeFlushCount++; }
		else if (in == 10){ FiveFlushCount += 2; }
	}

	//龙
	if (Exstr == 0x3fff){ return LOWDRAGON; }

	//十二皇族
	int RoyalCount = 0, HighCount = 0, LowCount = 0;
	for (int i = 14; i > 1; i--){
		if (i > 10){ RoyalCount += h[i]; }
		if (i > 8){ HighCount += h[i]; }
		else { LowCount += h[i]; }
	}
	if (RoyalCount == 13){ return ROYALCARD; }

	//三同花顺
	if (JudgeThreeFlushStright(Str, TexPos, ThPos, 1)){
		return THREE_STRIGHTFLUSH;
	}

	//三分天下(三个炸)
	int PairCount = 0, ThreeCount = 0, BombCount = 0;
	for (int i = 14; i > 1; i--){
		if (h[i] == 4){ BombCount++; }
		else if (h[i] == 3){ ThreeCount++; }
		else if (h[i] == 2){ PairCount++; }

		if (BombCount == 3){ return THREE_BOMB; }
	}

	//全大
	if (HighCount == 13){ return ALLHIGH; }
	//全小
	if (LowCount == 13){ return ALLLOW; }
	//凑一色
	if (BlackCount == 13 || BlackCount == 0){ return ALLSAMECOLOR; }

	//四套三条，五对三条，六对半
	if (ThreeCount == 4){ return FOUR_THREE; }
	if (PairCount == 5 && ThreeCount == 1){ return FIVEPAIRSANDTHREE; }
	if (PairCount == 6){ return SIXPAIRS; }

	//三顺子
	if (JudgeThreeStright(Exstr, h, TexPos, ThPos, 1)){
		return THREESTRIGHT;
	}
	//三同花
	if (FiveFlushCount == 2 && ThreeFlushCount == 1){ return THREEFLUSH; }

	return NORMAL_TYPE;
}

unsigned int GetWeight(int Str[], int Exstr, std::list<int> hs, int h[], int TexPos[], int Count){
	std::list<int>::iterator ito;
	int nw = 0;
	//同花顺
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 10; j++){
			if ((Str[i] & TexPos[j]) == TexPos[j]){
				nw = (14 - j) << 20;
				return WEIGHT(CT_FLUSHSTRIGHT, nw);
			}
		}
	}

	//同花
	for (int i = 0; i < 4; i++){
		if (GetCount(Str[i]) == 5){
			int in = Str[i];
			int index = 0;
			int k = 4;
			in = in >> 1;
			while (in){
				if (in % 2){
					nw += (index + 1) << k;
					k += 4;
				}
				in = in >> 1;
				index++;
			}
			return WEIGHT(CT_FLUSH, nw);
		}
	}

	//顺子
	for (int i = 0; i < 10; i++){
		if (Exstr == TexPos[i]){
			nw = (14 - i) << 20;
			return WEIGHT(CT_STRIGHT, nw);
		}
	}

	//わからん
	ito = hs.end();
	ito--;
	int count1 = 0, count2 = 0;
	int col1 = 0, val1 = 0;
	int val2 = 0;
	count1 = *ito >> 8;
	col1 = *ito % 16;
	val1 = (*ito >> 4) % 16;
	if (ito != hs.begin()){
		ito--;
		count2 = *ito >> 8;
		val2 = (*ito >> 4) % 16;
	}

	nw = val1 << 20;
	nw += val2 << 16;
	int in = Count - count1 - count2;
	int k = 0;
	for (int i = 14; i > 0; i--){
		if (k == in){ break; }
		if (i != val1 && i != val2 && h[i] > 0){
			nw += i << (12 - k * 4);
			k++;
		}
	}
	if (count1 == 3 && count2 > 1){
		return WEIGHT(CT_FULLHOUSE, nw);
	}
	else if (count1 == 2 && count2 > 1){
		return WEIGHT(CT_TWOPAIRS, nw);
	}
	else if (count1 == 4){
		return WEIGHT(CT_FOUR, nw);
	}
	else if (count1 == 3)
	{
		return WEIGHT(CT_THREE, nw);
	}
	else if (count1 == 2){
		return WEIGHT(CT_ONEPAIR, nw);
	}
	else{
		return WEIGHT(CT_HIGHCARD, nw);
	}
}

unsigned int CGameLogic::CalcWeight(const BYTE bCards[], int CardCount){
	int Str[4];
	memset(Str, 0, sizeof(int)* 4);
	int ExStr = 0;
	int h[15];
	memset(h, 0, sizeof(int)* 15);
	int BlackCount = 0;
	std::list<int> hs;
	std::list<int>::iterator ito;

	int TexPos[] = { 15873, 7936, 3968, 1984, 992, 496, 248, 124, 62, 8223 };
	int ThPos[] = { 14337, 7168, 3584, 1792, 896, 448, 224, 112, 56, 28, 14, 8199 };

	for (int i = 0; i < CardCount; i++){
		int col = bCards[i] >> 4;
		int val = bCards[i] % 16;
		if (val == 1){ val = 14; }

		if (col == 1 || col == 3){ BlackCount++; }

		h[val]++;
		if (val == 14){ h[1]++; }

		bool ex = false;
		if (i == 0){ ex = true; }
		for (ito = hs.begin(); ito != hs.end(); ito++){
			if ((*ito & 0xf0) == (val << 4)){
				*ito += 0x100;
				if ((*ito & 0x0f) < col){
					*ito = *ito - (*ito & 0x0f) + col;
				}
				ex = false;
				break;
			}
			ex = true;
		}
		if (ex){ hs.push_back(0x100 + (val << 4) + col); }
		hs.sort();

		Str[col] += 1 << (val - 1);
		if (val == 14){ Str[col] += 1; }

		if ((ExStr&(1 << (val - 1))) == 0){ 
			ExStr += 1 << (val - 1);
			if (val == 14){ ExStr++; }
		}
	}
	if (CardCount == 13){ return GetWeight(Str, ExStr, h, TexPos, ThPos, BlackCount); }
	else{ return GetWeight(Str, ExStr, hs, h, TexPos, CardCount); }
}

bool CGameLogic::JudgeOrder(unsigned int Weight[]){
	for (int i = 0; i < 2; i++){
		if (Weight[i] > Weight[i + 1]){
			return false;
		}
	}
	return true;
}

void DeleteUsedCards(std::list<BYTE> &cards, std::list<BYTE> us, BYTE Cur[], int La, int ha){
	int st = 0;
	if (La == 1){ st = 8; }
	else if (La == 2){ st = 3; }
	else { st = 0; }
	int i = st + ha;
	for (std::list<BYTE>::iterator ui = us.begin(); ui != us.end(); ui++){
		cards.remove(*ui);
		Cur[i++] = *ui;
	}
	for (; i < (La == 3 ? st + 3 : st + 5);){
		Cur[i++] = 0;
	}
}

void RecallCards(std::list<BYTE> &cards, std::list<BYTE> us, BYTE Cur[], int La, int ha){
	for (std::list<BYTE>::iterator ui = us.begin(); ui != us.end(); ui++){
		cards.push_back(*ui);
	}
	int st = 0;
	if (La == 1){ st = 8; }
	else if (La == 2){ st = 3; }
	else { st = 0; }
	st += ha;
	memset(Cur + st, 0, (La < 3 ? 5 - ha : 3 - ha));
}

void CGameLogic::BestSequence(std::list<BYTE> &card, unsigned int We[], int Layer, int type, BYTE CurSeq[], BYTE BestSeq[], unsigned int BestWe[]){
	int TexPos[] = { 15873, 7936, 3968, 1984, 992, 496, 248, 124, 62, 8223 };
	std::list<BYTE>::iterator ic = card.begin();

	int Str[4];
	memset(Str, 0, sizeof(int)* 4);
	int ExStr = 0;
	int h[15];
	memset(h, 0, sizeof(int)* 15);
	int C34[] = { 7, 11, 13, 14 };
	int C24[] = { 3, 5, 6, 9, 10, 12 };

	for (; ic != card.end(); ic++){
		int col = *ic >> 4;
		int val = *ic % 16;
		if (val == 1){ val = 14; }

		h[val] += 1 << col;
		if (val == 14){ h[1] += 1 << col; }

		Str[col] += 1 << (val - 1);
		if (val == 14){ Str[col] += 1; }

		if ((ExStr & (1 << (val - 1))) == 0){
			ExStr += 1 << (val - 1);
			if (val == 14){ ExStr++; }
		}
	}

	//同花顺
	if (type > 8 && Layer < 3){
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 10; j++){
				if ((Str[i] & TexPos[j]) == TexPos[j]){
					std::list<BYTE> us;
					for (int v = 14 - j; v > 14 - j - 5; v--){
						us.push_back((i << 4) + (v == 14 ? 1 : v));
					}
					DeleteUsedCards(card, us, CurSeq, Layer, 0);
					BestSequence(card, We, Layer + 1, 9, CurSeq, BestSeq, BestWe);
					return;
				}
			}
		}
	}

	//铁支
	if (type > 7 && Layer < 3){
		for (int i = 14; i > 1; i--){
			if (h[i] == 15){
				std::list<BYTE> us;
				for (int c = 0; c < 4; c++){
					us.push_back((c << 4) + i);
				}
				DeleteUsedCards(card, us, CurSeq, Layer, 0);
				BestSequence(card, We, Layer + 1, 8, CurSeq, BestSeq, BestWe);
				return;
			}
		}
	}

	//葫芦
	if (type > 6 && Layer < 3){
		SELECTTHREE(0)
			SELECTPAIR(3)
			BestSequence(card, We, Layer + 1, 7, CurSeq, BestSeq, BestWe);
		ENDSELECTPAIR(3)
			ENDSELECTTHREE(0)
	}

	//同花
	if (type > 5 && Layer < 3){
		for (int i = 0; i < 4; i++){
			if (GetCount(Str[i]) >= 5){
				int cm = Str[i] % (1 << 13);
				BYTE sscards[10] {};
				int cc = 1;
				int k = 0;
				while (cm){
					if (cm % 2 != 0){ sscards[k++] = (i << 4) + cc; }
					cm = cm >> 1;
					cc++;
				}

				int in[5] {0, 1, 2, 3, 4};
				std::list<BYTE> us;
				while (in[4] < k){
					if (in[0] == in[1]){
						in[0] = 0;
						in[1]++;
					}
					if (in[1] == in[2]){
						in[1] = 1;
						in[2]++;
					}
					if (in[2] == in[3]){
						in[2] = 2;
						in[3]++;
					}
					if (in[3] == in[4]){
						in[3] = 3;
						in[4]++;
						if (in[4] == k){ break; }
					}
					us.clear();
					for (int ii = 0; ii < 5; ii++){
						us.push_back(sscards[in[ii]]);
					}
					DeleteUsedCards(card, us, CurSeq, Layer, 0);
					BestSequence(card, We, Layer + 1, 6, CurSeq, BestSeq, BestWe);
					RecallCards(card, us, CurSeq, Layer, 0);
					in[0]++;
				}
			}
		}
	}

	//顺子
	if (type > 4 && Layer < 3){
		for (int j = 0; j < 10; j++){
			if ((ExStr & TexPos[j]) == TexPos[j]){
				int v = 14 - j;
				BYTE stcards[5][3] {};
				int Count = 1;
				//设置卡
				for (int i = 0; i < 5; i++){
					int cc = 0;
					for (int k = 0; k < 4; k++){
						if (((1 << k) & h[v - i]) == (1 << k)){
							stcards[i][cc++] = (k << 4) + v - i;
						}
					}
				}

				int in[5] {};
				std::list<BYTE> us;
				while (stcards[0][in[0]] != 0){
					if (stcards[4][in[4]] == 0){
						in[4] = 0;
						in[3]++;
					}
					if (stcards[3][in[3]] == 0){
						in[3] = 0;
						in[2]++;
					}
					if (stcards[2][in[2]] == 0){
						in[2] = 0;
						in[1]++;
					}
					if (stcards[1][in[1]] == 0){
						in[1] = 0;
						in[0]++;
					}
					if (stcards[0][in[0]] == 0){
						break;
					}
					us.clear();
					for (int ii = 0; ii < 5; ii++){
						us.push_back(stcards[ii][in[ii]]);
					}
					DeleteUsedCards(card, us, CurSeq, Layer, 0);
					BestSequence(card, We, Layer + 1, 5, CurSeq, BestSeq, BestWe);
					RecallCards(card, us, CurSeq, Layer, 0);
					in[4]++;
				}
				return;
			}
		}
	}

	//三张
	if (type > 3){
		SELECTTHREE(0)
			BestSequence(card, We, Layer + 1, 4, CurSeq, BestSeq, BestWe);
		RecallCards(card, ust, CurSeq, Layer, 0);
		return;
	}
}}
	}

	//两对
	if (type > 2 && Layer < 3){
		for (int i = 14; i > 1; i--){
			for (int j = 0; j < 6; j++){
				if (h[i] == C24[j]){
					std::list<BYTE> us;
					int cc = 0;
					int cm = C24[j];
					while (cm){
						if (cm % 2 != 0){
							us.push_back((cc << 4) + i);
						}
						cc++;
						cm = cm >> 1;
					}
					for (int x = 2; x < i; x++){
						for (int y = 0; y < 6; y++){
							if (h[x] == C24[y]){
								int cc2 = 0;
								int cm2 = C24[y];
								while (cm2){
									if (cm2 % 2){
										us.push_back((cc2 << 4) + x);
									}
									cc2++;
									cm2 = cm2 >> 1;
								}
								DeleteUsedCards(card, us, CurSeq, Layer, 0);
								BestSequence(card, We, Layer + 1, 3, CurSeq, BestSeq, BestWe);
								RecallCards(card, us, CurSeq, Layer, 0);
								return;
							}
						}
					}
				}
			}
		}
	}

	//对子
	if (type > 1 && Layer < 4){
		int va1 = 1;
		SELECTPAIR(0)
			BestSequence(card, We, Layer + 1, 2, CurSeq, BestSeq, BestWe);
		RecallCards(card, ustw, CurSeq, Layer, 0);
		return;
	}}}
	}

	card.sort();
	std::list<BYTE> us;
	int inde[10] {};
	int k = 0;

	int sp = Layer;
	while (sp < 4){
		int st = 0;
		if (sp == 2){ st = 3; }
		std::list<BYTE>::iterator ito = card.begin();
		BYTE inc = 0;
		int v2 = 0;
		for (; ito != card.end(); ito++){
			int v1 = *ito & 0x0f;
			if (v1 == 1){ v1 = 14; }
			if (v1 > v2){
				inc = *ito;
				v2 = v1;
			}
		}

		CurSeq[st] = inc;
		card.remove(inc);
		us.push_back(inc);
		inde[k++] = st;
		sp++;
	}

	for (int i = 0; i < 13; i++){
		std::list<BYTE>::iterator ito = card.begin();
		BYTE inc = 15;
		int v2 = 15;
		for (; ito != card.end(); ito++){
			int v1 = *ito & 0x0f;
			if (v1 == 1){ v1 = 14; }
			if (v1 < v2){
				inc = *ito;
				v2 = v1;
			}
		}
		if (CurSeq[i] == 0){
			card.remove(inc);
			us.push_back(inc);
			CurSeq[i] = inc;
			inde[k++] = i;
		}
	}
	unsigned int Cur[3]{};
	Cur[0] = CalcWeight(CurSeq, 3);
	Cur[1] = CalcWeight(CurSeq + 3, 5);
	Cur[2] = CalcWeight(CurSeq + 8, 5);
	if ((Cur[0] + Cur[1] + Cur[2] > BestWe[0] + BestWe[1] + BestWe[2]) && JudgeOrder(Cur)){
		memcpy(BestWe, Cur, sizeof(unsigned int)* 3);
		memcpy(BestSeq, CurSeq, 13);
	}
	int xx = 0;
	for (std::list<BYTE>::iterator ito = us.begin(); ito != us.end(); ito++, xx++){
		card.push_back(*ito);
		CurSeq[inde[xx]] = 0;
	}
}

void CGameLogic::ForceSelectCard(BYTE cards[], unsigned int we[]){
	std::list<BYTE> card;
	BYTE BestSeq[13] {};
	BYTE CurSeq[13] {};
	for (int i = 0; i < 13; i++){
		card.push_back(cards[i]);
	}
	memset(we, 0, sizeof(unsigned int)* 3);

	BestSequence(card, we, 1, 9, CurSeq, BestSeq, we);
	memcpy(cards, BestSeq, 13);
}

bool GetBitCount(int x,int pos[],int req){
	int count = 0, index = 0;
	int in = 0;
	while (x){
		if (x % 2 == 1){ 
			count++;
			pos[in++] = index;
		}
		x = x >> 1;
		index++;
		if (count == req && x > 0){ return false; }
	}
	if (count == req){ return true; }
	return false;
}

void CGameLogic::VeryForceSelectCard(BYTE bCards[], unsigned int we[]){
	BYTE copy[13];
	memcpy(copy, bCards, 13);
	int t1[285], t2[1287];
	unsigned int CurWe3[285], CurWe5[1287];

	int BestValSeq[3] = { 0, 0, 0 };
	unsigned int BestWe = 0;
	BYTE BestSeq[13];
	memset(BestSeq, 0, 13);
	//force t1,t2
	int in = 0;
	for (int i = 7; i < 0x1b01; i++){
		int pos[3] {};
		BYTE Se3[3] {};
		if (GetBitCount(i, pos, 3)){
			Se3[0] = copy[pos[0]];
			Se3[1] = copy[pos[1]];
			Se3[2] = copy[pos[2]];

			t1[in] = i;
			CurWe3[in++] = CalcWeight(Se3, 3);
		}
	}

	in = 0; 
	for (int i = 31; i < 0x1f01; i++){
		int pos[5];
		BYTE Se5[5];
		if (GetBitCount(i, pos, 5)){
			Se5[0] = copy[pos[0]];
			Se5[1] = copy[pos[1]];
			Se5[2] = copy[pos[2]];
			Se5[3] = copy[pos[3]];
			Se5[4] = copy[pos[4]];

			t2[in] = i;
			CurWe5[in++] = CalcWeight(Se5, 5);
		}
	}
	//not force
	//-------------------------------------------------------------------------------------------------
	//思路适用于任何组合问题(括号)
	//思路2：pos 2 > 1即可   allva=0x1fff- t1[i] x[10]= {all pos}  int va2=1<<x[i](i>4),va1=1<<x[i](i<5);
	//int cc[5]={0,1,2,3,4};
	//选5个位置   :while(va2>va1)
	//if(cc[i]==cc[i+1])(i=0,1,2,3,4顺序)  cc[i]=i; cc[i+1]++;
	//va1+=1<<x[cc[i]];(i=0,1,2,3,4)
	//va2=allva-va1;
	//-------------------------------------------------------------------------------------------------
	
	for (int i = 0; i < 285; i++){
		int calccount = 0;
		for (int j = 0; j < 1287; j++){
			if ((t1[i] & t2[j]) == 0){
				calccount++;
				int va = 0x1fff - t1[i] - t2[j];
				int k = 0;
				for (int left = 0, right = 1286, mid = (left + right) / 2;; mid = (left + right) / 2){
					if (va == t2[mid]){
						k = mid;
						break;
					}
					else if (va == t2[left]){
						k = left;
						break;
					}
					else if (va == t2[right]){
						k = right;
						break;
					}
					else if (va < t2[mid]){
						right = mid;
					}
					else{
						left = mid;
					}
				}

				int timesj = 1, timesk = 1;
				if ((CurWe5[j] >> 24) > 7){
					timesj= 2;
				}
				if ((CurWe5[k] >> 24) > 7){
					timesk = 2;
				}
				unsigned int CountWe = CurWe5[j] * timesj + CurWe5[k] * timesk + CurWe3[i];
				if (CurWe5[j]>CurWe3[i] && CurWe5[k] > CurWe3[i] && CountWe > BestWe){
					BestWe = CountWe;
					BestValSeq[0] = t1[i];
					if (CurWe5[j] > CurWe5[k]){
						BestValSeq[1] = t2[k];
						BestValSeq[2] = t2[j];
					}
					else{
						BestValSeq[1] = t2[j];
						BestValSeq[2] = t2[k];
					}
				}
			}
			if (calccount == 126){
				break;
			}
		}
	}

	int val1 = 0, val2 = 3, val3 = 8;
	for (int i = 0; i < 13; i++){
		if (((1 << i)&BestValSeq[0]) != 0){
			BestSeq[val1++] = copy[i];
		}
		else if (((1 << i)&BestValSeq[1]) != 0){
			BestSeq[val2++] = copy[i];
		}
		else if (((1 << i)&BestValSeq[2]) != 0){
			BestSeq[val3++] = copy[i];
		}
	}
	we[0] = CalcWeight(BestSeq, 3);
	we[1] = CalcWeight(BestSeq + 3, 5);
	we[2] = CalcWeight(BestSeq + 8, 5);
	memcpy(bCards, BestSeq, 13);

}