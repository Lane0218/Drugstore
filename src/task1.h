#pragma once
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>

using namespace std;

struct Medicine
{
    int id;
    double cost;
    // 进价+加价=卖出价格，只用于第一问，第二问假定加价都是6元
    double price;
    int expiration;
    // 卖出或丢弃的时间，-1表示没被卖出或丢弃
    int sold_time;
    // 临时卖出或丢弃时间，用于测试该adjust是否合法
    int tmp_sold_time;
    Medicine() : id(-1), cost(10000), price(10000), expiration(10000), sold_time(-1), tmp_sold_time(-1) {}
} medicine[50];

struct Strategy
{
    Medicine med[10][10];
    int price_index[10][10];
} strategy;

// 第一问丢弃药品id列表
int deleted_id[20];
double price_list[7] = {-1.5, -1, -0.5, 0, 2, 4, 6};
double simulate_profit = 0;

// price_index并不总是6，加价不一样
bool cmp_greater_buy_diff(Medicine x, Medicine y)
{
    if (x.price != y.price)
        return x.price < y.price;
    return x.expiration > y.expiration;
}

// 用于测试读入的策略是否合法
// bool task1_verify()
// {
//     for(int i=0;i<10;i++)
//     {
//         for(int j=0;j<3;j++)
//         {
//             medicine[strategy.med[i][j].id].sold_time=i;
//             for(int k=j+1;k<10 && strategy.med[i][k].id!=-1;k++)
//             {
//                 if(strategy.med[i][j].id==strategy.med[i][k].id)
//                 {
//                     cout << "sold and then occur" << endl;
//                     return false;
//                 }
//             }
//             for(int m=i+1;m<10;m++)
//             {
//                 for(int n=0;n<10;n++)
//                 {
//                     if(strategy.med[i][j].id==strategy.med[m][n].id)
//                     {
//                         cout << "sold and then occur" << endl;
//                         return false;
//                     }
//                 }
//             }
//         }
//     }
//     for(int i=0;i<50;i++)
//     {
//         if(!((medicine[i].sold_time==-1 && medicine[i].expiration>=10) || (medicine[i].sold_time!=-1 && medicine[i].expiration>=medicine[i].sold_time)))
//         {
//             cout << "out of date" << endl;
//             return false;
//         }
//     }
//     return true;
// }

double simulate()
{
    for (int i = 0; i < 10; i++)
    {
        Medicine dayi[10];
        for (int j = 0; j < 10; j++)
        {
            dayi[j] = strategy.med[i][j];
            dayi[j].price = dayi[j].cost + price_list[strategy.price_index[i][j]];
            dayi[j].cost += price_list[strategy.price_index[i][j]];
        }
        sort(dayi, dayi + 10, cmp_greater_buy_diff);
        for (int j = 0; j < 3; j++)
        {
            // 卖药的利润
            simulate_profit += dayi[j].cost - medicine[dayi[j].id].cost;
            for (int k = i; k < 10; k++)
            {
                // 已卖出的药自卖出之日起产生的管理费需要补回来
                if (medicine[dayi[j].id].expiration - k - 1 < 5)
                    simulate_profit += 1;
                else
                    simulate_profit += 0.5;
            }
        }
        for (int j = 3; j < 10 && dayi[j].id != -1; j++)
        {
            // 在货架但没卖出的药本应该交的管理费
            if (dayi[j].expiration - i - 1 < 5)
                simulate_profit += 1;
            else
                simulate_profit += 0.5;
        }
    }
    int tmp = 0;
    while (deleted_id[tmp] != -1)
    {
        simulate_profit -= medicine[deleted_id[tmp]].cost;
        int delete_time = medicine[deleted_id[tmp]].sold_time;
        for (int i = delete_time; i < 10; i++)
        {
            // 已丢弃的药自丢弃之日起产生的管理费需要补回来
            if (medicine[deleted_id[tmp]].expiration - i - 1 < 5)
                simulate_profit += 1;
            else
                simulate_profit += 0.5;
        }
        tmp++;
    }
    // 收取所有药品在每一天的管理费
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (medicine[j].expiration - i - 1 < 5)
                simulate_profit -= 1;
            else
                simulate_profit -= 0.5;
        }
    }
    return simulate_profit;
}