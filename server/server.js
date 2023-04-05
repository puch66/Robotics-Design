var server_port=80;
var esp32_ip = '192.168.1.102';
var esp32_port = '80';

/************************* GET SERVER IP ***************************/
'use strict';
const { networkInterfaces } = require('os');
const nets = networkInterfaces();
const results = Object.create(null); // Or just '{}', an empty object
for (const name of Object.keys(nets)) {
    for (const net of nets[name]) {
        // Skip over non-IPv4 and internal (i.e. 127.0.0.1) addresses
        // 'IPv4' is in Node <= 17, from 18 it's a number 4 or 6
        const familyV4Value = typeof net.family === 'string' ? 'IPv4' : 4
        if (net.family === familyV4Value && !net.internal) {
            if (!results[name]) {
                results[name] = [];
            }
            results[name].push(net.address);
        }
    }
}
//console.log(results);

/*************************** MODULI NODE ***************************/
var express = require('express');
var request2server = require('request');
var bodyParser = require('body-parser');
var app = express();
app.use(bodyParser.urlencoded({extended : true}));
app.use(bodyParser.json());

/*************************** OPERAZIONI GESTIONE CLIENT-SERVER **************************/
//GET PAGINA INIZIALE
app.get('/',function(req,res){
    if(req.query.esp32_ip) {
        esp32_ip = req.query.esp32_ip;
        console.log("New ip for esp32: " + esp32_ip);
    }
    res.sendFile(__dirname + '/public/index.html');
});

app.post('/move',function(req,res){
    var servo_channel = req.body.servo_channel;
    var angle = req.body.angle;
    var url = 'http://' + esp32_ip + ':' + esp32_port + '?servo_channel=' + servo_channel + '&angle=' + angle;
    //console.log(url);
    request2server({
        url: url, 
        method: 'GET',
        headers: {'content-type': 'application/json'}
    }, function(error, response, body){
        if(error) {
            console.log(error);
            res.redirect('/error');
        }
        else 
        {
            res.redirect('/');
        }
    });
});

app.get('/error', function(req,res){
    res.sendFile(__dirname + '/public/404.html');
});

app.get('/esp32',function(req,res){
    if(req.query.esp32_ip) {
        esp32_ip = req.query.esp32_ip;
        console.log("New ip for esp32: " + esp32_ip);
        res.send("I'm glad to hear you, " + esp32_ip);
    }
});

/*************************** SERVER IN ASCOLTO***************************/
app.listen(server_port);
console.log('Server running at '+ results["Wi-Fi"][0] + ':' + server_port);