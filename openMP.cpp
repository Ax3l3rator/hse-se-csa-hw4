// c++ main.cpp -fopenmp
#include <omp.h>
#include <unistd.h>

#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::exception;
using std::string;
using std::stringstream;
using std::vector;

struct Chatter {
    int status;
    int from;
    int to;
};

int main(int argc, char *argv[]) {
    int n;
    bool run = true;
    if (argc == 1) {
        cout << "Введите кол-во болтунов";
        cin >> n;
        if (n < 2) {
            cout << "[Ошибка] Неверное колтичество болтунов\n";
            return 0;
        }
    } else {
        try {
            n = atoi(argv[1]);
        } catch (exception e) {
            cout << "[Ошибка] Неверное колтичество болтунов\n";
            return 0;
        }
        if (n < 2) {
            cout << "[Ошибка] Неверное колтичество болтунов\n";
            return 0;
        }
    }

    vector<Chatter> chatters = vector<Chatter>(n);

    cout << "[Информация] Запускается симуляция " << n << " болтунов\n";
    cout << "[Информация] Введите 's' для прекращения симуляции\n";
    sleep(1);
    int threads = n * 2 + 2;
    int value = 0;
    for (int i = 0; i < n; i++) {
        chatters[i] = Chatter{-1, -1, 0};
    }
// Распараллеливание
#pragma omp parallel num_threads(threads)
    {
        auto id = omp_get_thread_num();

        if (id < n) {  // Вызывающий поток
            cout << "[Поток] Вызывающий поток " << id << " инициализирован\n";
            while (run) {
                int pos = rand() % n;
                while (pos == id) {
                    pos = rand() % n;
                }
#pragma omp critical  // Звоним
                {
                    if (chatters[pos].status == 0 && chatters[id].status == 0) {
                        chatters[pos].from = id;
                        chatters[pos].to = pos;
                        chatters[pos].status = 1;
                        chatters[id].from = id;
                        chatters[id].to = pos;
                        chatters[id].status = 2;
                    }
                }
                sleep(rand() % 5);
            }

        } else if (id < 2 * n) {  // Принимаюзие потоки
            int pos = id - n;
            cout << "[Поток] Принимающий поток " << id << " инициализирован\n";
            while (run) {
#pragma omp critical  // начало вызова
                {
                    if (chatters[pos].status == 1) {
                        cout << "[Начал] " << chatters[pos].from << " -> " << chatters[pos].to
                             << '\n';
                        chatters[pos].status = 3;
                        chatters[chatters[pos].from].status = 3;
                    }
                }
                sleep(rand() % 5);
#pragma omp critical  // Завершение вызова
                {
                    if (chatters[pos].status == 3) {
                        cout << "[Закончил] " << chatters[pos].from << " -> " << chatters[pos].to
                             << '\n';
                        chatters[chatters[pos].from].from = -1;
                        chatters[chatters[pos].from].to = -1;
                        chatters[chatters[pos].from].status = 0;
                        chatters[pos].from = -1;
                        chatters[pos].to = -1;
                        chatters[pos].status = 0;
                    }
                }
                sleep(5);
            }
        } else if (id < 2 * n + 1) {  // Наблюдаем
            cout << "[Поток] поток-наблюдатель инициализирован" << '\n';
            while (run) {
#pragma omp critical  // Выводим состояние болтунов
                {
                    cout << "[Состояние] ";
                    for (int i = 0; i < chatters.size(); i++) {
                        cout << "(" << chatters[i].to << " " << chatters[i].from << " "
                             << chatters[i].status << ") ";
                    }
                    cout << '\n';
                    cout.flush();
                }
                sleep(3);
            }
        } else {  // ждем ввода 's' для выхода
            cout << "[Поток] поток-завершитель инициализирован" << '\n';
            while (cin.get() != 's') {
            }
            run = false;
            cout << "[Информация] Ждем завершения слипов\n";
        }
    }
    cout << "[Информация] Симуляция завершается\n";
    return 0;
}