#pragma once
#include <iostream>
#include <tuple>
#include <vector>
#include <set>
#include <unordered_set>
#include <iomanip>
#include <initializer_list>

class ResourceNet
{
	public:
	// ���-�� ������
	int count;
	// ���-�� ���
	int countArcs;
	// ������ ���
	std::vector<std::tuple <int, int, float, int>> arcs;
	// ������ ��� ������������� ������� � ��������
	std::vector<std::vector<float>> v;
	// ������ ��� ������ ������� � �����
	std::vector<std::vector<float>> a;
	// ��������� ������� ����������
	std::set<float> arcs_priors;
	// ��������� ������ �������� ��� �� �����������
	std::vector<std::vector<int>> prior_a_ind;
	int numFirstStable = -1;
	int num = -1;
	// ��������� �����
	int start = 1;
	// ��������� �����
	int time;
	//
	bool isLatestStable = false;

	// �������� ������� ��� ���-�� ������ � ��������� ��������
	bool SetCount(int c)
	{
		if (c <= 0)
		{
			std::cout << "������: � ��������� ���� ������ ���� ��� ������� ���� �������!" << std::endl;
			return false;
		}
		count = c;
		return true;
	}

	// �������� ������� ��� ���-�� ��� � ��������� ��������
	bool SetCountArcs(int c)
	{
		if (c <= 0)
		{
			std::cout << "������: � ��������� ���� ������ ���� ��� ������� ���� ����!" << std::endl;
			return false;
		}
		countArcs = c;
		return true;
	}

	// �������� ������� ��� �������� ��� � ��������� ��������
	bool AddArc(int source, int dest, float weight, int prior)
	{
		if (source < 1 || source > count || dest < 1 || dest > count)
		{
			std::cout << "������: ���� ���� �� / � �������������� �������!" << std::endl;
			return false;
		}

		if (weight < 0)
		{
			std::cout << "������: ���������� ����������� ���� �� ����� ���� �������������!" << std::endl;
			return false;
		}

		if (source == dest)
		{
			std::cout << "������: � ��������� ���� �� ����� ���� ������!" << std::endl;
			return false;
		}

		if (prior < 0)
		{
			std::cout << "������: ��������� �� ����� ���� �������������!" << std::endl;
			return false;
		}
		arcs.push_back(std::make_tuple(source, dest, weight, prior));
		return true;
	}

	// �������� ������� ��� ���������� ������������� � ��������� ��������
	bool SetAlloc(std::vector<float> il)
	{
		// �������� �� ������������ ���-�� ������ ���������� ������������� �������
		if (il.size() != count)
		{
			std::cout << "������: ���-�� ������ � ��������� ������������� ������� �� ��������� � ���-��� ������ � ��������� ����!" << std::endl;
			return false;
		}

		// �������� �� ����������������� ������� � ��������� ������������� �������
		for (float x : il)
		{
			if (x < 0)
			{
				std::cout << "������: ������ �� ����� ���� ������������� � ����� �� ������!" << std::endl;
				return false;
			}
		}
		for (int i = 0; i < count; ++i)
			v[0][i] = il[i];
		return true;
	}

	// �������� ������� ��� ������� � ��������� ��������
	bool SetTime(int t)
	{
		// �������� ��������� �������
		if (t < 0)
		{
			std::cout << "������: ����� �� ����� ���� ������ 0!" << std::endl;
			return false;
		}
		time = t;

		// ��������� ������ ��� ���������� ������������� ������� � ������ �� �����
		GenerateTable();
		return true;
	}

	// �������� ������� ��� ������� � ���������� �������
	bool AddTime(int t)
	{
		// �������� ��������� �������
		if (t < 0)
		{
			std::cout << "������: ����� �� ����� ���� ������ 0!" << std::endl;
			return false;
		}
		start = time + 1;
		time += t;

		// ���������� ������
		GenerateTable();
		return true;
	}

	bool ChangePrior(int index, int prior)
	{
		if (index >= arcs.size())
		{
			std::cout << "������: ������ �� ��������� ������� ���" << std::endl;
			return false;
		}

		if (prior < 0)
		{
			std::cout << "������: ��������� �� ����� ���� �������������!" << std::endl;
			return false;
		}

		arcs[index] = std::make_tuple(std::get<0>(arcs[index]), std::get<1>(arcs[index]), std::get<2>(arcs[index]), prior);
		return true;
	}

	// �������� ���� �� ������������������
	bool isBalanced()
	{
		std::vector<float> balance(count);

		for (int j = 0; j < countArcs; ++j)
		{
			balance[std::get<0>(arcs[j]) - 1] += std::get<2>(arcs[j]);
			balance[std::get<1>(arcs[j]) - 1] -= std::get<2>(arcs[j]);
		}

		bool flag = true;
		for (int i = 0; i < count; ++i)
			if (balance[i] != 0)
			{
				flag = false;
				std::cout << "������� " << i + 1 << " �������� ";
				if (balance[i] > 0)
					std::cout << "�����������" << std::endl;
				else
					std::cout << "����������" << std::endl;
			}
				
		return flag;
	}

	// �������������� ����
	void Balance()
	{
		std::vector<float> balance(count);
		std::vector<int> surplus;
		std::vector<int> deficit;

		for (int j = 0; j < countArcs; ++j)
		{
			balance[std::get<0>(arcs[j]) - 1] += std::get<2>(arcs[j]);
			balance[std::get<1>(arcs[j]) - 1] -= std::get<2>(arcs[j]);
		}

		for (int i = 0; i < count; ++i)
		{
			if (balance[i] < 0)
				surplus.push_back(i + 1);
			if (balance[i] > 0)
				deficit.push_back(i + 1);
		}

		for (int i = 0; i < deficit.size(); ++i)
		{
			AddArc(surplus[0], deficit[i], abs(balance[i]), 0); // ����� ���������?
			std::cout << "��������� ���� " << surplus[0] << " -> " << deficit[i] << std::endl;
		}
	}

	// ���������� ����� ��������� �����������
	void randomPriors()
	{
		std::vector<std::vector<int>> arcsFromVerts(count);
		for (int i = 0; i < countArcs; ++i)
			arcsFromVerts[std::get<0>(arcs[i]) - 1].push_back(i);

		for (int i = 0; i < count; ++i)
			if (arcsFromVerts[i].size() > 1)
				for (int j = 0; j < arcsFromVerts[i].size(); ++j)
					arcs[arcsFromVerts[i][j]] = std::make_tuple(std::get<0>(arcs[arcsFromVerts[i][j]]), 
						std::get<1>(arcs[arcsFromVerts[i][j]]), std::get<2>(arcs[arcsFromVerts[i][j]]), rand() % arcsFromVerts[i].size());
	}

	// ������� �����
	void General(bool isContinue)
	{
		if (!isContinue)
		{
			// ����� ������� �������
			std::cout << "  0 | ";
			for (float x : v[0])
				std::cout << std::setw(3) << x << " ";
			for (float x : a[0])
				std::cout << std::setw(4) << "- ";
			std::cout << std::endl;
		}

		if (time > 0)
		{
			bool flag = false;
			int n = start;

			// ��������� ���������� ������� ����������� �� �������� ����� ���
			for (int i = 0; i < arcs.size(); ++i)
				arcs_priors.insert(std::get<3>(arcs[i]));
			prior_a_ind.resize(arcs_priors.size());
			for (int i = 0; i < arcs.size(); ++i)
			{
				int k = 0;
				for (int x: arcs_priors)
				{
					if (std::get<3>(arcs[i]) == x)
						prior_a_ind[k].push_back(i);
					++k;
				}
			}
			// ���������� ������ ��� ������� > 0
			while (n <= time)
			{
				// ����� ��� ������������� ���������� ��������� 
				if (!flag)
				{
					// ����� �������� � �������� ������ ������� (����� �� �����)
					if (n % 2 == 1)
					{
						int ind = 0;
						// ���� �� �������� ��
						for (int i = 1; i <= count; ++i)
						{
							float buf = v[n - 1][i - 1];
							std::unordered_set<int> indexes;
							bool flag1 = true;
							// ������������� ��������������� ������ ��� �������� �� ����������, �� ������� � �������
							for (ind = 0; ind < prior_a_ind.size(); ++ind)
							{
								float sum = 0;
								indexes.clear();
								for (int j = 0; j < prior_a_ind[ind].size(); ++j)
								{
									if (std::get<0>(arcs[prior_a_ind[ind][j]]) == i)
									{
										indexes.insert(prior_a_ind[ind][j]);
										sum += std::get<2>(arcs[prior_a_ind[ind][j]]);
									}
								}
								// ���� ������� �������, ����� ������������� ������� ����� ���, �� ����� �� ��������� ������
								if (buf >= sum)
									for (int x : indexes)
									{
										a[n][x] = std::get<2>(arcs[x]);
										buf -= std::get<2>(arcs[x]);
									}
								// ���� ���, �� �������
								else
								{
									for (int x : indexes)
										a[n][x] = (std::get<2>(arcs[x]) / sum) * buf;
									buf = 0;
									break;
								}
							}
						}
					}
					// ����� �������� � ������ ������ ������� (������������� ������� � ��������)
					else
					{
						for (int i = 1; i <= count; ++i)
						{
							float sum1 = 0;
							float sum2 = 0;
							for (int j = 0; j < arcs.size(); ++j)
							{
								if (std::get<0>(arcs[j]) == i)
									sum1 += a[n - 1][j];
								if (std::get<1>(arcs[j]) == i)
									sum2 += a[n - 1][j];
							}
							v[n][i - 1] = v[n - 2][i - 1] - sum1 + sum2;
						}
						// �������� ������������ ���������� ���������
						bool f1 = true;
						for (int i = 0; i < count; ++i)
						{
							if (v[n][i] != v[n - 2][i])
							{
								f1 = false;
								break;
							}
						}
						if (f1)
						{
							num = n - 2;
							flag = true;
						}
							
					}
				}
				// ����� ��� ����������� ���������� ���������
				// �������� ������������� ������� � ������ �� ����� ���������� �� ���������� ��������. 
				else
				{
					if (n % 2 == 1)
					{
						for (int i = 0; i < arcs.size(); ++i)
							a[n][i] = a[n - 2][i];
					}
					else
					{
						for (int i = 0; i < count; ++i)
							v[n][i] = v[n - 2][i];
					}
				}
				// ����� ������ ���������
				std::cout << std::setw(3) << n << " | ";
				if (n % 2 == 0)
				{
					for (float x : v[n])
						std::cout << std::setw(3) << x << " ";
					for (float x : a[n])
						std::cout << std::setw(4) << "- ";
				}
				else
				{
					for (float x : v[n])
						std::cout << std::setw(4) << "- ";
					for (float x : a[n])
						std::cout << std::setw(3) << x << " ";
				}
				std::cout << std::endl;
				++n;
			}
			// ����� ���������� � (��)������������ ���������� ��������� ��
			if (flag)
			{
				if (!isLatestStable)
				{
					std::cout << "��������� ��������� ��������� ���� ���������, � ������� ������� "
						<< num << " ������������� ������� � �������� ���� ���������" << std::endl;
					numFirstStable = num;
				}
				else
					std::cout << "��������� ��������� ��������� ���� ���������, � ������� ������� "
					<< numFirstStable << " ������������� ������� � �������� ���� ���������" << std::endl;
			    isLatestStable = true;					
			}
			else
			{
				std::cout << "��������� ��������� ��������� ���� �����������" << std::endl;
				isLatestStable = false;
				numFirstStable = -1;
			}
			arcs_priors.clear();
			prior_a_ind.clear();
		}
	}
private:
	// ��������� ������ ��� ���������� ������������� ������� � ������ �� �����
	void GenerateTable()
	{
		v.resize(time + 1, std::vector<float>(count));
		a.resize(time + 1, std::vector<float>(arcs.size()));
	}
};
























//����������� ������ � CONST�!!!1

/*
if (time % 2 == 0)
		{
			std::cout << "������������� ������� � �������� ��������� ���� � ������ ������� " << time << " ������: ";
			for (float x : v[time])
				std::cout << x << " ";
			std::cout << std::endl;
		}
		else
		{
			std::cout << "����� ������� �� ����� ��������� ���� � ������ ������� " << time << " �����: ";
			for (float x : a[time])
				std::cout << x << " ";
			std::cout << std::endl;
		}
*/