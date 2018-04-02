bool JudgeLegal(int iCard[], int in[], int now){
	bool com = true;
	for (int i = 0; i <= now; i++){
		iCard[in[i]]--;
		if (iCard[in[i]] < 0){ com = false; }
	}
	for (int i = 0; i <= now; i++){
		iCard[in[i]]++;
	}
	return com;
}

int Times(int a, int b){
	//a==1时自己是炸  早已返回
	if (a == 2 && b == 1){ return 2; }
	if (a == 3 && b == 1){ return 3; }
	if (a == 4 && b == 1){ return 4; }
	if (a == 2 && b == 2){ return 1; }
	if (a == 3 && b == 2){ return 3; }
	if (a == 4 && b == 2){ return 6; }
	if (a == 3 && b == 3){ return 1; }
	if (a == 4 && b == 3){ return 3; }
	if (a == 4 && b == 4){ return 1; }
	return 0;
}

int AIPlayer::GetTimes(int iCards[], int in[]){
	int index1 = 1, index2 = 1;
	if (in[0] == in[1]){ index1 = 2; }
	if (in[0] == in[2]){ index1 = 3; index2 = iCards[in[0]]; }
	if (in[1] == in[2]){ index2 = 2; }

	if (index1 != 1 || index2 != 1){ return Times(iCards[in[0]], index1)*Times(iCards[in[2]], index2); }

	int ti = Times(iCards[in[0]], 1)*Times(iCards[in[1]], 1)*Times(iCards[in[2]], 1);
	//2 3 4   3 3 3 
	if (ti == 24 || ti == 27){ ti = ti - 4 + m_iColCount; }
	//2 4 4  /2种同花
	else if (ti == 32){ ti -= 2; }
	//3 3 4
	else if (ti == 36){
		if (m_iColCount == 1){ ti -= 1; }
		else if (m_iColCount == 3){ ti -= 2; }
		else { ti -= 2; m_iMinus++; }
	}
	//3 4 4
	else if (ti == 48){ ti -= 1; }
	//4 4 4
	else { ti -= 4; }
	return ti;
}

void AIPlayer::OtherPosWeightCount(int Weight, int count){
	int type = Weight >> 12;
	int maxcard = (Weight >> 8) % 16;

	OtherPlayer op = FirstTime;
	if (type == 2 && maxcard <= 10){}
	else if (type == 2 && maxcard <= 14){ op = Over10; }
	else if (type == 3 && maxcard < 5){ op = OverA; }
	else if (type == 3 && maxcard <= 13){ op = Pair5; }
	else { op = PairK; }

	for (int i = op + 1; i < AllPos; i++){
		m_iOtherCount[i] += count;
	}
}

//回溯  容斥原理
int AIPlayer::GetCount(int iCards[], int in[], int now){
	int Count = 0;
	if (now == 0){ in[now] = 1; }
	else{ in[now] = in[now - 1]; }
	for (; in[now] < 14; in[now]++){
		if (JudgeLegal(iCards, in, now)){
			if (now == 2){
				BYTE possi[3];
				possi[0] = 0x00 + in[0];
				possi[1] = 0x10 + in[1];
				possi[2] = 0x20 + in[2];
				int TemWeight = m_logic.CalcWeight(possi);
				int a = GetTimes(iCards, in);
				OtherPosWeightCount(TemWeight, a);
				if (TemWeight < m_iWeight){
					Count += a;
				}
			}
			else{
				Count += GetCount(iCards, in, now + 1);
			}
		}
	}
	return Count;
}

int AIPlayer::GetCount(bool bCards[4][14], int in[], int now, int col){
	int Count = 0;
	if (now == 0){ in[now] = 1; }
	else{ in[now] = in[now - 1] + 1; }
	for (; in[now] < 14; in[now]++){
		if (bCards[col][in[now]]){
			if (now == 2){
				BYTE possi[3];
				possi[0] = (col << 4) + in[0];
				possi[1] = (col << 4) + in[1];
				possi[2] = (col << 4) + in[2];
				if (m_logic.CalcWeight(possi) < m_iWeight){
					Count++;
				}
			}
			else{
				Count += GetCount(bCards, in, now + 1, col);
			}
		}
	}
	return Count;
}


int AIPlayer::CalcAIWinCount(){
	int count = 0;
	m_iMinus = 0;
	//可以先处理本身为豹子的情况
	if (m_iWeight > (CT_FOUR << 12)){
		int val = (m_iWeight >> 8) % 16;
		count = (14 - val) * 4;
		return MAXCOUNT - (count + 60);
	}

	//两种数据形式
	//同花需要的数据结构
	bool bCards[4][14];
	memset(bCards, 1, 56);
	int iCards[14];
	iCards[0] = 0;
	for (int i = 1; i < 14; i++){
		iCards[i] = 4;
	}
	//DeleteAIHoleData
	for (int i = 0; i < 3; i++){
		int col = m_bHoleCards[i] >> 4;
		int val = m_bHoleCards[i] & 0x0f;
		bCards[col][val] = 0;
		iCards[val]--;
	}
	//Calc HoleCards ColCount
	m_iColCount = 3;
	if ((m_bHoleCards[0] >> 4) == (m_bHoleCards[1] >> 4) ||
		(m_bHoleCards[0] >> 4) == (m_bHoleCards[2] >> 4) ||
		(m_bHoleCards[1] >> 4) == (m_bHoleCards[2] >> 4)){
		m_iColCount = 2;
	}
	else if ((m_bHoleCards[0] >> 4) == (m_bHoleCards[1] >> 4) &&
		(m_bHoleCards[0] >> 4) == (m_bHoleCards[2] >> 4)){
		m_iColCount = 1;
	}

	int in[3];
	//GetFlushCount
	in[0] = in[1] = in[2] = 1;
	if (m_iWeight > (CT_FLUSH << 12)){
		for (int co = 0; co < 4; co++){
			count += GetCount(bCards, in, 0, co);
		}
	}

	//GetOtherCount
	in[0] = in[1] = in[2] = 1;
	count += GetCount(iCards, in, 0);
	count += (m_iMinus / 3);
	return count;
}
