require('dotenv').config()
const admin = require("firebase-admin");

const serviceAccount = require("../../serviceAccountKey");

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: process.env.DB_URL
});


class Contenedor {

  constructor() {
    // Referencia a la base de datos en Firebase
    this.database = admin.database();
    this.alarm = {
      isActive: false,
      detected: false
    }
  }

  async setLight(id,data) {

    try {
      const ref = this.database.ref(`devices/lights/${id}`);
      const resp = await ref.once('value');
      const device = resp.val();
      const update = {...device, ...data}
      await ref.update(update);
      console.log('SUCCESS');
      return {status: 'Dispostivo actualizado', device: update}
    } catch (error) {
      console.log('ERROR: '+error);
      return {error: error};
    }

  }

  async setBlind(id,data) {

    try {
      const ref = this.database.ref(`devices/blinds/${id}`);
      
      const resp = await ref.once('value');
      const device = resp.val();
      const update = {...device, ...data}
      await ref.update(update);
      console.log('SUCCESS');
      return {status: 'Dispostivo actualizado', device: update}
    } catch (error) {
      console.log('ERROR: '+error);
      return {error: error};
    }

  }

  async getAllDevices() {
    try {
      const lightsRef = this.database.ref('devices/lights');
      const blindsRef = this.database.ref('devices/blinds');
      const [lightsSnapshot, blindsSnapshot] = await Promise.all([
        lightsRef.once('value'),
        blindsRef.once('value'),
      ]);
      const lights = lightsSnapshot.val() || {};
      const blinds = blindsSnapshot.val() || {};
      const lightsArray = Object.values(lights).map(device => ({...device, type: 'light'}));
      const blindsArray = Object.values(blinds).map(device => ({...device, type: 'blind'}));
      return [...lightsArray, ...blindsArray];
    } catch (error) {
      console.log(error);
      return {error: error};
    }
  }

  setAlarm(state) {
    this.alarm.isActive = state
  }

  setDetection(state) {
    this.alarm.detected = state
  }

  getDetection() {
    return this.alarm.detected
  }
  

}

const devices = new Contenedor();

module.exports = devices;