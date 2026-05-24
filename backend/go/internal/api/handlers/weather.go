package handlers

import (
	"encoding/json"
	"fmt"
	"net"
	"net/http"
	"strings"
	"time"
)

// IPLocationResponse represents the response from the IP geolocation API
type IPLocationResponse struct {
	Status string  `json:"status"`
	City   string  `json:"city"`
	Lat    float64 `json:"lat"`
	Lon    float64 `json:"lon"`
}

// OpenMeteoResponse represents the response from the Open-Meteo API
type OpenMeteoResponse struct {
	CurrentWeather struct {
		Temperature float64 `json:"temperature"`
		WeatherCode int     `json:"weathercode"`
	} `json:"current_weather"`
}

// WeatherResponse is the structure sent back to the React frontend
type WeatherResponse struct {
	City      string  `json:"city"`
	Temp      float64 `json:"temp"`
	Condition int     `json:"condition"`
}

// getIP extracts the client's IP address from the request.
func getIP(r *http.Request) string {
	forwarded := r.Header.Get("X-Forwarded-For")
	if forwarded != "" {
		return strings.Split(forwarded, ",")[0]
	}
	realIP := r.Header.Get("X-Real-IP")
	if realIP != "" {
		return realIP
	}
	ip, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		return r.RemoteAddr
	}
	return ip
}

// isPrivateIP checks if the given IP address is private (e.g., 192.168.x.x, 10.x.x.x, etc.)
func isPrivateIP(ipStr string) bool {
	ip := net.ParseIP(ipStr)
	if ip == nil {
		return false
	}
	return ip.IsLoopback() || ip.IsPrivate() || ip.IsLinkLocalUnicast()
}

// GetWeather handles GET requests to fetch the current weather based on the client's IP location.
func GetWeather(w http.ResponseWriter, r *http.Request) {
	client := &http.Client{Timeout: 5 * time.Second}

	ip := getIP(r)

	locURL := "http://ip-api.com/json/"

	// Only append the IP to the URL if it's not private, otherwise it will return an error
	if !isPrivateIP(ip) {
		locURL += ip
	}

	locResp, err := client.Get(locURL)
	if err != nil {
		http.Error(w, "Error when fetching location", http.StatusServiceUnavailable)
		return
	}
	defer locResp.Body.Close()

	var locData IPLocationResponse
	// Check if the API call was successful and if the response can be decoded properly
	if err := json.NewDecoder(locResp.Body).Decode(&locData); err != nil || locData.Status != "success" {
		fmt.Printf("[Weather Error] IP-API failed for IP %s\n", ip)
		http.Error(w, "Error decoding location", http.StatusInternalServerError)
		return
	}

	weatherURL := fmt.Sprintf("https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current_weather=true", locData.Lat, locData.Lon)

	// Fetch weather data from Open-Meteo API
	weatherResp, err := client.Get(weatherURL)
	if err != nil {
		http.Error(w, "Error when fetching weather", http.StatusServiceUnavailable)
		return
	}
	defer weatherResp.Body.Close()

	var meteoData OpenMeteoResponse
	if err := json.NewDecoder(weatherResp.Body).Decode(&meteoData); err != nil {
		http.Error(w, "Error decoding weather data", http.StatusInternalServerError)
		return
	}
	reactResponse := WeatherResponse{
		City:      locData.City,
		Temp:      meteoData.CurrentWeather.Temperature,
		Condition: meteoData.CurrentWeather.WeatherCode,
	}

	responseBytes, err := json.Marshal(reactResponse)
	if err != nil {
		http.Error(w, "Error encoding weather response", http.StatusInternalServerError)
		return
	}

	writeJSON(w, responseBytes)
}
