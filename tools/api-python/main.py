import sys
import openmeteo_requests
import pandas as pd
import requests_cache
from retry_requests import retry

#https://open-meteo.com/en/docs?hourly=rain%2Cprecipitation_probability%2Cprecipitation%2Ctemperature_2m%2Cwind_speed_10m&past_days=1&daily=temperature_2m_max%2Ctemperature_2m_min%2Cwind_speed_10m_max%2Cprecipitation_probability_max%2Cweather_code


# Check if the correct number of arguments (script_name + lat + lon) exists
if len(sys.argv) != 3:
    print("Error: You must provide exactly two arguments: Latitude and Longitude.")
    print("Usage: python your_script.py <latitude> <longitude>")
    sys.exit(1)

try:
    # Convert arguments to floats
    lat = float(sys.argv[1])
    lon = float(sys.argv[2])
except ValueError:
    print("Error: Latitude and Longitude must be valid numbers.")
    sys.exit(1)

# specific error handling for all zeroes (0, 0) as requested
if lat == 0 and lon == 0:
    print("Error: Invalid location. Coordinates (0, 0) are not allowed.")
    sys.exit(1)

print(f"Starting forecast fetch for Coordinates: {lat}, {lon}...")


# Setup the Open-Meteo API client with cache and retry on error
cache_session = requests_cache.CachedSession('.cache', expire_after = 3600)
retry_session = retry(cache_session, retries = 4, backoff_factor = 0.2)
openmeteo = openmeteo_requests.Client(session = retry_session)

# Make sure all required weather variables are listed here
# The order of variables in hourly or daily is important to assign them correctly below
url = "https://api.open-meteo.com/v1/forecast"
params = {
    "latitude": lat,  # <--- Updated to use CLI argument
    "longitude": lon, # <--- Updated to use CLI argument
    "daily": ["temperature_2m_max", "temperature_2m_min", "wind_speed_10m_max", "precipitation_probability_max", "weather_code"],
    "hourly": ["rain", "precipitation_probability", "precipitation", "temperature_2m", "wind_speed_10m"],
    "past_days": 1,
}
responses = openmeteo.weather_api(url, params=params)

# Process first location. Add a for-loop for multiple locations or weather models
response = responses[0]
print(f"Coordinates: {response.Latitude()}°N {response.Longitude()}°E")
print(f"Elevation: {response.Elevation()} m asl")
print(f"Timezone difference to GMT+0: {response.UtcOffsetSeconds()}s")

# Process hourly data. The order of variables needs to be the same as requested.
hourly = response.Hourly()
hourly_rain = hourly.Variables(0).ValuesAsNumpy()
hourly_precipitation_probability = hourly.Variables(1).ValuesAsNumpy()
hourly_precipitation = hourly.Variables(2).ValuesAsNumpy()
hourly_temperature_2m = hourly.Variables(3).ValuesAsNumpy()
hourly_wind_speed_10m = hourly.Variables(4).ValuesAsNumpy()

hourly_data = {"date": pd.date_range(
    start = pd.to_datetime(hourly.Time(), unit = "s", utc = True),
    end =  pd.to_datetime(hourly.TimeEnd(), unit = "s", utc = True),
    freq = pd.Timedelta(seconds = hourly.Interval()),
    inclusive = "left"
)}

hourly_data["rain"] = hourly_rain
hourly_data["precipitation_probability"] = hourly_precipitation_probability
hourly_data["precipitation"] = hourly_precipitation
hourly_data["temperature_2m"] = hourly_temperature_2m
hourly_data["wind_speed_10m"] = hourly_wind_speed_10m

hourly_dataframe = pd.DataFrame(data = hourly_data)
print("\nHourly data\n", hourly_dataframe)

# Process daily data. The order of variables needs to be the same as requested.
daily = response.Daily()
daily_temperature_2m_max = daily.Variables(0).ValuesAsNumpy()
daily_temperature_2m_min = daily.Variables(1).ValuesAsNumpy()
daily_wind_speed_10m_max = daily.Variables(2).ValuesAsNumpy()
daily_precipitation_probability_max = daily.Variables(3).ValuesAsNumpy()
daily_weather_code = daily.Variables(4).ValuesAsNumpy()

daily_data = {"date": pd.date_range(
    start = pd.to_datetime(daily.Time(), unit = "s", utc = True),
    end =  pd.to_datetime(daily.TimeEnd(), unit = "s", utc = True),
    freq = pd.Timedelta(seconds = daily.Interval()),
    inclusive = "left"
)}

daily_data["temperature_2m_max"] = daily_temperature_2m_max
daily_data["temperature_2m_min"] = daily_temperature_2m_min
daily_data["wind_speed_10m_max"] = daily_wind_speed_10m_max
daily_data["precipitation_probability_max"] = daily_precipitation_probability_max
daily_data["weather_code"] = daily_weather_code

daily_dataframe = pd.DataFrame(data = daily_data)
print("\nDaily data\n", daily_dataframe)