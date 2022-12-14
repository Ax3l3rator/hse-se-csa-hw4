# Отчет ДЗ№4

## Алексей Коледаев | БПИ-213 | Вариант 16

> Задача о болтунах. N болтунов имеют телефоны. Они либо ждут звонков, либо звонят друг другу, чтобы побеседовать. Если телефон случайного абонента занят, болтун будет звонить другому абоненту, пока ему кто-нибудь не ответит. Побеседовав некоторое время, болтун или ждет звонка, или звонит на другой случайный номер. Создать многопоточное приложение, моделирующее поведение болтунов. Для решения задачи использовать мьютексы.

Скомпилировать - `g++ solution.cpp -lpthread -o prog`

## На 4

---

- На вход программа получает число $n$ — количество болтунов
- Программа использует модель итеративного параллелизма
- Реализовано [консольное приложение](./solution.cpp)(изменено в соответсвие требованиям на 8 баллов)
- Реализован ввод данных из консоли

## На 5

---

- В [программу](./solution.cpp) добавлены комментарии, поясняющие выполняемые действия и описание используемых переменных

- $n$ болтунов приходят домой и готовятся звонить или принимать вызовы. Будет болтун звонить или принимать вызов он решает в момент, когда берет в руки телефон. Если выбор был сделан в пользу созвона, то болтун начинает прозванивать всех болтунов в своей телефонной книжке(то есть всех остальных болтунов), если болтун уже кого-то прозванивает, он не может ответить на входящий вызов, то же самое происходит, если болтун уже разговаривает. Ждущий болтун просто ждет вызова. Как только разговор прекращается болтун ищет себе новвого собеседника.

## На 6

---

- В [программе](./solution.cpp) реализован ввод данных из командной строки `./prog -c chattersNum[int]`

- В программе есть вектор болтунов, который отслеживает состояние каждого болтуна, а так же потоки получающих и потоки осуществляющих вызовы. С промежутком в случайное время постоянно осуществляются попытки вызовов, симулируя работу болтунов. Получения статусов болтунов осузествляется через вектор болтунов, в котором храняться статусы всех болтунов. Их статусы получают специальное состояние как только произойдет соединение двух болтунов, это значит что исходящий вызов был успешно принят другим болтуном, теперь какое-то время они будут болтать, когда они освободятся изменится их состояние статуса и с помощью потоков они опять будут искать себе пару для созвона.

## На 7

---

- В [программу](./solution.cpp) добавлен файловый ввод и вывод (`./prog -f in.txt out.txt`)

- Приведены результаты работы в [файле](./out.txt)

- Расширен ввод из командной строки

## На 8

---

- В [программу](./solution.cpp) добавлена генерация случайных данных в допустимых диапозонах (`./prog -r lowerBound[int] upperBound[int]`)

- Расширен ввод из командной строки

- Добавлены [результаты](./tests) тестовых прогонов

## На 9

---

[Программа на шесть баллов](./openMP.cpp) с использованием openMP, компилим с помощью `g++  openMP.cpp -fopenmp -o openmp`
