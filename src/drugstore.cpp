#include "task1.h"
#include "task2.h"

bool read(int argc, char *argv[]);
void print(char *argv[]);

int main(int argc, char *argv[])
{
    srand(int(time(0)));
    // 初始化
    memset(deleted_id, -1, sizeof(int) * 20);
    memset(my_deleted_id, -1, sizeof(int) * 20);

    if (!read(argc, argv))
        return 1;
    simulate();
    cout << "task1:" << simulate_profit << endl;

    clear();
    deleted();
    greedy();
    greedy_profit = calculate(greedy_strategy);
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            best_strategy[i][j] = greedy_strategy[i][j];
    // 模拟退火
    for (int i = 0; i < 10; i++)
        SA();
    // 爬山
    for (int i = 0; i < 10; i++)
        climb();
    if (my_deleted_id[0] != -1)
        max_profit -= medicine[my_deleted_id[0]].cost;

    cout << "task2:" << max_profit << endl;
    print(argv);

    return 0;
}

bool read(int argc, char *argv[])
{
    // Read and format file data using command line arguments
    if (argc != 7)
    {
        cout << "Please provide command line flags and file paths as command line arguments." << endl;
        return false;
    }
    int index = 1;
    while (index < 6)
    {
        string flag = argv[index++];
        if (flag == "-m")
        {
            FILE *file1 = fopen(argv[index++], "r");
            if (!file1)
            {
                cout << "Unable to open the file." << endl;
                return false;
            }
            for (int i = 0; i < 50; i++)
            {
                fscanf(file1, "%lf\t%d\n", &medicine[i].cost, &medicine[i].expiration);
                medicine[i].id = i;
            }
            fclose(file1);
        }
        else if (flag == "-s" || flag == "-ws")
        {
            FILE *file2 = fopen(argv[index++], "r");
            if (!file2)
            {
                cout << "Unable to open the file." << endl;
                return false;
            }
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    int id;
                    fscanf(file2, "%d,%d\t", &id, &strategy.price_index[i][j]);
                    if (id != -1)
                        strategy.med[i][j] = medicine[id];
                    else
                    {
                        Medicine tmp;
                        strategy.med[i][j] = tmp;
                    }
                }
                fscanf(file2, "\n");
            }
            fclose(file2);
        }
        else if (flag == "-d" || flag == "-wd")
        {
            ifstream file3(argv[index++]);
            if (!file3)
            {
                cout << "Unable to open the file." << endl;
                return false;
            }
            int day, id, index1 = 0;
            while (file3 >> day >> id)
            {
                if (day == 10)
                    continue;
                deleted_id[index1++] = id;
                medicine[id].sold_time = day;
            }
            file3.close();
        }
        else
        {
            cout << "Invalid flag word." << endl;
            return false;
        }
    }
    return true;
}

void print(char *argv[])
{
    string filePath = string(argv[4]);
    string fileName1 = filePath.substr(0, filePath.length() - 12) + "my_strategy.txt";
    string fileName2 = filePath.substr(0, filePath.length() - 12) + "my_delete.txt";
    string fileName3 = filePath.substr(0, filePath.length() - 12) + "result.txt";

    FILE *file1 = fopen(fileName1.c_str(), "w");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
            fprintf(file1, "%d,6\t", best_strategy[i][j].id);
        fprintf(file1, "\n");
    }
    fclose(file1);

    FILE *file2 = fopen(fileName2.c_str(), "w");
    int j = 0;
    while (my_deleted_id[j] != -1)
        fprintf(file2, "0\t%d\n", my_deleted_id[j++]);
    fclose(file2);

    FILE *file3 = fopen(fileName3.c_str(), "w");
    fprintf(file3, "task1:%g\ntask2:%g\n", simulate_profit, max_profit);
    fclose(file3);
}