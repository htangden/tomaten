## i3blocks-Compatible Pomodoro Timer

<img width="444" height="34" alt="image" src="https://github.com/user-attachments/assets/b23af1f1-0030-4448-b9dc-2036f54c187a" />

### Setup
Clone the repository and build the binary with:

```bash
g++ tomaten.cpp
````

Then move the binary to a directory in your `$PATH`.

Install the `libnotify` package to enable notifications.

Update your i3blocks configuration with:

```
[tomaten]
command=tomaten status
interval=1
markup=pango
```

### Usage

* `tomaten start $DURATION`: Start a pomodoro timer
* `tomaten break $DURATION`: Start a break timer
* `tomaten status`: Show the current status
* `tomaten finish`: Finish the active timer
* `tomaten clear`: Clear the current timer

### Configuration

The configuration file is located at `~/.config/tomaten/config.json`.
Default values:

```json
{
  "messages": {
    "back_to_work": "Back to work!",
    "relax": "Relax!",
    "notification_title_break": "Tomaten",
    "notification_title_work": "Tomaten"
  },
  "colors": {
    "work": "#458588",
    "relax": "#689d68",
    "text": "#ebdbb2"
  },
  "send_notifications": true
}
```
