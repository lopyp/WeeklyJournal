#include "journal.h"

int main() {
  Controller controller;
  controller.run();

  return 0;
}
bool Date::operator<(const Date &other) const {
  if (year < other.year)
    return true;
  if (year >= other.year)
    return false;
  if (month < other.month)
    return true;
  if (month >= other.month)
    return false;
  return day < other.day;
}

bool Date::operator>(const Date &other) const { return other < *this; }

bool Date::operator==(const Date &other) const {
  return (!(other < *this) && !(*this < other));
}

std::string FullName::toString() const {
  return lastName + " " + firstName + " " + middleName;
}

bool FullName::operator==(const FullName &other) const {
  return (firstName == other.firstName && lastName == other.lastName &&
          middleName == other.middleName);
}

Model::Model() {}

void Model::addEvent(const Event &event) { events.push_back(event); }

void Model::addBirthday(const Birthday &birthday) {
  for (const Birthday &existingBirthday : birthdays) {
    if (existingBirthday.date == birthday.date &&
        existingBirthday.fullName == birthday.fullName) {
      std::cout << "День рождения на эту дату уже существует." << std::endl;
      return;
    }
  }
  birthdays.push_back(birthday);
}

std::vector<Event> Model::getEvents() {
  std::sort(events.begin(), events.end(), [](const Event &a, const Event &b) {
    return (a.created < b.created);
  });
  return events;
}

std::vector<Birthday> Model::getBirthdays() {
  std::sort(
      birthdays.begin(), birthdays.end(),
      [](const Birthday &a, const Birthday &b) { return a.date < b.date; });

  return birthdays;
}

void Model::removeExpiredEvents() {
  Date currentDate;
  for (auto it = events.begin(); it != events.end();) {
    if (currentDate > it->expires) {
      it = events.erase(it);
    } else {
      ++it;
    }
  }
}

void Model::incrementBirthdayAges() {
  Date currentDate;
  for (Birthday &birthday : birthdays) {
    if (currentDate == birthday.date) {
      birthday.age++;
    }
  }
}

void Model::saveToFile(const std::string &filename) {
  std::ofstream file(filename);
  if (file.is_open()) {
    for (const Event &event : events) {
      file << "Event: " << event.description
           << " Expires: " << event.expires.day << "." << event.expires.month
           << "." << event.expires.year << std::endl;
    }
    for (const Birthday &birthday : birthdays) {
      file << "Birthday: " << birthday.fullName.firstName << " "
           << birthday.fullName.lastName << " " << birthday.fullName.middleName
           << " Age: " << birthday.age << " Date: " << birthday.date.day << "."
           << birthday.date.month << "." << birthday.date.year << std::endl;
    }
    file.close();
  }
}

std::vector<Event> Model::getEventsByDate(const Date &date) {
  std::vector<Event> eventsByDate;
  for (const Event &event : events) {
    if (event.expires == date) {
      eventsByDate.push_back(event);
    }
  }
  return eventsByDate;
}

std::vector<Birthday> Model::getBirthdaysByDate(const Date &date) {
  std::vector<Birthday> birthdaysByDate;

  for (const Birthday &birthday : birthdays) {
    if (birthday.date == date) {
      birthdaysByDate.push_back(birthday);
    }
  }

  return birthdaysByDate;
}

std::vector<Birthday> Model::getBirthdaysByName(const std::string &name) {
  std::vector<Birthday> birthdaysByName;

  for (const Birthday &birthday : birthdays) {
    if (birthday.fullName.toString() == name) {
      birthdaysByName.push_back(birthday);
    }
  }

  return birthdaysByName;
}

Controller::Controller() {}

void Controller::startBackgroundTasks() {

  std::thread([&]() {
    while (true) {

      model.removeExpiredEvents();

      model.incrementBirthdayAges();

      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  }).detach();
}

void Controller::addEvent() {
  Event event;

  std::cout << "Введите описание события: ";
  std::cin.ignore();
  std::getline(std::cin, event.description);

  std::cout << "Введите дату окончания события (ДД.ММ.ГГГГ): ";
  std::string expiresDate;
  std::cin >> expiresDate;

  sscanf(expiresDate.c_str(), "%2d.%2d.%4d", &event.expires.day,
         &event.expires.month, &event.expires.year);
  event.expires.hour = 0;
  event.expires.minute = 0;
  auto now = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm *localTime = std::localtime(&currentTime);
  event.created.year = localTime->tm_year + 1900;
  event.created.month = localTime->tm_mon + 1;
  event.created.day = localTime->tm_mday;
  event.created.hour = localTime->tm_hour;
  event.created.minute = localTime->tm_min;
  model.addEvent(event);

  std::cout << "Событие успешно добавлено." << std::endl;
}

void Controller::addBirthday() {
  Birthday birthday;

  std::cout << "Введите полное имя (Фамилия Имя Отчество): ";
  std::cin.ignore();
  std::getline(std::cin, birthday.fullName.lastName, ' ');
  std::getline(std::cin, birthday.fullName.firstName, ' ');
  std::getline(std::cin, birthday.fullName.middleName);

  std::cout << "Введите дату дня рождения (ДД.ММ.ГГГГ): ";
  std::string birthDate;
  std::cin >> birthDate;

  sscanf(birthDate.c_str(), "%2d.%2d.%4d", &birthday.date.day,
         &birthday.date.month, &birthday.date.year);
  birthday.date.hour = 0;
  birthday.date.minute = 0;

  auto now = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm *localTime = std::localtime(&currentTime);
  birthday.age = localTime->tm_year + 1900 - birthday.date.year;
  if (birthday.date.month < localTime->tm_mon ||
      (birthday.date.month == localTime->tm_mon &&
       birthday.date.day < localTime->tm_mday)) {
    birthday.age--;
  }

  model.addBirthday(birthday);

  std::cout << "День рождения успешно добавлен." << std::endl;
}

void Controller::displayEventsByDate() {
  Date chosenDate;

  std::cout << "Введите дату (ДД.ММ.ГГГГ), чтобы отобразить события: ";
  std::string dateString;
  std::cin >> dateString;
  sscanf(dateString.c_str(), "%2d.%2d.%4d", &chosenDate.day, &chosenDate.month,
         &chosenDate.year);

  std::vector<Event> eventsByDate = model.getEventsByDate(chosenDate);

  if (!eventsByDate.empty()) {
    view.displayEvents(eventsByDate);
  } else {
    view.displayMessage("Событий на указанную дату не найдено.");
  }
}

void Controller::displayBirthdaysByDate() {
  Date chosenDate;

  std::cout << "Введите дату (ДД.ММ.ГГГГ), чтобы отобразить дни рождения: ";
  std::string dateString;
  std::cin >> dateString;
  sscanf(dateString.c_str(), "%2d.%2d.%4d", &chosenDate.day, &chosenDate.month,
         &chosenDate.year);

  std::vector<Birthday> birthdaysByDate = model.getBirthdaysByDate(chosenDate);

  if (!birthdaysByDate.empty()) {
    view.displayBirthdays(birthdaysByDate);
  } else {
    view.displayMessage("Дней рождений на указанную дату не найдено.");
  }
}

void Controller::displayBirthdaysByName() {
  std::string fullName;

  std::cout << "Введите полное имя (Фамилия Имя Отчество) человека: ";
  std::cin.ignore();
  std::getline(std::cin, fullName);

  std::vector<Birthday> birthdaysByName = model.getBirthdaysByName(fullName);

  if (!birthdaysByName.empty()) {
    view.displayBirthdays(birthdaysByName);
  } else {
    view.displayMessage("Дней рождений для указанного имени не найдено.");
  }
}

void Controller::run() {
  startBackgroundTasks();
  while (true) {
    std::cout << "Выберите действие:\n";
    std::cout << "1. Добавить событие\n";
    std::cout << "2. Добавить день рождения\n";
    std::cout << "3. Вывести список событий\n";
    std::cout << "4. Вывести список дней рождения\n";
    std::cout << "5. Вывести список cсобытий по дате\n";
    std::cout << "6. Вывести список дней рождений по дате\n";
    std::cout << "7. Вывести список дней рождения по имени\n";
    std::cout << "8. Сохранить данные в файл\n";
    std::cout << "9. Выйти\n";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
    case 1: {
      addEvent();
      break;
    }
    case 2: {
      addBirthday();
      break;
    }
    case 3: {
      std::vector<Event> events = model.getEvents();
      view.displayEvents(events);
      break;
    }
    case 4: {
      std::vector<Birthday> birthdays = model.getBirthdays();
      view.displayBirthdays(birthdays);
      break;
    }
    case 5: {
      displayEventsByDate();
      break;
    }
    case 6: {
      displayBirthdaysByDate();
      break;
    }
    case 7: {
      displayBirthdaysByName();
      break;
    }
    case 8: {
      std::string filename;
      std::cout << "Введите имя файла для сохранения данных: ";
      std::cin >> filename;
      model.saveToFile(filename);
      break;
    }
    case 9: {
      return;
    }
    default: {
      std::cout << "Неверный выбор. Пожалуйста, введите корректное значение.\n";
      break;
    }
    }
  }
}

void View::displayEvents(const std::vector<Event> &events) {
  std::cout << "Список событий:" << std::endl;
  for (const Event &event : events) {
    std::cout << "Дата создания: " << event.created.day << "."
              << event.created.month << "." << event.created.year << " "
              << event.created.hour << ":" << event.created.minute << std::endl;
    std::cout << "Дата окончания: " << event.expires.day << "."
              << event.expires.month << "." << event.expires.year << " "
              << event.expires.hour << ":" << event.expires.minute << std::endl;
    std::cout << "Описание: " << event.description << std::endl;
    std::cout << std::endl;
  }
}

void View::displayBirthdays(const std::vector<Birthday> &birthdays) {
  std::cout << "Список дней рождения:" << std::endl;
  for (const Birthday &birthday : birthdays) {
    std::cout << "Имя: " << birthday.fullName.firstName << " "
              << birthday.fullName.lastName << " "
              << birthday.fullName.middleName << std::endl;
    std::cout << "Дата: " << birthday.date.day << "." << birthday.date.month
              << "." << birthday.date.year << std::endl;
    std::cout << "Возраст: " << birthday.age << std::endl;
    std::cout << std::endl;
  }
}

void View::displayMessage(const std::string &message) {
  std::cout << message << std::endl;
}
