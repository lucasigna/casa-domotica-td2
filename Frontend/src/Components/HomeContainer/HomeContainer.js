import React, { useEffect } from 'react'
import './HomeContainer.scss';
import { DevicesContainer } from '../DevicesContainer/DevicesContainer';
import { isAdminLogin } from '../../firebase';


export const HomeContainer = ({setAlert}) => {


  const checkAdmin = async () => {
      const resp = await isAdminLogin()
      console.log(resp);
      if(!resp){
        // No esta logueado
        window.location.href = '/';
      }
  }

  useEffect( () => {
      checkAdmin()
  },[])

  return (
    <div className='homeContainer'>
        <h1 className="title">Control Casa Domótica</h1>
        <DevicesContainer setAlert={setAlert}/>
    </div>
  )
}
