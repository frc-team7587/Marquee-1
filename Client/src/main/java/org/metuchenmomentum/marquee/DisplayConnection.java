package org.metuchenmomentum.marquee;

/**
 * Message sending API.
 */
public interface DisplayConnection {
    /**
     * Sends the specified {@code message} to the ESP32
     * 
     * @param message message to send
     * @return the number of bytes sent. Should be 128..
     */
    int send(DisplayMessage message);
}
