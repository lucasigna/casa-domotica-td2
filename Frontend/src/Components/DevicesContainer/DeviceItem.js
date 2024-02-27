import React, {  useState } from 'react'
import Switch from '@mui/material/Switch';
import { Stack, Typography } from '@mui/material';
import { styled } from '@mui/material/styles';
import TimeRangePicker from './TimeRangePicker';
import Button from '@mui/material/Button';
import { updateDeviceSchedule, updateDeviceState } from '../../api/functions';
import Swal from 'sweetalert2'


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

export const DeviceItem = ({type,name,id,start,end,program,state}) => {

    
    const [startTime, setStartTime] = useState(start);
    const [endTime, setEndTime] = useState(end);
    const [isScheduled, setIsScheduled] = useState(program)
    const [isOn, setIsOn] = useState(state)
    const [changes, setChanges] = useState(false)

    const label = { inputProps: { 'aria-label': 'Switch schedule' } };

    const handleProgramChange = (e) => {
      setIsScheduled(e.target.checked)
      setChanges(true)
    }
    
    const handleStatusChange = (e) => {
      setIsOn(e.target.checked)
      
      const endpoint = isOn ? 'off' : 'on';
      updateDeviceState(type, endpoint, id)
      .then(data => console.log(data))
      .catch(error => console.error(error));
    }

    const handleSubmit = () => {
      
      const endpoint = isScheduled ? 'on' : 'off';
      updateDeviceSchedule(type, endpoint, id, startTime, endTime)
      .then( (data) => { 
        console.log(data);
        Swal.fire({
          title: 'Se guardó la configuración',
          icon: 'success',
          confirmButtonText: 'OK'
        })
        setChanges(false)
      })
      .catch(error => console.error(error));

    }

    
    

  return (
    <div className="deviceItem">
        <img className='simbolo' src={require(`../../assets/${type}_${isOn ? 'on' : 'off'}.png`)} alt='Simbolo luz' />
        <p className="name">{name}</p>
        <div className="programSwitch">
          <p>Programado</p>
          <Switch {...label} onChange={handleProgramChange} checked={isScheduled}/>
          <TimeRangePicker setChanges={setChanges} setStartTime={setStartTime} startTime={startTime} endTime={endTime} setEndTime={setEndTime} isScheduled={isScheduled}/>
          <Button onClick={handleSubmit} className={changes ? 'btnConfirmProgram' : 'btnConfirmProgramDisabled'} variant="contained" disabled={!changes}>Confirmar</Button>
        </div>
        <Stack className='switchOnOff' direction="row" spacing={1} alignItems="center">
          <Typography>OFF</Typography>
          <AntSwitch inputProps={{ 'aria-label': 'ant design' }} onChange={handleStatusChange} checked={isOn}/>
          <Typography>ON</Typography>
        </Stack>
    </div>
  )
}
