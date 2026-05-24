package websocket

import (
	"encoding/binary"
	"io"
	"net"
)

func readFrame(conn net.Conn) (string, error) {
	header := make([]byte, 2)
	if _, err := io.ReadFull(conn, header); err != nil {
		return "", err
	}

	// Extract the opcode, masked bit, and payload length from the header
	opcode := header[0] & 0x0F
	masked := header[1] >> 7
	length := int(header[1] & 0x7F)

	// Handle control frames (Close=0x8, Ping=0x9, Pong=0xA)
	if opcode == 0x8 {
		return "", io.EOF
	}

	// Read the extended payload length based on the initial length value
	switch length {
	case 126:
		// Read the extended payload length (16 bits)
		ext := make([]byte, 2)
		_, err := io.ReadFull(conn, ext)
		if err != nil {
			return "", err
		}
		length = int(binary.BigEndian.Uint16(ext))
	case 127:
		// Read the extended payload length (64 bits)
		ext := make([]byte, 8)
		_, err := io.ReadFull(conn, ext)
		if err != nil {
			return "", err
		}
		length = int(binary.BigEndian.Uint64(ext))
	}

	// Read the masking key if the payload is masked
	var maskKey [4]byte
	if masked == 1 {
		if _, err := io.ReadFull(conn, maskKey[:]); err != nil {
			return "", err
		}
	}

	// Read the payload data
	payload := make([]byte, length)
	if _, err := io.ReadFull(conn, payload); err != nil {
		return "", err
	}

	// Unmask the payload if it is masked
	if masked == 1 {
		for i := 0; i < length; i++ {
			payload[i] ^= maskKey[i%4]
		}
	}

	// Only handle text frames (opcode 0x1) for simplicity
	if opcode != 1 {
		return "", nil
	}

	return string(payload), nil
}

// writeFrame sends a text frame to the client with the given message.
func writeFrame(conn net.Conn, msg string) error {
	payload := []byte(msg)
	length := len(payload)

	var header []byte
	header = append(header, 0x81)

	switch {
	case length <= 125:
		// For payloads of length 125 bytes or less, the length is encoded directly in the second byte
		header = append(header, byte(length))
	case length <= 65535:
		// For payloads of length 126 bytes to 65535 bytes, the second byte is set to 126 and the actual length is encoded in the next 2 bytes
		header = append(header, 126)
		b := make([]byte, 2)
		binary.BigEndian.PutUint16(b, uint16(length))
		header = append(header, b...)
	default:
		// For payloads of length greater than 65535 bytes, the second byte is set to 127 and the actual length is encoded in the next 8 bytes
		header = append(header, 127)
		b := make([]byte, 8)
		binary.BigEndian.PutUint64(b, uint64(length))
		header = append(header, b...)
	}

	_, err := conn.Write(append(header, payload...))
	return err
}
