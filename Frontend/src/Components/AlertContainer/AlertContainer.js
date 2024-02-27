import React from 'react'
import './AlertContainer.scss'

export const AlertContainer = ({onClick}) => {



    return (
        <div className='alertContainerBack'>
            <div className='alertContainer'>
                <p>ALERTA</p>
                <p>Se detectó movimiento en tu casa</p>
                <button onClick={onClick}>OK</button>
            </div>
        </div>
    )
}
