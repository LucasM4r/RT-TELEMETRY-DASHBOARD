package handlers

import "net/http"

// Health handles GET requests to check the health status of the API.
func Health(w http.ResponseWriter, r *http.Request) {
	writeJSON(w, []byte(`{"status":"ok"}`))
}
