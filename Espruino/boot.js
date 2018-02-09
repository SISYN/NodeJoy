/**************************************************************************************************************
 * @file main.js
 * @author Dan Lindsey
 * @version 0.0.1
 * @copyright NodeJoy.com 2018
 * @description Espruino program for global NodeJoy devices
**************************************************************************************************************/
/**************************************************************************************************************
* Preliminary index
* 1. Define configs
* 2. Imported modules
* 3. Define globals
* 4. Program procedures
**************************************************************************************************************/
/**************************************************************************************************************
* Fatal errors:
* 1. Boot failure
* 2. WiFi connection failure
* 3. WebSocket connection failure
* 4. AccessPoint failure
* 5. AccessPoint WebServer failure
**************************************************************************************************************/
/**************************************************************************************************************
* Program procedures index
* 1. Boot
*   1-1. Validate boot
*   1-2. Connect to WiFi
*   1-3. Connect to WebSocket
*   1-4. Create WiFi Access Point
*   1-5. Create HTTP WebServer for the AP
* 2. Transmit data
*   2-1. HTTP request is sent to the AP WebServer via http://192.168.4.1/ from vanilla ESP32s
*   2-2. JSON from ?data= is accompanied by SELF.ID and relayed to Remote Server via WebSocket
**************************************************************************************************************/
/**************************************************************************************************************
* Planned implementations:
* 1. Make DEBUG an import module
**************************************************************************************************************/
function onInit() {
  /**************************************************************************************************************
   * Configuration & Options
   **************************************************************************************************************/
  var
    CONFIG = {
      'WIFI' : {
        'SSID' : 'LiWiFi 2.4Ghz',
        'PASS' : 'Lindsey7047'
      },
      'SOCKET' : {
        'HOST' : '45.33.12.205',
        'PORT' : '1337'
      },
      'AP' : {
        'SSID' : 'NAP--{ID}',
        'PASS' : '',
        'PORT' : '80'
      },
      'DEBUG' : {
        'CONSOLE' : 1
      }
    };

  /**************************************************************************************************************
   * Import modules
   **************************************************************************************************************/
  var
    wifi = require('Wifi'),
    http = require('http'),
    WebSocket = require("ws"),
    WebServer = require('WebServer'),
    cryptography = require('http://nodejoy.com/cdn/espruino/modules/cryptography.js');

  /**************************************************************************************************************
   * Declare globals
   **************************************************************************************************************/
  var
    WS,
    SELF = {},
    DEBUG = function(input , _designation) {
      _designation = (_designation || 'debug');
      if ( CONFIG.DEBUG.CONSOLE )
        console.log('['+_designation+'] ' + input);
    },
   /**************************************************************************************************************
    * Boot functions
    **************************************************************************************************************/
    Boot = function() {
      return new Promise(function(resolve, reject) {
        DEBUG('Boot initiating');
        // 5s limit on boot then assumes failure and reboots
        let boot_timeout = setTimeout(reject, 5000);
        Boot_Config().
        then(function() {
          DEBUG('Boot resolving');
          clearTimeout(boot_timeout);
          resolve();
        }, Boot_Config);

      }); // end new Promise()
    }, // end Boot
    Boot_Config = function() {
      // check for API/acct details and enter setup mode if not found (on file maybe?)
      return new Promise(function(resolve, reject) {
        DEBUG('Boot_Config initiating');
        if ( CONFIG.WIFI.SSID !== undefined ) {
          DEBUG('Boot_Config resolving');
          resolve(CONFIG);
        } else {
          reject(CONFIG);
        }
      }); // end new Promise()
    }, // end Boot_Config
   /**************************************************************************************************************
    * Connect functions
    **************************************************************************************************************/
    Connect = function() {
      return new Promise(function(resolve, reject) {
        DEBUG('Connect initiating');
        // 5s limit on boot then assumes failure and reboots
        let connect_timeout = setTimeout(reject, 5000);
        Connect_WiFi().
        then(Connect_AP , Connect_WiFi).
        then(Connect_AP_Server , Connect_AP).
        then(Connect_Socket , Connect_AP_Server).
        then(function() {
          DEBUG('Connect resolving');
          clearTimeout(connect_timeout);
          resolve();
        }, Connect_Socket);

      }); // end new Promise()
    },
    Connect_WiFi = function() {
      return new Promise(function(resolve, reject) {
        DEBUG('Connect_WiFi initiating');
        try {
          DEBUG('Connect_WiFi trying connection');
          DEBUG(CONFIG.WIFI.SSID + ' - ' + CONFIG.WIFI.PASS);
          // This prevents the occassional freeze during Connect_WiFi
          //var wifi_preliminary_timeout = setTimeout(Connect_WiFi, 250);
          wifi.connect(CONFIG.WIFI.SSID, {password: CONFIG.WIFI.PASS}, function() {
            let ip_object = wifi.getIP();
            DEBUG(CONFIG.WIFI.SSID + ' issued IP ' + ip_object.ip);
            //clearTimeout(wifi_preliminary_timeout);
            if ( ip_object.ip != '0.0.0.0' ) {
              SELF.ID = cryptography.encrypt(ip_object.mac);
              DEBUG('Connect_WiFi resolving');
              resolve(ip_object);
            } else {
              DEBUG('Connect_WiFi restarting');
              Connect_WiFI();
            } // End if ( success ) else
          }); // end wifi.connect()
        } catch(exception) {
          DEBUG('Connect_WiFi exception '+exception);
          DEBUG('Connect_WiFi restarting in 750ms');
          setTimeout(Connect_WiFi, 750);
        }
      }); // end new Promise()
    },
    Connect_Socket = function() {
      return new Promise(function(resolve, reject) {
        DEBUG('Connect_Socket initiating');
        DEBUG(CONFIG.SOCKET.HOST + ':' + CONFIG.SOCKET.PORT);
        try {
          WS = new WebSocket( CONFIG.SOCKET.HOST , {
            port: CONFIG.SOCKET.PORT,
            keepAlive: 750
          });
          WS.on('error', function() {
            DEBUG('Socket error has occurred');
          });
          WS.on('open', function() {
            DEBUG("Connected to socket");
            DEBUG('Connect_Socket resolving');
            resolve(WS);
          });
          WS.on('message', function(data) {
            DEBUG('Received data: ');
            DEBUG(data);
          });
        } catch(exception) {
          // If unsuccessful, restart the attempt
          DEBUG('Socket connection failed');
          DEBUG('Connect_Socket restarting in 750ms');
          setTimeout(Connect_Socket, 750);
        }
      }); // end new Promise()
    },
    Connect_AP = function() {
      return new Promise(function(resolve, reject) {
        let
          auth = (CONFIG.AP.PASS.length >= 8) ? { password: CONFIG.AP.PASS } : {},
          ssid = ((CONFIG.AP.SSID.indexOf('{ID}') >= 0) ? CONFIG.AP.SSID.replace('{ID}', SELF.ID) : CONFIG.AP.SSID).substr(0,31);

        DEBUG('Connect_AP initiating');
        DEBUG('Creating AP '+ssid);

        wifi.startAP(ssid, auth, function(err) {
          if (err) throw err;
          DEBUG('AP connected');
          DEBUG('Connect_AP resolving');
          resolve();
        });
      }); // end new Promise()
    },
    Connect_AP_Server = function() {
      return new Promise(function(resolve, reject) {
        DEBUG('Connect_AP_Server initiating');

        function request(req, res) {
          let parsed = url.parse(req.url, true);

          if ( req.url == '/favicon.ico' ) {
            console.log('no favicon');
            return;
          }

          console.log(req);
          Transmit(req);

          res.end('Request complete');
        }
        http.createServer(request).listen(CONFIG.AP.PORT);

        DEBUG('Connect_AP_Server resolving');
        resolve();
      }); // end new Promise()
    }, // end Connect_AP_Server
    /**************************************************************************************************************
    * Relay functions - maybe a rename of transmit, maybe different, who knows, its late.
    **************************************************************************************************************/
    Relay = function(req) {
      DEBUG('Relay initiating');
      return new Promise(function(resolve, reject){
        DEBUG('Relay resolving');
        resolve();
      });
    },
    /**************************************************************************************************************
    * Transmit functions
    **************************************************************************************************************/
    Transmit = function(req) {
      DEBUG('Transmit initiating');
      return new Promise(function(resolve, reject){
        DEBUG('Transmit resolving');
        resolve();
      });
    };



   /**************************************************************************************************************
   * Program procedures index
   * 1. Boot
   *   1-1. Validate boot
   * 2. Connect
   *   2-1. Connect to WiFi
   *   2-2. Connect to WebSocket
   *   2-3. Create WiFi Access Point
   *   2-4. Create HTTP WebServer for the AP
   * 3. Transmit data
   *   3-1. Keep-alive worker sends constant pings to keep connection alive
   *   3-2. HTTP request is sent to the AP WebServer via http://192.168.4.1/ from vanilla ESP32s
   *   3-3. JSON from ?data= is accompanied by SELF.ID and relayed to Remote Server via WebSocket
   **************************************************************************************************************/

   // If boot promise is rejected (after 5s), restart boot; otherwise, intiate transmitter
   Boot().
   then(Connect , Boot).
   then(function() {
     DEBUG('Sending sample data');
     WS.send('Sample #1');
   }).
   then(Transmit , Connect).
   then(function() {
     DEBUG('Waiting patiently :)');
   });


  let NewServer = function() {
    console.log('Init NewServer');
    let webs = new WebServer({
      port: 80,
      default_type: 'text/plain'
    });
    console.log('NewServer created');
    webs.on('start', function(request, WebServer){
      console.log('WebServer listening on port ' + WebServer.port);
    });

    webs.on('request', function(request, response, parsedUrl, WebServer){
      console.log('WebServer requested', parsedUrl);
    });

    webs.on('error', function(error, WebServer){
      console.log('WebServer Error', error);
    });
  };

  NewServer();



}
save();


/**************************************************************************************************************
* Footnotes
* 1. Versions are designated as [PRODUCTION_VERSION . BETA_VERSION . ALPHA_VERSION]
*   1-1. Any change made to the program in ALPHA stage should increment the LAST version number
*   1-2. Any change made to the program in BETA stage should increment the MIDDLE version number
*   1-3. Any change made to the program in PRODUCTION stage should increment the FIRST version number
**************************************************************************************************************/
