#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <sstream>
#include <chrono> // Для работы с year_month_day

using namespace std;
using namespace chrono;

struct house {
    string owner;
    year_month_day date;
    int cost;

    house(const string& o, const year_month_day& d, int c) : owner(o), date(d), cost(c) {}
};

// Проверка корректности даты
bool isValidDate(const year_month_day& date) {
    return date.ok();
}

// Проверка корректности имени владельца
bool isValidOwner(const string& owner) {
    regex result("^[A-Za-zА-Яа-я ]+( [A-Za-zА-Яа-я]+)?( [A-Za-zА-Яа-я]+)?$");
    return regex_match(owner, result);
}

// Проверка корректности стоимости
bool isValidCost(int cost) {
    return cost >= 0;
}

// Функция для форматирования даты в строку
string formatDate(const year_month_day& date) {
    ostringstream oss;
    oss << static_cast<unsigned>(date.day().operator unsigned()) << "."
        << static_cast<unsigned>(date.month().operator unsigned()) << "."
        << static_cast<int>(date.year());
    return oss.str();
}

// Безопасное преобразование строки в int
bool safeStoi(const string& str, int& result) {
    try {
        result = stoi(str);
        return true;
    }
    catch (const invalid_argument&) {
        cerr << "Ошибка: некорректное число в строке \"" << str << "\"." << endl;
    }
    catch (const out_of_range&) {
        cerr << "Ошибка: число \"" << str << "\" выходит за пределы int." << endl;
    }
    return false;
}

int main() {
    setlocale(0, "");
    ifstream ist("1.txt");
    if (!ist) {
        cerr << "Ошибка: Не удалось открыть файл." << endl;
        return 1;
    }

    vector<house> home;
    string line;

    // Регулярное выражение для парсинга строк из файла
    regex lineRegex("^\"([^\"]+)\"\\s+(\\d{2})\\.(\\d{2})\\.(\\d{4})\\s+(\\d+)$");
    smatch match;

    while (getline(ist, line)) {
        if (regex_match(line, match, lineRegex)) {
            string owner = match[1];
            int dd, mm, yyyy, cost;

            // Безопасное преобразование строки в числа
            if (!safeStoi(match[2], dd) || !safeStoi(match[3], mm) || !safeStoi(match[4], yyyy) || !safeStoi(match[5], cost)) {
                cerr << "Ошибка: некорректные данные в строке: " << line << endl;
                continue;
            }

            // Создание объекта year_month_day
            year_month_day date = year{ yyyy } / month{ static_cast<unsigned>(mm) } / day{ static_cast<unsigned>(dd) };

            // Проверки на корректность данных
            if (!isValidDate(date)) {
                cerr << "Ошибка: Неверная дата в строке: " << line << endl;
                continue;
            }

            if (!isValidOwner(owner)) {
                cerr << "Ошибка: Неверное имя владельца в строке: " << line << endl;
                continue;
            }

            if (!isValidCost(cost)) {
                cerr << "Ошибка: Неверная стоимость в строке: " << line << endl;
                continue;
            }

            // Безопасное добавление данных
            home.emplace_back(owner, date, cost);
        }
        else {
            cerr << "Ошибка: Неверный формат строки: " << line << endl;
        }
    }

    // Вывод данных о домах
    for (const house &h : home) {
        cout << "Владелец: " << h.owner << endl;
        cout << "Дата: " << formatDate(h.date) << endl;
        cout << "Стоимость: " << h.cost << " руб." << endl;
        cout << endl;
    }

    return 0;
}

