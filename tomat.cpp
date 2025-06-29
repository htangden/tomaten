#include "json.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace std;

const string CURRENT_FILE = "/home/hugo/code/programs/tomaten/current.json";
const string ARCHIVE_FILE = "/home/hugo/code/programs/tomaten/archive.json";

const string work_color = "#458588";
const string break_color = "#689d68";

void write_current(const json &entry) {
  json j;
  j["entry"] = entry;
  ofstream out(CURRENT_FILE);
  out << std::setw(4) << j << std::endl;
}

void archive_entry(const json &entry) {
  ifstream infile(ARCHIVE_FILE);
  json archive;

  if (infile.is_open() && infile.peek() != ifstream::traits_type::eof()) {
    infile >> archive;
  }
  infile.close();

  if (!archive.contains("entries") || !archive["entries"].is_array()) {
    archive["entries"] = json::array();
  }

  archive["entries"].push_back(entry);

  ofstream out(ARCHIVE_FILE);
  out << std::setw(4) << archive << std::endl;
}

string time_string(int seconds_left) {
  stringstream ss;
  ss << std::setw(2) << std::setfill('0') << seconds_left;
  return ss.str();
}

int main(int argc, char *argv[]) {

  string action;
  int duration;

  if (argc == 3) {
    action = argv[1];
    duration = stoi(argv[2]);
  } else if (argc == 2) {
    action = argv[1];
  } else {
    cout << "Usage: pomodoro <action> [duration_minutes]" << endl;
    cout << "Available actions:\nstatus, pomodoro, break, finish, clear"
         << endl;
    return 1;
  }

  // Reading JSON file
  ifstream file(CURRENT_FILE);
  if (!file.is_open()) {
    cout << "Could not open current.json\n";
    return 1;
  }
  json j;
  file >> j;
  json &active_entry = j["entry"];
  time_t now = time(nullptr);

  if (action == "status") {

    if (active_entry["type"] == "naught") {
      return 0;
    }

    time_t end_time =
        (int)active_entry["start"] + 60 * (int)active_entry["duration"];
    int time_left = end_time - now;

    if (active_entry.contains("end")) {
      if (active_entry["type"] == "pomodoro") {
        cout << "<span color='#ebdbb2' bgcolor='" + break_color  + "'> Relax! </span>"
             << endl;

      } else {
        cout << "<span color='#ebdbb2' bgcolor='" + work_color + "'> Back to work! </span>"
             << endl;
      }
      return 0;
    }

    if (time_left > 0) {
      string bgcolor = (active_entry["type"] == "pomodoro") ? work_color : break_color;
      int seconds_left = time_left % 60;
      string text = to_string(time_left / 60) + ":" + time_string(seconds_left);
      cout << "<span color='#ebdbb2' bgcolor='" + bgcolor + "'> " + text + " </span>"
           << endl;
    } else {
      active_entry["end"] = end_time;
      write_current(active_entry);

      if (active_entry["type"] == "pomodoro") {
        system("notify-send 'Tomaten 🟢' 'Relax!'");
        cout << "Relax! 🟢" << endl;
      } else {
        system("notify-send 'Tomaten 🔴' 'Back to work!'");
        cout << "Back to work! 🔴" << endl;
      }
    }
    return 0;
  }

  if (action == "pomodoro" || action == "break") {
    if (active_entry["type"] != "naught" && active_entry.contains("end")) {
      archive_entry(active_entry);
    }
    json new_entry;
    new_entry["type"] = action;
    new_entry["start"] = now;
    new_entry["duration"] = duration;
    write_current(new_entry);
    return 0;
  }

  if (action == "finish") {
    if (active_entry.contains("end")) {
      return 0;
    }
    active_entry["end"] = now;
    write_current(active_entry);
    return 0;
  }

  if (action == "clear") {
    json new_entry;
    new_entry["type"] = "naught";
    write_current(new_entry);
    return 0;
  }

  cout << "invalid action" << endl;
  return 1;
}
