const URL_PATH = process.env.REACT_APP_URL_PATH;

export const getData = async(setData) => {
    fetch(URL_PATH)
      .then(response => response.json())
      .then(data => setData(data))
      .catch(error => console.error(error))
}

export const updateDeviceState = async(type,endpoint,id) => {
    fetch(`${URL_PATH}/${type}s/${id}/${endpoint}`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          }
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error(error))
}

export const resolveAlarm = async() => {
    fetch(`${URL_PATH}/alarms/resolved`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          }
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error(error))
}

export const updateAlarmState = async(endpoint) => {
    fetch(`${URL_PATH}/alarms/${endpoint}`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          }
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error(error))
}

export const updateDeviceSchedule = async(type,endpoint,id, startTime, endTime) => {
  console.log(`${URL_PATH}/${type}s/${id}/program/${endpoint}`);
    fetch(`${URL_PATH}/${type}s/${id}/program/${endpoint}`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({startTime: startTime, endTime: endTime})
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error(error))
}

export const getAlarmState = async(setState, setDetection, state) => {
  fetch(`${URL_PATH}/alarms/state`)
    .then(response => response.json())
    .then( (data) => {
      if(state) {
        setState(data.state);
      }
      setDetection(data.detected);
    })
    .catch(error => console.error(error))
}