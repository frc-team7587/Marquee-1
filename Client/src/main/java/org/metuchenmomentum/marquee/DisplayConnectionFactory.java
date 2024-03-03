package org.metuchenmomentum.marquee;

import java.nio.file.Path;
import java.util.List;

/**
 * A factory that builds {@link DeviceConnection} instances.
 */
public class DisplayConnectionFactory {
    /**
     * Connect to the Marquee over a serial USB cable.
     * 
     * @return a {@link UsbDisplayConnection} if it was possible to connect,
     *         a {@link VacuousDisplayConnection} otherwise.
     */
    public static DisplayConnection usbConnection() {
        List<Path> usbSerialConnections = findSerialConnections();
        return connectionFromUsbPaths(usbSerialConnections);
    }


    private static DisplayConnection connectionFromUsbPaths(List<Path> usbDevicePaths) {
        DisplayConnection connection = null;
        switch(usbDevicePaths.size()) {
            case 0:
                System.out.println("Marquee not connected, no serial USB devices found.");
                break;
            case 1:
                System.out.println("Found one USB Serial device; opening connection");
               connection =  UsbDisplayConnection.builder(usbDevicePaths
                    .get(0))
                    .build();
                break;
            default:
                System.out.println("Multiple serial devices found, cannot connect.");
                break;
        }


        if (connection == null) {
            connection = new VacuousDisplayConnection();
        }

        return connection;
    }

    private static List<Path> findSerialConnections() {
        UsbSerialDeviceList usbSerialDevices = new UsbSerialDeviceList();
        boolean deviceListStatus = usbSerialDevices.start();
        System.out.println("USB serial device initialization returns: " + deviceListStatus);
        System.out.println("Devices:");
        List<Path> usbDevicePaths = usbSerialDevices.getDevices();
        for (Path device : usbDevicePaths) {
            System.out.println("    " + device);
        }
        return usbDevicePaths;
    }
}
