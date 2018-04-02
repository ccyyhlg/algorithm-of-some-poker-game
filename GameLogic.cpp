#define			NORMAL_TYPE				0				//正常牌型
#define			THREEFLUSH				1				//三同花
#define			THREESTRIGHT			2				//三顺子
#define			SIXPAIRS				3				//6对
#define			FIVEPAIRSANDTHREE		4				//5对3条
#define			FOUR_THREE				5				//4组三条
#define			ALLSAMECOLOR			6				//凑一色
#define			ALLLOW					7				//全小
#define			ALLHIGH					8				//全大
#define			THREE_BOMB				9				//三分天下(4个炸)
#define			THREE_STRIGHTFLUSH		10				//三同花顺
#define			ROYALCARD				11				//十二皇族
#define			LOWDRAGON				12				//一条龙
#define			BLACKDRAGON				13				//青龙

#define			CT_INVALIDCARD				0x00			//相公
#define			CT_HIGHCARD					0x01			//乌龙
#define			CT_ONEPAIR					0x02			//对子	
#define			CT_TWOPAIRS					0x03			//两对
#define			CT_THREE					0x04			//三张
#define			CT_STRIGHT					0x05			//顺子
#define			CT_FLUSH					0x06			//同花
#define			CT_FULLHOUSE				0x07			//葫芦
#define			CT_FOUR						0x08			//铁支（炸）
#define			CT_FLUSHSTRIGHT				0x09			//同花顺
#define			CT_THREE_FIRST				0x0a			//三冲
#define			CT_FULLHOUSE_MID			0x0b			//中墩葫芦

#define SELECTTHREE(st) for(int va1=14;va1>1;va1--){ \
for (int j = 0; j < 4; j++){\
if ((h[va1] & C34[j]) == C34[j]){\
	int alco = C34[j]; \
	std::list<BYTE> ust; \
	int col = 0; \
	while (alco){\
	if (alco % 2 != 0){ ust.push_back((col << 4) + (va1 == 14 ? 1 : va1)); } \
	col++; \
	alco = alco >> 1; \
	} \
	DeleteUsedCards(card, ust, CurSeq, Layer, st);

#define ENDSELECTTHREE(st) RecallCards(card,ust,CurSeq, Layer, st);}}}

#define SELECTPAIR(st) for(int va2=14;va2>1;va2--){ \
	for (int y = 0; y < 6; y++){\
	if ((h[va2] & C24[y]) == C24[y] && va2 != va1){\
		int allco = C24[y]; \
		std::list<BYTE> ustw; \
		int co = 0; \
		while (allco){\
		if (allco % 2 != 0){ ustw.push_back((co << 4) + (va2 == 14 ? 1 : va2)); } \
		co++; \
		allco = allco >> 1; \
		} \
		DeleteUsedCards(card, ustw, CurSeq, Layer, st);

#define ENDSELECTPAIR(st) RecallCards(card,ustw,CurSeq, Layer, st);}}}

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
