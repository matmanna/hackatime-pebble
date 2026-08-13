module.exports = [
  {
    "type": "heading",
    "defaultValue": "Hackatime Settings"
  },
  {
    "type": "text",
    "defaultValue": "Configure your coding stats display."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Account"
      },
      {
        "type": "input",
        "messageKey": "Username",
        "defaultValue": "mat",
        "label": "Hackatime Username",
        "description": "Your Hackatime username or Slack ID"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Display"
      },
      {
        "type": "toggle",
        "messageKey": "ShowLanguages",
        "label": "Show Languages",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "ShowProjects",
        "label": "Show Projects",
        "defaultValue": true
      },
      {
        "type": "slider",
        "messageKey": "MaxLanguages",
        "label": "Max Languages to Show",
        "defaultValue": 5,
        "min": 1,
        "max": 10
      },
      {
        "type": "slider",
        "messageKey": "MaxProjects",
        "label": "Max Projects to Show",
        "defaultValue": 3,
        "min": 1,
        "max": 10
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
