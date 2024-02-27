// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAuth, onAuthStateChanged, signInWithEmailAndPassword, signOut  } from "firebase/auth";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: process.env.REACT_APP_API_KEY,
  authDomain: process.env.REACT_APP_AUTH_DOMAIN,
  projectId: process.env.REACT_APP_PROJECT_ID,
  storageBucket: process.env.REACT_APP_STORAGE_BUCKET,
  messagingSenderId: process.env.REACT_APP_MESSAGING_SENDER_ID,
  appId: process.env.REACT_APP_APP_ID,
  measurementId: process.env.REACT_APP_MEASUREMENT_ID
};
// Initialize Firebase
const app = initializeApp(firebaseConfig);
console.log('Firebase iniciado');

const auth = getAuth();

export const loginAdmin = async (mail, pass) => {
  let resp = null
  await signInWithEmailAndPassword(auth, mail, pass)
  .then((userCredential) => {
    // Signed in
    const user = userCredential.user;
    resp = true
    // ...
  })
  .catch((error) => {
    const errorCode = error.code;
    const errorMessage = error.message;
    console.log(error);
    resp = false
  });
  return resp
}

export const isAdminLogin = async () => {
  const auth = getAuth();
  
  return new Promise((resolve, reject) => {
    onAuthStateChanged(auth, (user) => {
      if (user) {
        // Usuario autenticado
        resolve(true);
      } else {
        // Usuario no autenticado
        resolve(false);
      }
    }, (error) => {
      // Maneja los errores aquí si es necesario
      reject(error);
    });
  });
};

export const logout = async () => {
  try {
    await signOut(auth);
    console.log("Se cerró sesión correctamente.");
    // Realiza cualquier acción adicional después de cerrar sesión
  } catch (error) {
    console.log("Error al cerrar sesión:", error.message);
  }
}

export default { isAdminLogin, loginAdmin, logout };