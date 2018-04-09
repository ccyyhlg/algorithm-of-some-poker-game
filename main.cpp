#include <iostream>
#include <list>
#include <time.h>
#define MAXNUM 99

bool PrimeNum(std::list<int> prime, int v){
	std::list<int>::iterator ito = prime.begin();
	while ((*ito) <= v){
		if (ito == prime.end()){ return false; }
		if ((*ito) < v){ ito++; }
		else if ((*ito) == v){ return true; }
	}
	return false;
}

bool TwoAddNumNotPrime(std::list<int> prime, int num){
	std::list<int>::iterator ito = prime.begin();
	for (;; ito++){
		int v = num - (*ito);
		if (v < (*ito)){ return true; }
		if (PrimeNum(prime, v)){ return false; }
	}
}

bool JudgeNumInList(std::list<int> &listnum, int v){
	std::list<int>::iterator ito = listnum.begin();

	while (ito != listnum.end()){
		if ((*ito) == v){
			return true;
		}
		else if ((*ito) > v){
			return false;
		}
		ito++;
	}
	return false;
}

void delete2prime(std::list<int> &addnum, std::list<int>&mulnum, std::list<int> &prime){
	std::list<int> TwoPrimeAdd;
	std::list<int> TwoPrimeMul;
	for (std::list<int>::iterator ito1 = prime.begin(); (*ito1) < MAXNUM; ito1++){
		for (std::list<int>::iterator ito2 = ito1; (*ito2) < MAXNUM; ito2++){
			TwoPrimeAdd.push_back((*ito1) + (*ito2));
			TwoPrimeMul.push_back((*ito1) * (*ito2));
		}
	}
	TwoPrimeAdd.sort();
	TwoPrimeAdd.unique();

	std::list<int>::iterator twomul;
	for (int num1 = 2; num1 < MAXNUM; num1++){
		/*bool pri = JudgeNumInList(prime, num1);
		if (pri){
			for (int num2 = num1 + 1; num2 < MAXNUM; num2 ++){
				if (!JudgeNumInList(prime, num2)){
					mulnum.push_back(num1 * num2);
				}
			}
		}
		else{
			for (int num2 = num1; num2 < MAXNUM; num2++){
				mulnum.push_back(num2 * num1);
			}
		}*/
		twomul = TwoPrimeMul.begin();
		for (int num2 = num1; num2 < MAXNUM; num2++){
			if (twomul == TwoPrimeMul.end()){ twomul--; }
			else if ((num1*num2) == (*twomul)){ twomul++; continue; }
			else if ((num1*num2)>(*twomul)){ twomul++; num2--; }
			else{ mulnum.push_back((num1*num2)); }
		}
	}
	mulnum.sort();

	std::list<int>::iterator twoadd = TwoPrimeAdd.begin();
	for (int num = 4; num < 2 * MAXNUM; num++){
		if (twoadd == TwoPrimeAdd.end()){ twoadd--; }
		if (num == (*twoadd)){ twoadd++; continue; }
		else{ addnum.push_back(num); }
	}

	std::list<int> AllPrimePos;
	twoadd = TwoPrimeAdd.begin();
	while (twoadd != TwoPrimeAdd.end()){
		for (int v1 = (*twoadd)/2; v1 > 1; v1--){
			int v2 = *twoadd - v1;
			AllPrimePos.push_back(v2*v1);
		}
		twoadd++;
	}
	AllPrimePos.sort();

	std::list<int>::iterator alp = AllPrimePos.begin();
	for (twomul = mulnum.begin(); twomul != mulnum.end();){
		if (alp == AllPrimePos.end()){ break; }
		if ((*twomul) < (*alp)){ twomul++; }
		else if ((*twomul)>(*alp)){ alp++; }
		else
		{
			twomul = mulnum.erase(twomul);
			alp++;
		}
	}

	for (std::list<int>::iterator ito1 = mulnum.begin(); ito1 != mulnum.end(); ){
		std::list<int>::iterator ito2 = ito1;
		ito2++;
		if (ito2 == mulnum.end()){ break; }
		bool same = false;
		while ((*ito1) == (*ito2)){ ito2 = mulnum.erase(ito2); same = true;}
		if (same){ ito1 = mulnum.erase(ito1); }
		else{ ito1++; }
	}
}

void OnlyOnePosInAdd(std::list<int> &addnum, std::list<int>&mulnum, std::list<int> &prime){
	std::list<int>::iterator ito = addnum.begin();

	while (ito != addnum.end()){
		int c = 0;
		int mi, ma;
		for (int i = 2; i < (*ito) / 2; i++){
			int j = (*ito) - i;
			if (JudgeNumInList(mulnum, i*j)){
				c++;
				if (c > 1){ break; }
				mi = i < j ? i : j;
				ma = i < j ? j : i;
			}
		}
		if (c == 1){ std::cout << mi << " " << ma << " " << std::endl; }
		if (c != 1){ ito = addnum.erase(ito); }
		else{ ito++; }
	}
}

int main(){
	int s[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
	std::list<int> prime;
	std::list<int> addnum;
	std::list<int> mulnum;
	for (int i = 0; i < 10; i++){ prime.push_back(s[i]); }

	/*for (int i = 4; i < 2 * MAXNUM; i++){ addnum.push_back(i); }
	for (int i = 2; i < MAXNUM; i++){
		for (int j = i; j < MAXNUM; j++){
			mulnum.push_back(i*j);
		}
	}
	mulnum.sort();
	mulnum.unique();*/

	long st, en;
	st = clock();
	for (int num = 31; num < MAXNUM*MAXNUM; num++){
		std::list<int>::iterator ito = prime.begin();
		
		bool pri = true;
		while (ito!=prime.end())
		{
			int ne = num / (*ito);
			if (ne < (*ito)){ break; }
			if (ne * (*ito) == num){
				pri = false;
				break;
			}
			ito++;
		}
		if (pri){ prime.push_back(num); }
	}
	en = clock();
	std::cout << "prime:" << en - st << std::endl;

	st = clock();
	//first sentence 
	delete2prime(addnum, mulnum, prime);
	en = clock();
	std::cout << "p1:" << en - st << std::endl;

	//st = clock();
	////second sentence
	//OnlyOnePossible(addnum, mulnum, prime);
	//en = clock();
	//std::cout << "p2:" << en - st << std::endl;

	st = clock();
	//third sentence
	OnlyOnePosInAdd(addnum, mulnum, prime);
	en = clock();
	std::cout << "p3:" << en - st << std::endl;
	/*std::list<int> first;
	first = FirstAlgothm(prime);
	std::list<int> second;
	second = SecondAlgothm(prime);

	std::list<int>::iterator ito1 = first.begin();
	std::list<int>::iterator ito2 = second.begin();

	for (int i = 4; i < MAXNUM; i++){
		if (TwoMulNumOnlyOnePos(prime, i)){
			std::cout << i << " " << std::endl;
		}
	}*/
	return 0;
}
