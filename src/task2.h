#pragma once
#include "task1.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

// 当前的策略
Medicine cur_strategy[10][10];
// 最优的策略
Medicine best_strategy[10][10];
// 贪心法求出的策略
Medicine greedy_strategy[10][10];
// 贪心法删除策略（经测试发现只有data9和10需要删除1个药品）
int my_deleted_id[20];
double max_profit = -10000;
double greedy_profit;

// 清除对medicine的sold_time的影响
void clear()
{
    for (int i = 0; i < 50; i++)
        medicine[i].sold_time = -1;
}

// 确保medicine的sold_time与tmp_sold_time相同
void update()
{
    for (int i = 0; i < 50; i++)
        medicine[i].tmp_sold_time = medicine[i].sold_time;
}

// 加价都是序号6，所以不考虑加价，直接用进价排序
bool cmp_greater_buy(Medicine x, Medicine y)
{
    if (x.cost != y.cost)
        return x.cost < y.cost;
    return x.expiration > y.expiration;
}

// 贪心法求解丢弃药品序号时要求丢弃快过期的，且价格低的药品
bool cmp_greater_delete(Medicine x, Medicine y)
{
    if (x.expiration != y.expiration)
        return x.expiration < y.expiration;
    return x.cost > y.cost;
}

// 贪心法排序上架药品，要求先卖快过期的，且价格低的药品
bool cmp_greater_greedy(Medicine x, Medicine y)
{
    if (x.expiration != y.expiration)
        return x.expiration < y.expiration;
    return x.cost < y.cost;
}

// 生成[0,1]浮点数
double frand()
{
    return (double)rand() / RAND_MAX;
}

// 贪心法丢弃药品
void deleted()
{
    Medicine tmp_medicine[50];
    for (int i = 0; i < 50; i++)
        tmp_medicine[i] = medicine[i];
    sort(tmp_medicine, tmp_medicine + 50, cmp_greater_delete);

    int index1 = 0, index2 = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            while (tmp_medicine[index1].expiration - i - 1 < 0)
            {
                medicine[tmp_medicine[index1].id].sold_time = 0;
                my_deleted_id[index2++] = tmp_medicine[index1].id;
                index1++;
            }
            index1++;
        }
    }
}

// 求解贪心法策略
void greedy()
{
    Medicine tmp_medicine[50];
    for (int i = 0; i < 50; i++)
        tmp_medicine[i] = medicine[i];
    sort(tmp_medicine, tmp_medicine + 50, cmp_greater_greedy);
    int index1 = 0;
    for (int i = 0; i < 10; i++)
    {
        double max_cost = 0;
        for (int j = 0; j < 3; j++)
        {
            while (medicine[tmp_medicine[index1].id].sold_time != -1)
                index1++;
            medicine[tmp_medicine[index1].id].sold_time = i;
            greedy_strategy[i][j] = tmp_medicine[index1];
            if (tmp_medicine[index1].cost > max_cost)
                max_cost = tmp_medicine[index1].cost;
            index1++;
        }
        int index2 = index1;
        for (int j = 3; j < 10; j++)
        {
            // 如果药已被丢弃或价格低于待售药品的最高价
            while (index2 < 50 && (tmp_medicine[index2].cost <= max_cost || medicine[tmp_medicine[index2].id].sold_time != -1))
                index2++;
            if (index2 < 50)
                greedy_strategy[i][j] = tmp_medicine[index2++];
            else
            {
                // 若找不到价格比待售药品高的药品，货架未满位置的药品置为-1
                Medicine tmp_med;
                greedy_strategy[i][j] = tmp_med;
            }
        }
    }
}

// 计算在strategy策略下的收益（不含丢弃药品的开销）
double calculate(Medicine strategy[10][10])
{
    double profit = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // 卖药的利润
            profit += 6;
            for (int k = i; k < 10; k++)
            {
                // 已卖出的药自卖出之日起产生的管理费需要补回来
                if (medicine[strategy[i][j].id].expiration - k - 1 < 5)
                    profit += 1;
                else
                    profit += 0.5;
            }
        }
        for (int j = 3; j < 10 && strategy[i][j].id != -1; j++)
        {
            // 在货架但没卖出的药本应该交的管理费
            if (strategy[i][j].expiration - i - 1 < 5)
                profit += 1;
            else
                profit += 0.5;
        }
    }
    int tmp = 0;
    while (my_deleted_id[tmp] != -1)
    {
        int delete_time = medicine[my_deleted_id[tmp]].sold_time;
        for (int i = delete_time; i < 10; i++)
        {
            // 已丢弃的药自丢弃之日起产生的管理费需要补回来
            if (medicine[my_deleted_id[tmp]].expiration - i - 1 < 5)
                profit += 1;
            else
                profit += 0.5;
        }
        tmp++;
    }
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (medicine[j].expiration - i - 1 < 5)
                profit -= 1;
            else
                profit -= 0.5;
        }
    }
    if (profit > max_profit)
    {
        max_profit = profit;
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                best_strategy[i][j] = strategy[i][j];
    }
    return profit;
}

// 判断交换后的临时策略是否合法
bool judge(Medicine cur[10][10], Medicine tmp[10][10])
{
    for (int i = 0; i < 10; i++)
        sort(tmp[i], tmp[i] + 10, cmp_greater_buy);
    // 判断货架上有无至少3个药
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (tmp[i][j].id == -1)
                return false;
        }
    }
    // 判断有无已经卖出，第二天又出现的
    // 判断有无同一天内有两个相同的药
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = j + 1; k < 10 && tmp[i][k].id != -1; k++)
            {
                if (tmp[i][j].id == tmp[i][k].id)
                    return false;
            }
            for (int m = i + 1; m < 10; m++)
            {
                for (int n = 0; n < 10; n++)
                {
                    if (tmp[i][j].id == tmp[m][n].id)
                        return false;
                }
            }
        }
        for (int j = 3; j < 10; j++)
        {
            for (int k = j + 1; k < 10 && tmp[i][k].id != -1; k++)
            {
                if (tmp[i][j].id == tmp[i][k].id)
                    return false;
            }
        }
    }
    // 判断有无按原来的策略在这一天要卖的药，调整后排在这一天第三名以后，卖不了了，要求保质期不低于10天
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 3; k < 10; k++)
            {
                if (cur[i][j].id == tmp[i][k].id)
                {
                    if (cur[i][j].expiration < 10)
                        return false;
                }
            }
        }
    }
    // 判断是否过期
    for (int i = 0; i < 50; i++)
    {
        if (!((medicine[i].tmp_sold_time == -1 && medicine[i].expiration >= 10) || (medicine[i].tmp_sold_time != -1 && medicine[i].expiration > medicine[i].tmp_sold_time)))
            return false;
    }
    return true;
}

// 进行模拟退火
void SA()
{
    clear();
    if (my_deleted_id[0] != -1)
        medicine[my_deleted_id[0]].sold_time = 0;
    double T = 100000;
    double alpha = 0.99;
    double last_profit = greedy_profit;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
            cur_strategy[i][j] = greedy_strategy[i][j];
    }

    while (T > 0.000001)
    {
        T *= alpha;
        int opt = rand() % 4;
        double float_random=frand();

        if (opt == 0)
        {
            int day1 = rand() % 10;
            int on_sale_index1 = rand() % 10;
            int medicine_index = rand() % 50;

            // 选中的药已经被卖或被丢弃
            if (medicine[medicine_index].sold_time != -1 && day1 >= medicine[medicine_index].sold_time)
                continue;

            // 选中的药已经过期
            if (day1 >= medicine[medicine_index].expiration)
                continue;

            // 换走的药保质期不足10天
            if (on_sale_index1 < 3 && cur_strategy[day1][on_sale_index1].expiration < 10)
                continue;

            // 初始化tmp_strategy
            Medicine tmp_strategy[10][10];
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                    tmp_strategy[i][j] = cur_strategy[i][j];
            }

            for (int i = 0; i < 3; i++)
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = -1;
            tmp_strategy[day1][on_sale_index1] = medicine[medicine_index];
            medicine[tmp_strategy[day1][on_sale_index1].id].tmp_sold_time = -1;
            sort(tmp_strategy[day1], tmp_strategy[day1] + 10, cmp_greater_buy);
            for (int i = 0; i < 3; i++)
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = day1;

            if (!judge(cur_strategy, tmp_strategy))
                continue;

            double tmp_profit = calculate(tmp_strategy);
            double delta = tmp_profit - last_profit;

            // 如果符合模拟退火条件，则进行交换
            if (delta > 0 || exp(delta / T) > float_random)
            {
                last_profit = tmp_profit;
                for (int i = 0; i < 3; i++)
                    medicine[cur_strategy[day1][i].id].sold_time = -1;
                cur_strategy[day1][on_sale_index1] = medicine[medicine_index];
                sort(cur_strategy[day1], cur_strategy[day1] + 10, cmp_greater_buy);
                for (int i = 0; i < 3; i++)
                    medicine[cur_strategy[day1][i].id].sold_time = day1;
                update();
            }
        }
        else
        {
            int day1 = rand() % 10;
            int on_sale_index1 = rand() % 10;
            int day2 = rand() % 10;
            int on_sale_index2 = rand() % 10;

            int a = min(day1, day2), b = max(day1, day2);
            day1 = a, day2 = b;

            // 原来在day1的药换到day2后已过期
            if (cur_strategy[day1][on_sale_index1].expiration <= day2)
                continue;

            // 原来在day1的药换到day2后已在day2前卖出
            if (medicine[cur_strategy[day1][on_sale_index1].id].sold_time <= day2)
                continue;

            // 初始化tmp_strategy
            Medicine tmp_strategy[10][10];
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                    tmp_strategy[i][j] = cur_strategy[i][j];
            }

            for (int i = 0; i < 3; i++)
            {
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = -1;
                medicine[tmp_strategy[day2][i].id].tmp_sold_time = -1;
            }
            swap(tmp_strategy[day1][on_sale_index1], tmp_strategy[day2][on_sale_index2]);
            sort(tmp_strategy[day1], tmp_strategy[day1] + 10, cmp_greater_buy);
            sort(tmp_strategy[day2], tmp_strategy[day2] + 10, cmp_greater_buy);
            for (int i = 0; i < 3; i++)
            {
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = day1;
                medicine[tmp_strategy[day2][i].id].tmp_sold_time = day2;
            }

            if (!judge(cur_strategy, tmp_strategy))
                continue;

            double tmp_profit = calculate(tmp_strategy);
            double delta = tmp_profit - last_profit;

            // 如果符合模拟退火条件，则进行交换
            if (delta > 0 || exp(delta / T) > float_random)
            {
                last_profit = tmp_profit;
                for (int i = 0; i < 3; i++)
                {
                    medicine[cur_strategy[day1][i].id].sold_time = -1;
                    medicine[cur_strategy[day2][i].id].sold_time = -1;
                }
                swap(cur_strategy[day1][on_sale_index1], cur_strategy[day2][on_sale_index2]);
                sort(cur_strategy[day1], cur_strategy[day1] + 10, cmp_greater_buy);
                sort(cur_strategy[day2], cur_strategy[day2] + 10, cmp_greater_buy);
                for (int i = 0; i < 3; i++)
                {
                    medicine[cur_strategy[day1][i].id].sold_time = day1;
                    medicine[cur_strategy[day2][i].id].sold_time = day2;
                }
                update();
            }
        }
    }
}

// 在所得解附近爬山
void climb()
{
    clear();
    medicine[my_deleted_id[0]].sold_time = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
            medicine[best_strategy[i][j].id].sold_time = i;
        for (int j = 0; j < 10; j++)
            cur_strategy[i][j] = best_strategy[i][j];
    }
    update();
    double last_profit = max_profit;
    for (int i1 = 0; i1 < 100000; i1++)
    {
        int opt = rand() % 4;
        if (opt == 0)
        {
            int day1 = rand() % 10;
            int on_sale_index1 = rand() % 10;
            int medicine_index = rand() % 50;

            // 选中的药已经被卖或被丢弃
            if (medicine[medicine_index].sold_time != -1 && day1 >= medicine[medicine_index].sold_time)
                continue;

            // 选中的药已经过期
            if (day1 > medicine[medicine_index].expiration)
                continue;

            // 换走的药保质期不足10天
            if (on_sale_index1 < 3 && cur_strategy[day1][on_sale_index1].expiration < 10)
                continue;

            // 初始化tmp_strategy
            Medicine tmp_strategy[10][10];
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                    tmp_strategy[i][j] = cur_strategy[i][j];
            }

            for (int i = 0; i < 3; i++)
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = -1;
            tmp_strategy[day1][on_sale_index1] = medicine[medicine_index];
            medicine[tmp_strategy[day1][on_sale_index1].id].tmp_sold_time = -1;
            sort(tmp_strategy[day1], tmp_strategy[day1] + 10, cmp_greater_buy);
            for (int i = 0; i < 3; i++)
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = day1;

            if (!judge(cur_strategy, tmp_strategy))
                continue;

            double tmp_profit = calculate(tmp_strategy);
            if (tmp_profit >= last_profit)
            {
                last_profit = tmp_profit;
                for (int i = 0; i < 3; i++)
                    medicine[cur_strategy[day1][i].id].sold_time = -1;
                cur_strategy[day1][on_sale_index1] = medicine[medicine_index];
                sort(cur_strategy[day1], cur_strategy[day1] + 10, cmp_greater_buy);
                for (int i = 0; i < 3; i++)
                    medicine[cur_strategy[day1][i].id].sold_time = day1;
                update();
            }
        }
        else
        {
            int day1 = rand() % 10;
            int on_sale_index1 = rand() % 10;
            int day2 = rand() % 10;
            int on_sale_index2 = rand() % 10;

            int a = min(day1, day2), b = max(day1, day2);
            day1 = a, day2 = b;

            // 原来在day1的药换到day2后已过期
            if (cur_strategy[day1][on_sale_index1].expiration <= day2)
                continue;

            // 原来在day1的药换到day2后已在day2前卖出
            if (medicine[cur_strategy[day1][on_sale_index1].id].sold_time <= day2)
                continue;

            // 初始化tmp_strategy
            Medicine tmp_strategy[10][10];
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                    tmp_strategy[i][j] = cur_strategy[i][j];
            }

            for (int i = 0; i < 3; i++)
            {
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = -1;
                medicine[tmp_strategy[day2][i].id].tmp_sold_time = -1;
            }
            swap(tmp_strategy[day1][on_sale_index1], tmp_strategy[day2][on_sale_index2]);
            sort(tmp_strategy[day1], tmp_strategy[day1] + 10, cmp_greater_buy);
            sort(tmp_strategy[day2], tmp_strategy[day2] + 10, cmp_greater_buy);
            for (int i = 0; i < 3; i++)
            {
                medicine[tmp_strategy[day1][i].id].tmp_sold_time = day1;
                medicine[tmp_strategy[day2][i].id].tmp_sold_time = day2;
            }

            if (!judge(cur_strategy, tmp_strategy))
                continue;

            double tmp_profit = calculate(tmp_strategy);
            if (tmp_profit >= last_profit)
            {
                last_profit = tmp_profit;
                for (int i = 0; i < 3; i++)
                {
                    medicine[cur_strategy[day1][i].id].sold_time = -1;
                    medicine[cur_strategy[day2][i].id].sold_time = -1;
                }
                swap(cur_strategy[day1][on_sale_index1], cur_strategy[day2][on_sale_index2]);
                sort(cur_strategy[day1], cur_strategy[day1] + 10, cmp_greater_buy);
                sort(cur_strategy[day2], cur_strategy[day2] + 10, cmp_greater_buy);
                for (int i = 0; i < 3; i++)
                {
                    medicine[cur_strategy[day1][i].id].sold_time = day1;
                    medicine[cur_strategy[day2][i].id].sold_time = day2;
                }
                update();
            }
        }
    }
}
