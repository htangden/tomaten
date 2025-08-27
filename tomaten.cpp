#include "json.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

using json = nlohmann::json;
using namespace std;

string get_data_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        cerr << "HOME environment variable not set" << endl;
        exit(1);
    }
    return string(home) + "/.local/share/tomaten";
}

const string CURRENT_FILE = get_data_dir() + "/current.json";
const string ARCHIVE_FILE = get_data_dir() + "/archive.json";

void ensure_data_directory() {
    string data_dir = get_data_dir();
    
    // Create ~/.local if it doesn't exist
    string local_dir = string(getenv("HOME")) + "/.local";
    mkdir(local_dir.c_str(), 0755);
    
    // Create ~/.local/share if it doesn't exist
    string share_dir = local_dir + "/share";
    mkdir(share_dir.c_str(), 0755);
    
    // Create ~/.local/share/tomaten if it doesn't exist
    mkdir(data_dir.c_str(), 0755);
}

void initialize_current_json() {
    ifstream check_file(CURRENT_FILE);
    if (check_file.is_open()) {
        check_file.close();
        return; // File exists, no need to create
    }
    
    json initial_entry;
    initial_entry["type"] = "naught";
    
    json j;
    j["entry"] = initial_entry;
    
    ofstream out(CURRENT_FILE);
    out << std::setw(4) << j << std::endl;
}

void initialize_archive_json() {
    ifstream check_file(ARCHIVE_FILE);
    if (check_file.is_open()) {
        check_file.close();
        return; // File exists, no need to create
    }
    
    json archive;
    archive["entries"] = json::array();
    
    ofstream out(ARCHIVE_FILE);
    out << std::setw(4) << archive << std::endl;
}

struct Config {
    string work_color = "#458588";
    string break_color = "#689d68";
    string text_color = "#ebdbb2";
    string relax_message = "Relax!";
    string back_to_work_message = "Back to work!";
    string notification_title_break = "Tomaten";
    string notification_title_work = "Tomaten";
    bool send_notifications = true;
};

Config load_config() {
    Config config;
    
    const char* home = getenv("HOME");
    if (!home) return config;
    
    string config_path = string(home) + "/.config/tomaten/config.json";
    ifstream config_file(config_path);
    
    if (!config_file.is_open()) {
        return config;
    }
    
    try {
        json config_json;
        config_file >> config_json;
        
        if (config_json.contains("colors")) {
            if (config_json["colors"].contains("work")) {
                config.work_color = config_json["colors"]["work"];
            }
            if (config_json["colors"].contains("break")) {
                config.break_color = config_json["colors"]["break"];
            }
            if (config_json["colors"].contains("text")) {
                config.text_color = config_json["colors"]["text"];
            }
        }
        
        if (config_json.contains("messages")) {
            if (config_json["messages"].contains("relax")) {
                config.relax_message = config_json["messages"]["relax"];
            }
            if (config_json["messages"].contains("back_to_work")) {
                config.back_to_work_message = config_json["messages"]["back_to_work"];
            }
            if (config_json["messages"].contains("notification_title_break")) {
                config.notification_title_break = config_json["messages"]["notification_title_break"];
            }
            if (config_json["messages"].contains("notification_title_work")) {
                config.notification_title_work = config_json["messages"]["notification_title_work"];
            }
        }
        
        if (config_json.contains("send_notifications")) {
            config.send_notifications = config_json["send_notifications"];
        }
    } catch (...) {
        // If JSON parsing fails, return default config
    }
    
    return config;
}

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

  ensure_data_directory();
  initialize_current_json();
  initialize_archive_json();
  Config config = load_config();

  string action;
  int duration;

  if (argc == 3) {
    action = argv[1];
    duration = stoi(argv[2]);
  } else if (argc == 2) {
    action = argv[1];
  } else {
    cout << "Usage: tomaten <action> [duration_minutes]" << endl;
    cout << "Available actions:\nstatus, start, break, finish, clear"
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
      if (active_entry["type"] == "tomat") {
        cout << "<span color='" + config.text_color + "' bgcolor='" + config.break_color  + "'> " + config.relax_message + " </span>"
             << endl;

      } else {
        cout << "<span color='" + config.text_color + "' bgcolor='" + config.work_color + "'> " + config.back_to_work_message + " </span>"
             << endl;
      }
      return 0;
    }

    if (time_left > 0) {
      string bgcolor = (active_entry["type"] == "tomat") ? config.work_color : config.break_color;
      int seconds_left = time_left % 60;
      string text = to_string(time_left / 60) + ":" + time_string(seconds_left);
      cout << "<span color='" + config.text_color + "' bgcolor='" + bgcolor + "'> " + text + " </span>"
           << endl;
    } else {
      active_entry["end"] = end_time;
      write_current(active_entry);

      if (active_entry["type"] == "tomat") {
        if (config.send_notifications) {
          system(("notify-send '" + config.notification_title_break + "' '" + config.relax_message + "'").c_str());
        }
        cout << config.relax_message << " 🟢" << endl;
      } else {
        if (config.send_notifications) {
          system(("notify-send '" + config.notification_title_work + "' '" + config.back_to_work_message + "'").c_str());
        }
        cout << config.back_to_work_message << " 🔴" << endl;
      }
    }
    return 0;
  }

  if (action == "start" || action == "break") {
    if (active_entry["type"] != "naught" && active_entry.contains("end")) {
      archive_entry(active_entry);
    }
    json new_entry;
    new_entry["type"] = (action == "start") ? "tomat" : "break";
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
