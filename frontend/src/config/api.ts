export const HISTORY_LENGTH = 60;

export const API_URL =
    import.meta.env.VITE_API_URL ||
    `http://${window.location.hostname}:3001`;

export const RAW_WS =
    import.meta.env.VITE_WS_URL ||
    `ws://${window.location.hostname}:3000`;

export const WS_URL = RAW_WS.endsWith("/ws")
    ? RAW_WS
    : `${RAW_WS.replace(/\/$/, "")}/ws`;
