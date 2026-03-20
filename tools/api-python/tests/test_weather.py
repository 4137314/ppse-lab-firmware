import pytest
import requests_mock
from pcb_tool.core.weather import fetch_weather, OPEN_METEO_URL

def test_fetch_weather_success():
    """Verifica che la funzione parsi correttamente una risposta JSON valida."""
    mock_response = {
        "current": {
            "temperature_2m": 15.5,
            "relative_humidity_2m": 60,
            "wind_speed_10m": 10.2,
            "weather_code": 1
        },
        "hourly": {
            "time": ["2026-03-20T12:00"],
            "temperature_2m": [15.5] * 168,
            "weather_code": [1] * 168
        }
    }

    with requests_mock.Mocker() as m:
        # Istruiamo il mock a rispondere con i nostri dati finti quando viene chiamato l'URL
        m.get(OPEN_METEO_URL, json=mock_response)
        
        data = fetch_weather(46.06, 11.12)
        
        assert data["current"]["temperature_2m"] == 15.5
        assert len(data["hourly"]["temperature_2m"]) == 168
        assert m.called  # Verifica che la chiamata sia stata effettivamente fatta

def test_fetch_weather_network_error():
    """Verifica che la funzione gestisca correttamente gli errori di rete (es. 404)."""
    with requests_mock.Mocker() as m:
        m.get(OPEN_METEO_URL, status_code=404)
        
        # Ci aspettiamo che requests sollevi un'eccezione
        with pytest.raises(Exception):
            fetch_weather(0, 0)
