package org.metuchenmomentum.marquee;

/**
 * A {@link DisplayConnection} implementation that does absolutely nothing whatsoever.
 */
public class VacuousDisplayConnection implements DisplayConnection {
    
    /**
     * Sends no characters anywhere.
     */
    @Override
    public int send(DisplayMessage message) {
        return 0;
    }
}
