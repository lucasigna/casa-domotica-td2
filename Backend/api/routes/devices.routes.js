const express = require("express");
const routerDevices = express.Router();
const controller = require("../controllers/index.controller");

// Endpoints
routerDevices.get('/', controller.getDevices); // Obtener el estado de todos los equipos
routerDevices.post('/', controller.getScheduleDevices); // Obtener el estado de todos los equipos
routerDevices.post('/lights/:id/on', controller.turnOnLight); // Encender una luz específica
routerDevices.post('/lights/:id/off', controller.turnOffLight); // Apagar una luz específica
routerDevices.post('/blinds/:id/on', controller.openBlind); // Abrir una persiana específica
routerDevices.post('/blinds/:id/off', controller.closeBlind); // Cerrar una persiana específica
routerDevices.post('/lights/:id/program/on', controller.programLightOn); // Establecer la programación de una luz específica
routerDevices.post('/lights/:id/program/off', controller.programLightOff); // Establecer la programación de una luz específica
routerDevices.post('/blinds/:id/program/on', controller.programBlindOn); // Establecer la programación de una persiana específica
routerDevices.post('/blinds/:id/program/off', controller.programBlindOff); // Establecer la programación de una persiana específica

routerDevices.post('/alarms/on', controller.turnOnAlarm); // WEB
routerDevices.post('/alarms/off', controller.turnOffAlarm); // WEB
routerDevices.post('/alarms/detected', controller.alarmDetected); // MICRO
routerDevices.post('/alarms/resolved', controller.alarmResolved); // WEB
routerDevices.get('/alarms/events', controller.alarmEvents); // WEB

module.exports = routerDevices;
