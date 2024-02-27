require('dotenv').config()
const express = require('express')
const router = require('./routes/index.routes')
const routerDevices = require('./routes/devices.routes')
const cors = require('cors')
const scheduleDevices = require('./utils/scheduleDevices')
const app = express();

app.use(express.json());
app.use(express.urlencoded({extended: true}))
app.use(cors())
app.get('/events', (req, res) => {
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');
    res.flushHeaders();
  
    const data = true;

    res.write(`data: ${data}\n\n`);
    res.end();
  });
app.use("/api/v1/devices", routerDevices)
app.use("/api", router)

// Llamar a la función cada minuto con setInterval
setInterval(()=>{
  console.log('HOLA MUNDO');
}, 60000);

app.listen(process.env.PORT, () => console.log(`Server started on port http://localhost:${process.env.PORT}`))


