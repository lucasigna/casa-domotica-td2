import React, { useEffect, useState } from 'react'
import './DevicesContainer.scss';
import { DeviceItem } from './DeviceItem';
import CircularProgress from '@mui/material/CircularProgress';
import { getData } from '../../api/functions';
import { AlarmItem } from './AlarmItem';


export const DevicesContainer = ({setAlert}) => {

  
  const [data, setData] = useState(null)

  useEffect( () => {
    getData(setData)
  }, [])

  return (
    <div className="devicesContainer">
      <AlarmItem setAlert={setAlert} state={true}/>
      {data ? (
        data.map((device) => (
          <DeviceItem
            id={device.id}
            type={device.type}
            name={device.name}
            program={device.isScheduled}
            start={device.startTime}
            end={device.endTime}
            state={device.state}
          />
        ))
      ) : (
        <CircularProgress />
      )}
    </div>
  )
}
