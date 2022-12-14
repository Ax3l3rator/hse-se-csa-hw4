#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

using std::cin;
using std::cout;
using std::exception;
using std::string;
using std::stringstream;
using std::vector;

// status: 0 - свободен; 1 - вызывает; 2 - принимает; 3 - занят
// from: от кого принимается выщов
// to: кому звонок

struct Chatter {
    int status;
    int from;
    int to;
};

// mutex: глобальный мьютекс
// id: id потока
// count: количество потоков

struct ArgStruct {
    pthread_mutex_t mutex;
    int id;
    int count;
};

// Глобальный массив болтунов
static vector<Chatter> chatters;
static vector<string> logs;
static bool run = true;
static bool is_file_input = false;

// потоковая функция для совершения вызовов
void *callto(void *args) {
    ArgStruct *arg = (ArgStruct *)args;
    int count = arg->count;
    pthread_mutex_t mutex = arg->mutex;
    int id = arg->id;

    pthread_mutex_lock(&mutex);
    if (is_file_input) {
        stringstream ss;
        ss << "[Поток] Вызывающий поток " << id << "инициализирован\n";
        logs.push_back(ss.str());
    }
    cout << "[Поток] Вызывающий поток " << id << "инициализирован\n";
    cout.flush();

    pthread_mutex_unlock(&mutex);

    while (true) {
        // получаем рандмоный номер болтуна
        int index = rand() % count;
        while (index == id) {
            index = rand() % count;
        }
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // если оба свободны, соединяем
        if (chatters[id].status == 0 && chatters[index].status == 0) {
            chatters[index].from = id;
            chatters[index].to = index;
            chatters[index].status = 1;
            chatters[id].to = index;
            chatters[id].from = id;
            chatters[id].status = 2;
            pthread_mutex_unlock(&mutex);
            sleep(rand() % 10);
        } else {
            pthread_mutex_unlock(&mutex);
            sleep(5);
        }
    }

    return nullptr;
}

// Потоковая функция ответов
void *getcall(void *args) {
    ArgStruct *arg = (ArgStruct *)args;
    int count = arg->count;
    pthread_mutex_t mutex = arg->mutex;
    int id = arg->id;

    pthread_mutex_lock(&mutex);
    if (is_file_input) {
        stringstream ss;
        ss << "[Поток] Принимающий поток " << id << "инициализирован\n";
        logs.push_back(ss.str());
    }
    cout << "[Поток] Принимающий поток " << id << "инициализирован\n";
    cout.flush();
    pthread_mutex_unlock(&mutex);

    while (true) {
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // если получаем отвечаем
        if (chatters[id].status == 1) {
            if (is_file_input) {
                stringstream ss;
                ss << "[Начал] " << chatters[id].from << " -> " << chatters[id].to << '\n';
                logs.push_back(ss.str());
            }
            cout << "[Начал] " << chatters[id].from << " -> " << chatters[id].to << '\n';
            cout.flush();

            // устанавливаем состояние звонка
            chatters[id].status = 3;
            chatters[chatters[id].from].status = 3;
            cout.flush();
            pthread_mutex_unlock(&mutex);
            sleep(rand() % 10);
            pthread_mutex_lock(&mutex);
            // Через какое-то время после вызова
            if (is_file_input) {
                stringstream ss;
                ss << "[Закончил] " << chatters[id].from << " -> " << chatters[id].to << '\n';
                logs.push_back(ss.str());
            }
            cout << "[Закончил] " << chatters[id].from << " -> " << chatters[id].to << '\n';
            cout.flush();
            chatters[chatters[id].from].from = -1;
            chatters[chatters[id].from].to = -1;
            chatters[chatters[id].from].status = 0;
            chatters[id].from = -1;
            chatters[id].to = -1;
            chatters[id].status = 0;
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    return nullptr;
}

// Мониторим ввод 's' для завершения программы
void *exitable(void *args) {
    ArgStruct *arg = (ArgStruct *)args;
    pthread_mutex_t mutex = arg->mutex;
    while (cin.get() != 's') {
    };
    pthread_mutex_lock(&mutex);
    run = false;
    pthread_mutex_unlock(&mutex);
}

// Функция контроля состояния
void *control(void *args) {
    ArgStruct *arg = (ArgStruct *)args;
    pthread_mutex_t mutex = arg->mutex;
    while (true) {
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if (is_file_input) {
            stringstream ss;
            ss << "[Состояние] ";
            for (int i = 0; i < chatters.size(); i++) {
                ss << "(" << chatters[i].to << " " << chatters[i].from << " " << chatters[i].status
                   << ") ";
            }
            ss << '\n';
            logs.push_back(ss.str());
        }
        cout << "[Состояние] ";
        for (int i = 0; i < chatters.size(); i++) {
            cout << "(" << chatters[i].to << " " << chatters[i].from << " " << chatters[i].status
                 << ") ";
        }
        cout << '\n';
        cout.flush();
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
}

int main(int argc, char const *argv[]) {
    // Проверки аргументов
    if (argc < 3) {
        cout << "[Ошибка] неверное количество аргументов\n";
        return 0;
    }
    int n;
    string out_file_name;
    if (strcmp(argv[1], "-c") == 0) {
        try {
            n = atoi(argv[2]);
        } catch (exception e) {
            cout << "[Ошибка] неверное количество болтунов\n";
            return 0;
        }
        if (n < 2) {
            cout << "[Ошибка] неверное количество болтунов\n";
            return 0;
        }
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc < 4) {
            cout << "[Ошибка] неверное количество аргументов\n";
            return 0;
        }
        std::ifstream fin;
        fin.open(argv[2]);
        if (!fin) {
            cout << "[Ошибка] неверное имя входного файла\n";
            return 0;
        }
        fin >> n;
        fin.close();
        if (n < 2) {
            cout << "[Ошибка] неверное количество болтунов\n";
            return 0;
        }
        out_file_name = argv[3];
        is_file_input = true;
    } else if (strcmp(argv[1], "-r") == 0) {
        if (argc < 4) {
            cout << "[Ошибка] неверное количество аргументов\n";
            return 0;
        }
        try {
            int low = atoi(argv[2]);
            int high = atoi(argv[3]);
            if (low < 2 || low >= high) {
                throw std::invalid_argument("");
            }
            n = rand() % (high - low) + low;
        } catch (exception e) {
            cout << "[Ошибка] неверне рандома\n";
            return 0;
        }
        if (argc == 5) {
            out_file_name = argv[4];
            is_file_input = true;
        }
    } else {
        cout << "[Ошибка] неверный флаг\n";
        return 0;
    }

    logs = vector<string>();
    srand(10);
    if (is_file_input) {
        stringstream ss;
        ss << "[Информация] Запускается симуляция " << n << " болтунов\n";
        logs.push_back(ss.str());
    }
    cout << "[Информация] Запускается симуляция " << n << " болтунов\n";
    cout.flush();
    cout << "[Информация] Введите 's' чтобы остановить симуляцию\n";
    sleep(1);

    // Создаем все нужные потоки (2*n для n болтунов)

    pthread_t threads[n * 2];

    // Два вспомогательных потока
    pthread_t observer;
    pthread_t controller;

    // Используем мьютекс для контроля доступа к общим переменным
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    chatters = vector<Chatter>(n);

    for (int i = 0; i < n; i++) {
        Chatter t{-1, -1, 0};
        chatters[i] = t;
    }

    ArgStruct params[n];
    ArgStruct additional;
    additional.mutex = mutex;

    for (int i = 0; i < n; i++) {
        params[i].mutex = mutex;
        params[i].id = i;
        params[i].count = n;
        pthread_create(&threads[i], nullptr, callto, &params[i]);
        pthread_create(&threads[i + n], nullptr, getcall, &params[i]);
    }
    pthread_create(&controller, nullptr, exitable, &additional);
    pthread_create(&observer, nullptr, control, &additional);

    // Джойним контроллер
    pthread_join(controller, nullptr);

    pthread_mutex_lock(&mutex);
    run = false;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);

    // Если файловый ввод/вывод
    if (is_file_input) {
        cout << "[Информация] Логи сохраняются в файл\n";
        std::ofstream fout;
        fout.open(out_file_name);
        for (int i = 0; i < logs.size(); ++i) {
            fout << logs[i];
        }
        fout.close();
    }
    cout << "[Информация] Симуляция завершена\n";
    return 0;
}