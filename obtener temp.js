const axios = require("axios");
const tomorrowApiKey = "S24k73g8QOHpyywxzjomD3fyAHodaYN5";
const ubidotsToken = "BBUS-SPupuFhC6nBfigumjWpgANguDI2ccG";
const location = "-38.7359,-72.5904"; // Coordenadas de Temuco
const deviceLabel = "weather_station";

// Obtener datos de Tomorrow.io
function obtenerDatosDeTomorrow() {
  return axios.get(`https://api.tomorrow.io/v4/timelines`, {
    params: {
      apikey: tomorrowApiKey,
      location: location,
      fields: ["temperature"],
      timesteps: "current",
      units: "metric",
    },
  });
}

// Enviar datos a Ubidots
function enviarDatosAUbidots(weatherData) {
  const ubidotsData = {
    temperature: weatherData.temperature,
  };

  return axios.post(
    `https://industrial.api.ubidots.com/api/v1.6/devices/${deviceLabel}`,
    ubidotsData,
    {
      headers: {
        "X-Auth-Token": ubidotsToken,
        "Content-Type": "application/json",
      },
    }
  );
}

// Flujo combinado
function ejecutarFlujo() {
  obtenerDatosDeTomorrow()
    .then((response) => {
      const weatherData = response.data.data.timelines[0].intervals[0].values;
      console.log("Datos de Tomorrow.io recibidos:", weatherData);

      return enviarDatosAUbidots(weatherData);
    })
    .then((response) => {
      console.log("Datos enviados a Ubidots:", response.data);
    })
    .catch((error) => {
      console.error("Error en el proceso:", error.response?.data || error.message);
    });
}

// Ejecutar el flujo cada 10 segundos
setInterval(ejecutarFlujo, 10000);
