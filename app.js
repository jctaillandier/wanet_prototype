/*
*   Serveur de développement rapide d'interfaces Web pour projets ESP8266
*   Fast development Web Server for ESP8266 projects
*   http://www.projetsdiy.fr - 2017
*/
var express = require('express');
var request = require('request');
var bodyparser = require('body-parser');
var temp;
var humidity;

// Sert les fichiers publics (css, img) - Serve public files
var app = express();
// var baseUrl = "http://192.168.1.22";
var data = [];

app.set('view engine', 'ejs');

app.use(bodyparser.urlencoded({extended:true}));
app.use(express.static('public'));
app.use('/static', express.static(__dirname + '/public'));
app.locals.moment = require('moment');

app.get('/', function(req, res) {
  /////////////////single API request before rendering

  //Field 1 is temperature
  var url = 'https://api.thingspeak.com/channels/562202/fields/1.json?api_key=21M15LY0Z6JTQ3RH&results=2';
  // Temperature
  request(url, function (err, response, body) { 
    if(err){console.log(err)}
    else{
      var parsedData = JSON.parse(body);
      data[0] = (parsedData.feeds[0].field1);
      //time is UTC time (MTL+4)
      data[1] = (parsedData.feeds[0].created_at);
    }
  });
  // Humidity
  var url2 = 'https://api.thingspeak.com/channels/562202/fields/2.json?api_key=21M15LY0Z6JTQ3RH&results=2';
  request(url2, function (err, response, body) { 
    if(err){console.log(err)}
    else{
      var parsedData2 = JSON.parse(body);
      data[2] = (parsedData2.feeds[0].field2);
      data[3] = (parsedData2.feeds[0].created_at);
    }
  });
  res.render('index', {data:data});
});



app.listen(process.env.PORT, process.env.IP);
//app.listen(3000);