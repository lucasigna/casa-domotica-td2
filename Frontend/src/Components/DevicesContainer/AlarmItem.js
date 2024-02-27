import React, {  useEffect, useState } from 'react'
import Switch from '@mui/material/Switch';
import { Stack, Typography } from '@mui/material';
import { styled } from '@mui/material/styles';
import { getAlarmState, updateAlarmState } from '../../api/functions';
import { io } from 'socket.io-client';


const AntSwitch = styled(Switch)(({ theme }) => ({
  width: 35,
  height: 19,
  padding: 0,
  display: 'flex',
  '&:active': {
    '& .MuiSwitch-thumb': {
      width: 15,
    },
    '& .MuiSwitch-switchBase.Mui-checked': {
      transform: 'translateX(13px)',
    },
  },
  '& .MuiSwitch-switchBase': {
    padding: 2,
    '&.Mui-checked': {
      transform: 'translateX(16px)',
      color: '#fff',
      '& + .MuiSwitch-track': {
        opacity: 1,
        backgroundColor: theme.palette.mode === 'dark' ? '#177ddc' : '#1890ff',
      },
    },
  },
  '& .MuiSwitch-thumb': {
    boxShadow: '0 2px 4px 0 rgb(0 35 11 / 20%)',
    width: 15,
    height: 15,
    borderRadius: 10,
    transition: theme.transitions.create(['width'], {
      duration: 200,
    }),
  },
  '& .MuiSwitch-track': {
    borderRadius: 16 / 2,
    opacity: 1,
    backgroundColor:
      theme.palette.mode === 'dark' ? 'rgba(255,255,255,.35)' : 'rgba(0,0,0,.25)',
    boxSizing: 'border-box',
  },
}));

export const AlarmItem = ({state, setAlert}) => {
    
    const [isOn, setIsOn] = useState(state)
    
    const handleStatusChange = (e) => {
      setIsOn(e.target.checked)
      
      const endpoint = isOn ? 'off' : 'on';
      updateAlarmState(endpoint)
      .then(data => console.log(data))
      .catch(error => console.error(error));
    }

    useEffect(() => {
      getAlarmState(setIsOn, setAlert, true)
      setInterval(() => {
        getAlarmState(setIsOn, setAlert, false)
      }, 1000);
    }, [])
    

  return (
    <div className="alarmItem">
        <div>
            <img className='simbolo' src={require(`../../assets/alarm.png`)} alt='Simbolo luz' />
            <p className="name">Alarma</p>
        </div>
        <Stack className='switchOnOff' direction="row" spacing={1} alignItems="center">
          <Typography>OFF</Typography>
          <AntSwitch inputProps={{ 'aria-label': 'ant design' }} onChange={handleStatusChange} checked={isOn}/>
          <Typography>ON</Typography>
        </Stack>
    </div>
  )
}
