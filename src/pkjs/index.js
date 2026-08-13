var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var username = 'mat';
var showLanguages = true;
var showProjects = true;
var maxLanguages = 5;
var maxProjects = 3;
var hasFetched = false;

var MSG = {
  RequestFetch: 10000,
  TotalSeconds: 10001,
  Streak: 10002,
  Languages: 10003,
  Projects: 10100
};

function fetchStats(force) {
  if (hasFetched && !force) return;
  hasFetched = true;
  console.log('Fetching stats for: ' + username);
  var now = new Date();
  var start = new Date(now.getFullYear(), now.getMonth(), now.getDate()).toISOString();
  var end = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 23, 59, 59).toISOString();
  var url = 'https://hackatime.hackclub.com/api/v1/users/' + username + '/stats?features=languages,projects&start_date=' + start + '&end_date=' + end;

  var xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.onload = function() {
    console.log('HTTP: ' + xhr.status);
    if (xhr.status === 200) {
      try {
        var data = JSON.parse(xhr.responseText).data;
        var msg = {};

        msg[MSG.TotalSeconds] = data.total_seconds || 0;
        msg[MSG.Streak] = data.streak || 0;

        var langs = data.languages || [];
        var langCount = Math.min(langs.length, maxLanguages);
        msg[MSG.Languages] = langCount;
        for (var i = 0; i < langCount; i++) {
          msg[MSG.Languages + 1 + i * 3] = langs[i].name || '';
          msg[MSG.Languages + 2 + i * 3] = Math.round(langs[i].total_seconds || 0);
          msg[MSG.Languages + 3 + i * 3] = Math.round((langs[i].percent || 0) * 100);
        }

        var projs = data.projects || [];
        var projCount = Math.min(projs.length, maxProjects);
        msg[MSG.Projects] = projCount;
        for (var j = 0; j < projCount; j++) {
          msg[MSG.Projects + 1 + j * 3] = projs[j].name || '';
          msg[MSG.Projects + 2 + j * 3] = Math.round(projs[j].total_seconds || 0);
          msg[MSG.Projects + 3 + j * 3] = Math.round((projs[j].percent || 0) * 100);
        }

        console.log('Sending ' + langCount + ' langs, ' + projCount + ' projs');
        Pebble.sendAppMessage(msg, function() {
          console.log('Sent OK');
        }, function(e) {
          console.log('Send fail: ' + JSON.stringify(e));
        });
      } catch (e) {
        console.log('Parse error: ' + e.message);
      }
    } else {
      console.log('HTTP error: ' + xhr.status);
    }
  };
  xhr.onerror = function() {
    console.log('Network error');
  };
  xhr.send();
}

Pebble.addEventListener('ready', function(e) {
  console.log('JS ready');
  fetchStats(false);
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('Msg received');
  fetchStats(true);
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e.response) {
    try {
      var settings = JSON.parse(decodeURIComponent(e.response));
      if (settings.Username) username = settings.Username;
      if (settings.ShowLanguages !== undefined) showLanguages = settings.ShowLanguages === '1';
      if (settings.ShowProjects !== undefined) showProjects = settings.ShowProjects === '1';
      if (settings.MaxLanguages) maxLanguages = parseInt(settings.MaxLanguages) || 5;
      if (settings.MaxProjects) maxProjects = parseInt(settings.MaxProjects) || 3;
    } catch (err) {
      console.log('Settings error: ' + err);
    }
  }
});
