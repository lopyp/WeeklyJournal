#ifndef JOURNAL_H
#define JOURNAL_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

struct Date {
  int day;
  int month;
  int year;
  int hour;
  int minute;
  bool operator<(const Date &other) const;
  bool operator>(const Date &other) const;
  bool operator==(const Date &other) const;
};

struct Event {
  Date created;
  Date expires;
  std::string description;
};

struct FullName {
  std::string firstName;
  std::string lastName;
  std::string middleName;
  bool operator==(const FullName &other) const;
  std::string toString() const;
};

struct Birthday {
  Date date;
  FullName fullName;
  int age;
};

class Model {
private:
  std::vector<Event> events;
  std::vector<Birthday> birthdays;

public:
  Model();

  void addEvent(const Event &event);
  void addBirthday(const Birthday &birthday);
  std::vector<Event> getEvents();
  std::vector<Birthday> getBirthdays();
  std::vector<Event> getEventsByDate(const Date &date);
  std::vector<Birthday> getBirthdaysByDate(const Date &date);
  std::vector<Birthday> getBirthdaysByName(const std::string &name);
  void removeExpiredEvents();
  void incrementBirthdayAges();
  void saveToFile(const std::string &filename);
};

class View {
public:
  void displayEvents(const std::vector<Event> &events);
  void displayBirthdays(const std::vector<Birthday> &birthdays);
  void displayMessage(const std::string &message);
};

class Controller {
private:
  Model model;
  View view;

  void runAsyncTasks();
  void startBackgroundTasks();
  void addEvent();
  void addBirthday();
  void displayEventsByDate();
  void displayBirthdaysByDate();
  void displayBirthdaysByName();

public:
  Controller();

  void run();
};

#endif // JOURNAL_H
