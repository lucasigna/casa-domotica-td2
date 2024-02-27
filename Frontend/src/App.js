import './App.scss';
import { BrowserRouter,Route,Routes,Navigate } from 'react-router-dom'
import { HomeContainer } from './Components/HomeContainer/HomeContainer';
import { useEffect, useState } from 'react';
import { AlertContainer } from './Components/AlertContainer/AlertContainer';
import { resolveAlarm } from './api/functions';
import { LoginContainer } from './Components/LoginContainer/LoginContainer';
import { initializeApp } from "firebase/app";

function App() {

  const [alert, setAlert] = useState(false)
  
  const removeAlert = async () => {
    resolveAlarm()
    setAlert(false)
  }
  

  useEffect(() => {
    const eventSource = new EventSource('http://localhost:8080/api/v1/devices/alarms/events');

    eventSource.onmessage = (event) => {
      setAlert(event.data == 'true')
    };

    return () => {
      eventSource.close();
    };

  }, []);

  return (
    <BrowserRouter>
  
        {alert && <AlertContainer onClick={removeAlert}/>}
        <Routes>
        
            <Route exact path="/" element={<LoginContainer/>}/>
            <Route exact path="/home" element={<HomeContainer setAlert={setAlert}/>}/>
            <Route exact path="*" element={<Navigate to="/"/>}/>
        </Routes>

    </BrowserRouter>
  );
}

export default App;
