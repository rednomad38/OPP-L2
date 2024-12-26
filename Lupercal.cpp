#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <chrono>  // Для работы с year_month_day

using namespace std;
using namespace chrono;

struct House {
  string owner;
  year_month_day date;
  int cost;

  House(const string& o, const year_month_day& d, int c)
     : owner(o), date(d), cost(c) {
  }
};

// Проверка корректности даты
bool IsValidDate(const year_month_day& date) {
  return date.ok();
}

// Проверка корректности имени владельца
bool IsValidOwner(const string& owner) {
  regex result("^[A-Za-zА-Яа-я ]+( [A-Za-zА-Яа-я]+)?( [A-Za-zА-Яа-я]+)?$");
  return regex_match(owner, result);
}

// Проверка корректности стоимости
bool IsValidCost(int cost) {
  return cost >= 0;
}

// Функция для форматирования даты в строку
string FormatDate(const year_month_day& date) {
  ostringstream oss;
  oss << static_cast<unsigned>(date.day().operator unsigned()) << "."
    << static_cast<unsigned>(date.month().operator unsigned()) << "."
    << static_cast<int>(date.year());
  return oss.str();
}

// Безопасное преобразование строки в int
bool SafeStoi(const string& str, int& result) {
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

// Сортировка по фамилии (первое слово)
void SortByLastName(vector<House>& home) {
  sort(home.begin(), home.end(), [](const House& a, const House& b) {
    string lastNameA = a.owner.substr(0, a.owner.find(' '));
    string lastNameB = b.owner.substr(0, b.owner.find(' '));

    transform(lastNameA.begin(), lastNameA.end(), lastNameA.begin(), ::tolower);
    transform(lastNameB.begin(), lastNameB.end(), lastNameB.begin(), ::tolower);

    return lastNameA < lastNameB;
    });
}

// Сортировка по имени (второе слово)
void SortByFirstName(vector<House>& home) {
  sort(home.begin(), home.end(), [](const House& a, const House& b) {
    auto getFirstName = [](const string& owner) {
      istringstream iss(owner);
      vector<string> words((istream_iterator<string>(iss)), istream_iterator<string>());
      return (words.size() > 1) ? words[1] : "";
      };

    string firstNameA = getFirstName(a.owner);
    string firstNameB = getFirstName(b.owner);

    transform(firstNameA.begin(), firstNameA.end(), firstNameA.begin(), ::tolower);
    transform(firstNameB.begin(), firstNameB.end(), firstNameB.begin(), ::tolower);

    return firstNameA < firstNameB;
  });
}

// Сортировка по дате
void SortByDate(vector<House>& home) {
  sort(home.begin(), home.end(), [](const House& a, const House& b) {
    return a.date < b.date;
    });
}

// Вывод списка домов
void DisplayHouses(const vector<House>& home) {
  for (const House& h : home) {
    cout << "Владелец: " << h.owner << endl;
    cout << "Дата: " << FormatDate(h.date) << endl;
    cout << "Стоимость: " << h.cost << " руб." << endl;
    cout << endl;
  }
}

// Основное меню
void MainMenu(vector<House>& home) {
  int choice;
  do {
    cout << "=== Меню ===" << endl;
    cout << "1. Показать все записи" << endl;
    cout << "2. Сортировать по фамилии" << endl;
    cout << "3. Сортировать по имени" << endl;
    cout << "4. Сортировать по дате" << endl;
    cout << "5. Выйти" << endl;
    cout << "Введите номер действия: ";
    cin >> choice;

    switch (choice) {
      case 1:
        cout << "\n--- Список записей ---\n";
        DisplayHouses(home);
        break;
      case 2:
        SortByLastName(home);
        cout << "\n--- Записи отсортированы по фамилии ---\n";
        DisplayHouses(home);
        break;
      case 3:
        SortByFirstName(home);
        cout << "\n--- Записи отсортированы по имени ---\n";
        DisplayHouses(home);
        break;
      case 4:
        SortByDate(home);
        cout << "\n--- Записи отсортированы по дате ---\n";
        DisplayHouses(home);
        break;
      case 5:
        cout << "Выход из программы.\n";
        break;
      default:
        cout << "Ошибка: Неверный выбор.\n";
        break;
      }
  } while (choice != 5);
}

int main() {
  setlocale(0, "");
  ifstream ist("1.txt");
  if (!ist) {
     cerr << "Ошибка: Не удалось открыть файл." << endl;
     return 1;
  }

  vector<House> home;
  string line;

  // Регулярное выражение для парсинга строк из файла
  regex lineRegex("^\"([^\"]+)\"\\s+(\\d{2})\\.(\\d{2})\\.(\\d{4})\\s+(\\d+)$");
  smatch match;

  while (getline(ist, line)) {
    if (regex_match(line, match, lineRegex)) {
      string owner = match[1];
      int dd, mm, yyyy, cost;

    if (!SafeStoi(match[2], dd) || !SafeStoi(match[3], mm) || !SafeStoi(match[4], yyyy) || !SafeStoi(match[5], cost)) {
      cerr << "Ошибка: некорректные данные в строке: " << line << endl;
      continue;
      }

    year_month_day date = year{ yyyy } / month{ static_cast<unsigned>(mm) } / day{ static_cast<unsigned>(dd) };

    if (!IsValidDate(date)) {
      cerr << "Ошибка: Неверная дата в строке: " << line << endl;
      continue;
    }

    if (!IsValidOwner(owner)) {
      cerr << "Ошибка: Неверное имя владельца в строке: " << line << endl;
      continue;
    }

    if (!IsValidCost(cost)) {
      cerr << "Ошибка: Неверная стоимость в строке: " << line << endl;
      continue;
    }

    home.emplace_back(owner, date, cost);
    }
    else {
      cerr << "Ошибка: Неверный формат строки: " << line << endl;
    }
  }

  MainMenu(home);

return 0;
}
