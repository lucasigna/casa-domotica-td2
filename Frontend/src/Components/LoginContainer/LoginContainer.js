import React, { useState } from 'react';
import { FaSpinner } from 'react-icons/fa'; // Importa un ícono de carga
import './LoginContainer.scss';
import { loginAdmin } from '../../firebase';

export const LoginContainer = () => {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [isLoading, setIsLoading] = useState(false); // Estado para controlar la visibilidad del spinner

  const handleLogin = async () => {
    setIsLoading(true); // Muestra el spinner al iniciar el proceso de inicio de sesión

    const resp = await loginAdmin(username, password)
    if (resp) {
      // Se logueo
      window.location.href = '/home';
    } else {
      // No se logueo

    }

    // Luego de completar el proceso de inicio de sesión, oculta el spinner
    setIsLoading(false);
  };

  return (
    <div className="login-container">
      <h2>Login</h2>
      <div>
        <div className="form-group">
          <label htmlFor="username">Username:</label>
          <input
            type="text"
            id="username"
            placeholder="Ingresa tu usuario"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            required
          />
        </div>
        <div className="form-group">
          <label htmlFor="password">Password:</label>
          <input
            type="password"
            id="password"
            placeholder="Ingresa tu contraseña"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            required
          />
        </div>
        <button onClick={handleLogin}>Iniciar Sesión</button>
      </div>
      {isLoading && <FaSpinner className="spinner" />} {/* Muestra el spinner si isLoading es true */}
    </div>
  );
};
