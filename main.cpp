#include <iostream>
#include "RNetWithMyPriorNewWithBalance.h"
#include <limits>

using namespace std;

void main()
{
	setlocale(LC_ALL, "RUSSIAN");
	int n, r;
	int narcs;
	int k;
	int i = 0;
	int q;
	float m;
	char ynmark;
	ResourceNet RN;
	bool flag;
	bool isContinue = false;

	while (true)
	{
		cout << "Введите кол-во вершин Вашей ресурсной сети: ";
		cin >> n;
		flag = RN.SetCount(n);
		if (flag)
			break;
	}
	while (true)
	{
		cout << "Введите кол-во дуг Вашей ресурсной сети: ";
		cin >> narcs;
		flag = RN.SetCountArcs(narcs);
		if (flag)
			break;
	}

	cout << "Введите дуги Вашей ресурсной сети (входная вершина, выходная вершина, пропускная способность, приоритет): " << endl;

	while (i < narcs)
	{
		cin >> r >> k >> m >> q;
		flag = RN.AddArc(r, k, m, q);
		if (flag)
			i += 1;
	}
	if (RN.isBalanced())
		cout << "Ресурсная сеть сбалансирована" << endl;
	else
	{
		cout << "Ресурсная сеть не сбалансирована. Хотите сбалансировать? (Y / N) ";
		cin >> ynmark;
		if (ynmark == 'Y')
			RN.Balance();
	}
	std::vector<float> il;
	int time;

	cout << "Введите время, по которое вы хотите получить данные (> 0): ";

	while (true)
	{
		cin >> time;
		flag = RN.SetTime(time);
		if (flag)
			break;
	}

	cout << "Задайте начальное распределение ресурса в вершинах сети: ";
	while (true)
	{
		while (il.size() < n)
		{
			cin >> m;
			il.push_back(m);
		}
		flag = RN.SetAlloc(il);
		if (flag)
			break;
		else
		{
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			il.clear();
		}
			
	}

	while (true)
	{
		RN.General(isContinue);
		cout << endl;
		cout << "Хотите продолжить? (Y / N) ";
		cin >> ynmark;
		if (ynmark == 'N')
			break;
		else
		{
			isContinue = true;
			cout << "Сколько шагов? ";
			while (true)
			{
				cin >> time;
				flag = RN.AddTime(time);
				if (flag)
					break;
			}
			cout << "Хотите изменить приоритеты вручную? (Y / N) ";
			cin >> ynmark;
			if (ynmark == 'Y')
			{
				i = 0;
				cout << "Новое распределение приоритетов: ";
				while (i < narcs)
				{
					cin >> q;
					flag = RN.ChangePrior(i, q);
					if (flag)
						i += 1;
				}
			}
			else
			{
				cout << "Хотите рандомизировать приоритеты? (Y / N) ";
				cin >> ynmark;
				if (ynmark == 'Y')
				{
					RN.randomPriors();
				}
			}
				
		}
	}
}


/*
	cout << "Введите дуги Вашей ресурсной сети (источник - приемник - пропускная способность)" << endl;
	cout << "При окончании ввода введите -1 -1 -1: " << endl;
	while (true)
	{

		cin >> n >> k >> m;
		flag = RN.AddArc(n, k, m);
		if (flag)
			break;
	}

	std::vector<float> il;
	int time;
	while (true)
	{
		cout << "Задайте начальное распределение ресурса в вершинах сети" << endl;
		cout << "При окончании ввода введите -1: " << endl;
		while (true)
		{
			cin >> m;
			if (m == -1)
				break;
			il.push_back(m);
		}
		cout << "Введите время, по которому вы хотите получить данные (> 0): " << endl;
		cin >> time;
		flag = RN.General(time, il);
		if (flag)
			break;
	}
*/
/*
RN.AddArc(1, 2, 1);
	RN.AddArc(2, 3, 1);
	RN.AddArc(3, 2, 1);
	RN.AddArc(2, 4, 1);
	RN.AddArc(3, 4, 1);
	RN.AddArc(4, 3, 1);
	RN.AddArc(4, 1, 1);
	cout << endl;
	RN.General(8, { 2, 0, 4, 2 });
*/

/*
void main()
{
	setlocale(LC_ALL, "RUSSIAN");

	cout << "Приветствие" << endl; // Написать

	int n;
	//int k;
	//float m;
	ResourceNet RN;
	bool flag;
	while (true)
	{
		cout << "Введите кол-во вершин Вашей ресурсной сети: ";
		cin >> n;
		flag = RN.SetCount(n);
		if (flag)
			break;
	}
	cout << endl;
	RN.AddArc(1, 2, 1);
	RN.AddArc(2, 3, 1);
	RN.AddArc(2, 4, 2);
	RN.AddArc(3, 4, 1);
	RN.AddArc(4, 1, 1);
	RN.AddArc(4, 2, 2);

	cout << endl;
	RN.General(14, { 1, 1, 1, 1 });
	cin >> n;
}
*/