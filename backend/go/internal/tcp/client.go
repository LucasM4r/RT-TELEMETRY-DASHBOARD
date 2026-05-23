// Package tcp provides simple TCP client utilities.
package tcp

import (
	"bufio"
	"errors"
	"fmt"
	"net"
)

var ErrNotConnected = errors.New("client is not connected")

// Client represents a TCP client.
type Client struct {
	Host string
	Port string

	conn net.Conn
}

// NewClient creates a new TCP client instance.
func NewClient(host, port string) *Client {
	return &Client{
		Host: host,
		Port: port,
	}
}

// Connect establishes a TCP connection to the configured server.
func (c *Client) Connect() error {
	addr := net.JoinHostPort(c.Host, c.Port)

	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return fmt.Errorf("connect to %s: %w", addr, err)
	}

	c.conn = conn

	return nil
}

// IsConnected reports whether the client has an active connection.
func (c *Client) IsConnected() bool {
	return c.conn != nil
}

// Send writes a command to the connection.
func (c *Client) Send(cmd string) error {
	if !c.IsConnected() {
		return ErrNotConnected
	}

	_, err := c.conn.Write([]byte(cmd + "\n"))
	if err != nil {
		return fmt.Errorf("write command: %w", err)
	}

	return nil
}

// Close closes the client connection.
func (c *Client) Close() error {
	if c.conn == nil {
		return nil
	}

	if err := c.conn.Close(); err != nil {
		return fmt.Errorf("close connection: %w", err)
	}

	c.conn = nil

	return nil
}

// Listen continuously reads lines from the connection.
func (c *Client) Listen(out chan<- []byte) error {
	if !c.IsConnected() {
		return ErrNotConnected
	}

	defer close(out)

	scanner := bufio.NewScanner(c.conn)

	for scanner.Scan() {
		data := append([]byte(nil), scanner.Bytes()...)
		out <- data
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("listen connection: %w", err)
	}

	return nil
}

// Fetch sends a command and waits for a single line response.
func (c *Client) Fetch(cmd string) ([]byte, error) {
	if !c.IsConnected() {
		return nil, ErrNotConnected
	}

	if err := c.Send(cmd); err != nil {
		return nil, err
	}

	reader := bufio.NewReader(c.conn)

	data, err := reader.ReadBytes('\n')
	if err != nil {
		return nil, fmt.Errorf("read response: %w", err)
	}

	return data, nil
}
