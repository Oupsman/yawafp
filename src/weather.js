// JSON Online viewer : http://jsonviewer.stack.hu/
// https://api.forecast.io/forecast/af6d2d2f2302bede0951efb26c565b70/47.13469567993814,-2.161695759881867

var api_key = '';

var units = 0;
var type = 0;
var response;
var json;

var xhrRequest = function (url, type, callback) {
  console.log ("xhrRequest " + url + type);
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function parse_Weather (shift,json) {
  if (shift === json.length) {
    console.log ("End of story");
  } else {
    // Conditions
    var conditions=255;
    
    var icon = json.icon;   
    // possible values
    // clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night
    
    switch (icon) {
      case "clear-day": 
        conditions = 10;
      break;
      case "clear-night":
        conditions = 31;
      break;
      case "rain":
        conditions = 11;
      break;
      case "snow":
        conditions = 16;
      break;
      case "sleet":
        conditions = 18;
      break;
      case "wind":
        conditions = 24;
      break;
      case "fog":
        conditions = 21;
      break;
      case "cloudy":
        conditions = 26;
      break;
      case "partly-cloudy-day":
        conditions = 30;
      break;
      case "partly-cloudy-night":
        conditions = 29;
      break;
      default:
        conditions = 255;
      break;
    }
    
    var dictionary = {
      "KEY_SHIFT"         : shift,
      "KEY_TIME"          : json[shift].time,
      "KEY_CONDITIONS"		: conditions,
      "KEY_SUMMARY"       : json[shift].summary,
      "KEY_TEMPERATURE"   : json[shift].temperature,
      "KEY_FEELSLIKE"     : json[shift].apparentTemperature,
      "KEY_WIND"          : json[shift].windSpeed,
      "KEY_WINDDIR"       : json[shift].windBearing,
    };
    
    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log("Weather info sent to Pebble successfully! Message number : "  + shift );
        parse_Weather(shift+1,json);
      },
      function(e) {
        console.log("Error sending weather info to Pebble! Message number : " + shift);
        console.log ("Retring");
        parse_Weather (shift,json);
      }
    ); 
  }
}

function ask_Weather(pos) {
  var forecast_url = 'https://api.forecast.io/forecast/' + api_key + '/' + pos.coords.latitude + ',' + pos.coords.longitude ;
  //var forecast_url = 'https://api.forecast.io/forecast/' + api_key + '/47.13469567993814,-2.161695759881867';
  var shift;

  switch (units) {
  case 0: 
    forecast_url = forecast_url + "?units=si";
    break;
  case 1:
    forecast_url = forecast_url + "?units=us";
    break;
  default:
    forecast_url = forecast_url + "?units=us";
  break;
  
  }
  switch (type) {
    case 0:
      forecast_url = forecast_url + "&exclude=minutely,daily,hourly,alerts,flags";
      break;
      
    case 1:
      forecast_url = forecast_url + "&exclude=minutely,daily,currently,alerts,flags";
      break;
    case 2:
      forecast_url = forecast_url + "&exclude=minutely,hourly,currently,alerts,flags";
      break;
  }
    
  console.log ("Forecast URL : " + forecast_url);
               
  xhrRequest(forecast_url, 'GET', 
    function(responseText) {
      console.log ("In callback");
      // responseText contains a JSON object with weather info
      // console.log ("Response text : "+ responseText);
      var taille = responseText.length;      
    if (taille === 0 ) {
      console.log ("Response is empty, I think there is a problem somewhere") ; 
    } else {
    // console.log ("Forecast response : " + responseText);
 
    response = JSON.parse(responseText);
     console.log (" ---- You asked type : " + type );
      
    switch (type) {
    case 0:
      json = response.currently;
      parse_Weather (json);
    break;
    case 1:
      shift = 0;
      json = response.hourly.data;
      parse_Weather (shift,json);  
    break;
    case 2:
      shift = 0;
      json = response.daily.data;
      parse_Weather (shift,json);
    break;
    default:
      console.log ("Shouldn't be here");
      json = response.currently;
      parse_Weather (shift,json);
    break;
	}
	}
    }
  );
} 

function locationError(err) {
  console.log("Error requesting location!");
}

// Listen for when the app is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function(e) {
      
      console.log("AppMessage received!");  
      console.log('Received message: ' + JSON.stringify(e.payload));
      units = localStorage.getItem("units");
      
      type = parseInt(e.payload.KEY_TYPE);
      
      console.log (" ---- You asked type : " + type );
      
      navigator.geolocation.getCurrentPosition(
        ask_Weather,
        locationError,
        {timeout: 15000, maximumAge: 60000}
      );
    } 
);

Pebble.addEventListener("showConfiguration", 
    function(e) {
      console.log("showConfiguration Event");
    }
);

Pebble.addEventListener("webviewclosed", function(e) {
  // console.log("Configuration window closed");
  // console.log(e.type);
  // console.log(e.response);
  var configuration = JSON.parse(e.response);
  //Pebble.sendAppMessage(configuration);
  units = configuration.units;
  
  // console.log ("Unit : " + temp_unit);
  // console.log ("gps : " + gps);
  // console.log ("Town : " + town);
  localStorage.setItem("units", units);

  // Force weather refresh when you change the settings


});
