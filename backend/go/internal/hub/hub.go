package hub

import "net"

// Hub manages the set of active clients and broadcasts messages to the clients.
type Hub struct {
	Clients    map[*Client]bool
	Broadcast  chan []byte
	Register   chan *Client
	Unregister chan *Client
}

// Client represents a single WebSocket connection.
type Client struct {
	Conn net.Conn
	Send chan []byte
}

// NewHub initializes a new Hub instance.
func NewHub() *Hub {
	return &Hub{
		Clients:    make(map[*Client]bool),
		Broadcast:  make(chan []byte),
		Register:   make(chan *Client),
		Unregister: make(chan *Client),
	}
}

// Run starts the main loop for the Hub, handling client registration, unregistration, and broadcasting messages.
func (h *Hub) Run() {
	for {
		select {
		case client := <-h.Register:
			h.addClient(client)

		case client := <-h.Unregister:
			h.removeClient(client)

		case message := <-h.Broadcast:
			h.broadcastMessage(message)
		}
	}
}

// addClient adds a new client to the Hub.
func (h *Hub) addClient(client *Client) {
	h.Clients[client] = true
}

// removeClient removes a client from the Hub and closes its send channel.
func (h *Hub) removeClient(client *Client) {
	if _, ok := h.Clients[client]; ok {
		delete(h.Clients, client)
		close(client.Send)
	}
}

// broadcastMessage sends a message to all connected clients.
func (h *Hub) broadcastMessage(message []byte) {
	for client := range h.Clients {
		select {
		case client.Send <- message:
		default:
			close(client.Send)
			delete(h.Clients, client)
		}
	}
}
