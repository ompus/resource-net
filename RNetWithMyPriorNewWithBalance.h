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
	// Кол-во вершин
	int count;
	// Кол-во дуг
	int countArcs;
	// Вектор дуг
	std::vector<std::tuple <int, int, float, int>> arcs;
	// Вектор для распределений ресурса в вершинах
	std::vector<std::vector<float>> v;
	// Вектор для потока ресурса в дугах
	std::vector<std::vector<float>> a;
	// Множество номеров приоритета
	std::set<float> arcs_priors;
	// Двумерный вектор индексов дуг по приоритетам
	std::vector<std::vector<int>> prior_a_ind;
	int numFirstStable = -1;
	int num = -1;
	// Начальное время
	int start = 1;
	// Суммарное время
	int time;
	//
	bool isLatestStable = false;

	// Проверка условия для кол-ва вершин и установка значения
	bool SetCount(int c)
	{
		if (c <= 0)
		{
			std::cout << "Ошибка: в ресурсной сети должна быть как минимум одна вершина!" << std::endl;
			return false;
		}
		count = c;
		return true;
	}

	// Проверка условия для кол-ва дуг и установка значения
	bool SetCountArcs(int c)
	{
		if (c <= 0)
		{
			std::cout << "Ошибка: в ресурсной сети должна быть как минимум одна дуга!" << std::endl;
			return false;
		}
		countArcs = c;
		return true;
	}

	// Проверка условия для вводимых дуг и установка значений
	bool AddArc(int source, int dest, float weight, int prior)
	{
		if (source < 1 || source > count || dest < 1 || dest > count)
		{
			std::cout << "Ошибка: дуга ведёт из / в несуществующую вершину!" << std::endl;
			return false;
		}

		if (weight < 0)
		{
			std::cout << "Ошибка: пропускная способность дуги не может быть отрицательной!" << std::endl;
			return false;
		}

		if (source == dest)
		{
			std::cout << "Ошибка: в ресурсной сети не может быть петель!" << std::endl;
			return false;
		}

		if (prior < 0)
		{
			std::cout << "Ошибка: приоритет не может быть отрицательным!" << std::endl;
			return false;
		}
		arcs.push_back(std::make_tuple(source, dest, weight, prior));
		return true;
	}

	// Проверка условия для начального распределения и установка значений
	bool SetAlloc(std::vector<float> il)
	{
		// Проверка на соответствие кол-ва вершин начальному распределению ресурса
		if (il.size() != count)
		{
			std::cout << "Ошибка: Кол-во вершин в начальном распределении ресурса не совпадает с кол-вом вершин в ресурсной сети!" << std::endl;
			return false;
		}

		// Проверка на неотрицательность ресурса в начальном распределении ресурса
		for (float x : il)
		{
			if (x < 0)
			{
				std::cout << "Ошибка: ресурс не может быть отрицательным в любой из вершин!" << std::endl;
				return false;
			}
		}
		for (int i = 0; i < count; ++i)
			v[0][i] = il[i];
		return true;
	}

	// Проверка условия для времени и установка значения
	bool SetTime(int t)
	{
		// Проверка введённого времени
		if (t < 0)
		{
			std::cout << "Ошибка: время не может быть меньше 0!" << std::endl;
			return false;
		}
		time = t;

		// Генерация таблиц для начального распределения ресурса и потока на дугах
		GenerateTable();
		return true;
	}

	// Проверка условия для времени и расширение таблицы
	bool AddTime(int t)
	{
		// Проверка введённого времени
		if (t < 0)
		{
			std::cout << "Ошибка: время не может быть меньше 0!" << std::endl;
			return false;
		}
		start = time + 1;
		time += t;

		// Расширение таблиц
		GenerateTable();
		return true;
	}

	bool ChangePrior(int index, int prior)
	{
		if (index >= arcs.size())
		{
			std::cout << "Ошибка: индекс за границами массива дуг" << std::endl;
			return false;
		}

		if (prior < 0)
		{
			std::cout << "Ошибка: приоритет не может быть отрицательным!" << std::endl;
			return false;
		}

		arcs[index] = std::make_tuple(std::get<0>(arcs[index]), std::get<1>(arcs[index]), std::get<2>(arcs[index]), prior);
		return true;
	}

	// Проверка сети на сбалансированность
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
				std::cout << "Вершина " << i + 1 << " является ";
				if (balance[i] > 0)
					std::cout << "профицитной" << std::endl;
				else
					std::cout << "дефицитной" << std::endl;
			}
				
		return flag;
	}

	// Балансирование сети
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
			AddArc(surplus[0], deficit[i], abs(balance[i]), 0); // какой приоритет?
			std::cout << "Добавлена дуга " << surplus[0] << " -> " << deficit[i] << std::endl;
		}
	}

	// Назначение дугам случайных приоритетов
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

	// Главный метод
	void General(bool isContinue)
	{
		if (!isContinue)
		{
			// Вывод нулевой строчки
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

			// Генерация двумерного вектора приоритетов по индексам среди дуг
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
			// Заполнение таблиц для времени > 0
			while (n <= time)
			{
				// Ветка для неустойчивого начального состояния 
				if (!flag)
				{
					// Ветка работает в нечётный момент времени (поток на дугах)
					if (n % 2 == 1)
					{
						int ind = 0;
						// Цикл по вершинам РС
						for (int i = 1; i <= count; ++i)
						{
							float buf = v[n - 1][i - 1];
							std::unordered_set<int> indexes;
							bool flag1 = true;
							// Просматриваем последовательно наборы дуг согласно их приоритету, от высшего к низшему
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
								// Если ресурса хватает, чтобы удовлетворить текущий набор дуг, то отдаём им требуемый ресурс
								if (buf >= sum)
									for (int x : indexes)
									{
										a[n][x] = std::get<2>(arcs[x]);
										buf -= std::get<2>(arcs[x]);
									}
								// Если нет, то выходим
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
					// Ветка работает в чётный момент времени (распределение ресурса в вершинах)
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
						// Проверка устойчивости начального состояния
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
				// Ветка для устойчивого начального состояния
				// Значения распределения ресурса и потока на дугах копируются из предыдущих итераций. 
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
				// Вывод таблиц построчно
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
			// Вывод информации о (не)устойчивости начального состояния РС
			if (flag)
			{
				if (!isLatestStable)
				{
					std::cout << "Начальное состояние ресурсной сети устойчиво, с момента времени "
						<< num << " распределение ресурса в вершинах сети постоянно" << std::endl;
					numFirstStable = num;
				}
				else
					std::cout << "Начальное состояние ресурсной сети устойчиво, с момента времени "
					<< numFirstStable << " распределение ресурса в вершинах сети постоянно" << std::endl;
			    isLatestStable = true;					
			}
			else
			{
				std::cout << "Начальное состояние ресурсной сети неустойчиво" << std::endl;
				isLatestStable = false;
				numFirstStable = -1;
			}
			arcs_priors.clear();
			prior_a_ind.clear();
		}
	}
private:
	// Генерация таблиц для начального распределения ресурса и потока на дугах
	void GenerateTable()
	{
		v.resize(time + 1, std::vector<float>(count));
		a.resize(time + 1, std::vector<float>(arcs.size()));
	}
};
























//ОПТИМИЗАЦИЯ ССЫЛКИ И CONSTЫ!!!1

/*
if (time % 2 == 0)
		{
			std::cout << "Распределение ресурса в вершинах ресурсной сети в момент времени " << time << " таково: ";
			for (float x : v[time])
				std::cout << x << " ";
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Поток ресурса на дугах ресурсной сети в момент времени " << time << " таков: ";
			for (float x : a[time])
				std::cout << x << " ";
			std::cout << std::endl;
		}
*/