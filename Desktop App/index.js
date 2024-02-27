const { app, BrowserWindow, ipcMain } = require('electron');
const { SerialPort } = require('serialport');
const { list } = require('@serialport/list');
const usbDetect = require('node-usb-detection');


let mainWindow;

app.on('ready', async () => {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    }
  });
  mainWindow.loadFile('./public/index.html');

  const devices = usbDetect.list()
  const device = devices.find(isLpc845Brk)
  if(device){
    mainWindow.webContents.send('device-connected');
  }
  // Detectar conexión y desconexión de la placa LPC845-BRK
  usbDetect.add(deviceAttached);
  usbDetect.remove(deviceDetached);

  let bufferData = Buffer.alloc(0); // Inicializa un buffer vacío

  // Enumerar puertos COM disponibles
  SerialPort.list().then((ports) => {
    const comPort = ports.find((port) => port.manufacturer === 'NXP LPC11Uxx VCOM'); // Reemplaza 'YourMicrocontrollerManufacturer' con el nombre del fabricante de tu microcontrolador
    if (comPort) {
      // Configura el puerto serie con la información correcta
      const port = new SerialPort({
        path: comPort.path,
        baudRate: 9600, // Ajusta la velocidad de baudios según la configuración de tu microcontrolador
      }, false);

      // Escuchar eventos del puerto serial
      port.on('data', (data) => {
        // Agregar los datos al buffer acumulado
        bufferData = Buffer.concat([bufferData, data]);

        // Verificar si se ha recibido el carácter de finalización "}"
        const endIndex = bufferData.indexOf('}');
        if (endIndex !== -1) {
          // Extraer la cadena JSON desde el inicio hasta el carácter de finalización
          const jsonString = bufferData.slice(0, endIndex + 1).toString('utf-8');

          // Analizar la cadena JSON para obtener un objeto JavaScript
          try {
            const jsonObject = JSON.parse(jsonString);
            console.log(jsonObject);
            console.log(jsonObject.nombre_wifi);
            console.log(jsonObject.password_wifi);
            mainWindow.webContents.send('get-data-wifi', jsonObject);
            // Limpia el bufferData después de procesar los datos
            bufferData = bufferData.slice(endIndex + 1);
          } catch (error) {
            console.error('Error al analizar la cadena JSON:', error);
          }
        }
      });

      // Envía datos al microcontrolador cuando se recibe un mensaje del renderer process
      ipcMain.on('send-data', (event, wifiName, wifiPassword, get) => {
        // Envía los datos a través del puerto serie
        console.log('Nombre del WiFi:', wifiName);
        console.log('Contraseña del WiFi:', wifiPassword);
        if (get) {
          port.write('GET', (err) => {
            if (err) {
              console.error('Error al enviar datos al microcontrolador:', err);
            } else {
              console.log('Datos enviados al microcontrolador');
            }
          });
        } else {
          port.write(`${wifiName}\n${wifiPassword}\n`, (err) => {
            if (err) {
              console.error('Error al enviar datos al microcontrolador:', err);
            } else {
              console.log('Datos enviados al microcontrolador');
            }
          });
        }
      });
    } else {
      console.error('No se encontró el puerto COM del microcontrolador.');
    }
  });

});

app.on('window-all-closed', () => {

  // Cerrar la aplicación solo en macOS
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

function deviceAttached(device) {
  console.log('dispositivo conectado');
  console.log(isLpc845Brk(device));
  if (isLpc845Brk(device)) {
    // La placa LPC845-BRK se ha conectado
    mainWindow.webContents.send('device-connected');
  }
}

function deviceDetached(device) {
  console.log('dispositivo desconectado');
  if (isLpc845Brk(device)) {
    // La placa LPC845-BRK se ha desconectado
    mainWindow.webContents.send('device-disconnected');
  }
}

function isLpc845Brk(device) {
  console.log(device.deviceDescriptor.idVendor);
  console.log(typeof (device.deviceDescriptor.idVendor));
  // Reemplaza 'vendor_id' y 'product_id' con los valores correspondientes de tu placa LPC845-BRK
  return device.deviceDescriptor.idVendor == 8137 && device.deviceDescriptor.idProduct == 306;
}