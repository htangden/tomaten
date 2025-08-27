## i3blocks compatible pomodoro timer

<img width="444" height="34" alt="image" src="https://github.com/user-attachments/assets/b23af1f1-0030-4448-b9dc-2036f54c187a" />


### Setup
Clone the repository and build the binary with `g++ tomaten.cpp`. Move the binary to path.

Install `libnotify` package in order for notifications working.

Update i3blocks configuration with: 
```
[tomaten]
command=tomaten status
interval=1
markup=pango

```

### Usage
- `tomaten start $DURATION`: Starts a pomodoro timer
- `tomaten break $DURATION`: Starts a break timer
- `tomaten status`: Prints status
- `tomaten finish`: Completes active timer
- `tomaten clear`: Clears current timer

### Config
Config is possible in ~/.config/tomaten/config.json. These are the default values:
```
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

In order for the notifications to work `notify-send` has to be installed. 
