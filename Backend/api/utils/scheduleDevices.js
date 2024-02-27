const db = require("../data/data")

const scheduleDevices = async () => {
    // Obtener la lista de dispositivos desde la base de datos
    const devices = await db.getAllDevices();
  
    // Obtener la hora actual
    const now = new Date();
  
    // Iterar sobre todas los dispositivos programados
    for (const device of devices) {
      if (device.isScheduled) {
        // Si el dispositivo está programado, verificar si es el momento de prender o apagar
        const start = new Date(now.getFullYear(), now.getMonth(), now.getDate(), device.startTime.split(':')[0], device.startTime.split(':')[1]);
        const end = new Date(now.getFullYear(), now.getMonth(), now.getDate(), device.endTime.split(':')[0], device.endTime.split(':')[1]);
  
        if (now >= start && now <= end) {
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
  };
  
module.exports = scheduleDevices
  