const controller = {};
const moment = require('moment-timezone');

const db = require("../data/data");
//const scheduleDevices = require("../utils/scheduleDevices");

controller.getDevices = async (req, res) => {
  console.log('get');
  const resp = await db.getAllDevices();
  res.json(resp);
};

controller.getScheduleDevices = async (req, res) => {
  console.log('AUTOMATIC');
  await scheduleDevices();
  res.json({message: 'Mensaje recibido'})
};

//! Endpoints luces
controller.turnOnLight = async (req, res) => {
    const id = req.params.id;
    const light = {
        state: true
    };
    const resp = await db.setLight(id,light);
    res.json(resp);
};

controller.turnOffLight = async (req, res) => {
    const id = req.params.id;
    const light = {
      state: false
    };
    const resp = await db.setLight(id,light);
    res.json(resp);
};

controller.programLightOn = async (req, res) => {
  const id = req.params.id;
  const { startTime, endTime } = req.body;
  const light = {
    isScheduled: true,
    startTime: startTime,
    endTime: endTime
  };
  const resp = await db.setLight(id,light);
  res.json(resp);
};

controller.programLightOff = async (req, res) => {
  const id = req.params.id;
  const light = {
    isScheduled: false,
  };
  const resp = await db.setLight(id,light);
  res.json(resp);
};

//! Endpoints persianas
controller.openBlind = async (req, res) => {
  const id = req.params.id;
  const blind = {
    state: true
  };
  const resp = await db.setBlind(id,blind);
  res.json(resp);
};

controller.closeBlind = async (req, res) => {
  const id = req.params.id;
  const blind = {
    state: false
  };
  const resp = await db.setBlind(id,blind);
  res.json(resp);
};

controller.programBlindOn = async (req, res) => {
  const id = req.params.id;
  const { startTime, endTime } = req.params.body;
  const blind = {
    isScheduled: true,
    startTime: startTime,
    endTime: endTime,
  };
  const resp = await db.setBlind(id,blind);
  res.json(resp);
};

controller.programBlindOff = async (req, res) => {
  const id = req.params.id;
  const blind = {
    isScheduled: false
  };
  const resp = await db.setBlind(id,blind);
  res.json(resp);
};

//! Alarmas

controller.turnOnAlarm = async (req, res) => {
  db.setAlarm(true)
  res.json({success: 'La alarma está activada'})
}
controller.turnOffAlarm = async (req, res) => {
  db.setAlarm(false)
  res.json({success: 'La alarma está desactivada'})
}
controller.alarmDetected = async (req, res) => {
  if(db.alarm.isActive) {
    db.setDetection(true)
    res.json({message: '¡ALERTA! ¡Se detectó movimiento!'})
  } else {
    db.setDetection(false)
    res.json({message: 'La alarma esta desactivada'})
  }
}
controller.alarmResolved = async (req, res) => {
  db.setDetection(false)
  res.json({message: 'Alerta reestablecida'})
}
controller.alarmEvents = async (req, res) => {
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');
  res.flushHeaders();
  res.write(`data: ${db.getDetection()}\n\n`);
  res.end();
}

const scheduleDevices = async () => {
  // Obtener la lista de dispositivos desde la base de datos
  console.log('1');
  const devices = await db.getAllDevices();
  console.log('2');

  // Obtener la hora actual en la zona horaria de Argentina (ART)
  const nowArgentina = moment().tz('America/Argentina/Buenos_Aires');

  // Obtener el timestamp UNIX de la hora actual en UTC
  const nowUTC = nowArgentina.clone().tz('UTC').valueOf();

  // Iterar sobre todas los dispositivos programados
  for (const device of devices) {
    console.log('4');
    console.log(device.isScheduled);
    if (device.isScheduled) {
      console.log('LOOOOOL');
      // Obtener los timestamps UNIX de las horas de inicio y finalización en UTC
      const startUTC = nowArgentina.clone().set({
        hour: parseInt(device.startTime.split(':')[0]),
        minute: parseInt(device.startTime.split(':')[1]),
        second: 0,
        millisecond: 0
      }).tz('UTC').valueOf();

      const endUTC = nowArgentina.clone().set({
        hour: parseInt(device.endTime.split(':')[0]),
        minute: parseInt(device.endTime.split(':')[1]),
        second: 0,
        millisecond: 0
      }).tz('UTC').valueOf();
      console.log(nowUTC);
      console.log(startUTC);
      console.log(endUTC);
      console.log(device);
      if (nowUTC >= startUTC && nowUTC <= endUTC) {
        // Es el momento de prender el dispositivo
        if (!device.state) {
          // Si el dispositivo estaba apagado, actualizar su estado en la base de datos
          device.state = true;
          if(device.type == 'light'){
            console.log('LOL');
            await db.setLight(device.id,device);
          }
          if(device.type == 'blind'){
            console.log('LOL');
            await db.setBlind(device.id,device);
          }
          console.log(`${device.name} encendida`);
        }
      } else {
        // Es el momento de apagar el dispositivo
        if (device.state) {
          // Si el dispositivo estaba prendido, actualizar su estado en la base de datos
          device.state = false;
          if(device.type == 'light'){
            console.log('LOL');
            await db.setLight(device.id,device);
          }
          if(device.type == 'blind'){
            console.log('LOL');
            await db.setBlind(device.id,device);
          }
          console.log(`${device.name} apagada`);
        }
      }
    }
  }
  console.log('3');
};

module.exports = controller;
